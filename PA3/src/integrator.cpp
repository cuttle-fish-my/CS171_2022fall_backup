#include "integrator.h"
#include <omp.h>

#include <utility>
#include <iostream>

PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> cam,
                                                 std::shared_ptr<Scene> scene)
        : camera(std::move(cam)), scene(std::move(scene)) {
}

void PhongLightingIntegrator::render() const {
    Vec2i resolution = camera->getImage()->getResolution();
    int cnt = 0;
#pragma omp parallel for schedule(guided, 2), default(none), shared(cnt, resolution)
    for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        for (int dy = 0; dy < resolution.y(); dy++) {
            Vec3f L(0, 0, 0);
            int sample_freq = 1;
            float angle = 26.6 * PI / 180.f;
//            float angle = 0;
            std::vector<Ray> rays = camera->generateRay((float) dx, (float) dy, sample_freq, angle);
            for (auto &ray: rays) {
                Interaction interaction{};
                if (scene->intersect(ray, interaction)) {
                    L += radiance(ray, interaction);
                }
            }
            camera->getImage()->setPixel(dx, dy, L / (sample_freq * sample_freq));
        }
    }
}

Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction) const {
    Vec3f radiance(0, 0, 0);
    std::shared_ptr<Light> light = scene->getLight();
    std::vector<LightSample> samples = light->getSamples();
    if (interaction.type == interaction.Type::LIGHT) {
        return light->getColor();
    }
    Vec3f ambient = scene->getAmbient().cwiseProduct(interaction.model.ambient);
    Vec3f diffuse{0, 0, 0};
    Vec3f specular{0, 0, 0};
    for (auto &sample: samples) {
        Ray shadow_ray{interaction.pos, (sample.position - interaction.pos).normalized()};
        shadow_ray.t_max = (sample.position - interaction.pos).norm() / shadow_ray.direction.norm();
        Interaction new_interaction{};
        if (!scene->isShadowed(shadow_ray)) {
            Vec3f lightDir = shadow_ray.direction;
            Vec3f viewDir = -ray.direction;
            float diff = std::max(interaction.normal.dot(lightDir), 0.0f);
            diffuse += diff * interaction.model.diffusion;
            if (lightDir.dot(interaction.normal) > 0) {
                Vec3f reflectDir = -lightDir - 2.0f * interaction.normal.dot(-lightDir) * interaction.normal;
                float spec = std::pow(std::max(viewDir.dot(reflectDir), 0.0f), interaction.model.shininess);
                specular += spec * interaction.model.specular;
            }
        }
    }

    radiance = (diffuse + specular).cwiseProduct(light->getColor()) / samples.size() +
               ambient.cwiseProduct(light->getColor());
    return radiance;
}
