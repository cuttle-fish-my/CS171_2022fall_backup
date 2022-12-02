#include "integrator.h"
#include "utils.h"
#include <omp.h>

#include <utility>
#include <iostream>

Integrator::Integrator(std::shared_ptr<Camera> cam,
                       std::shared_ptr<Scene> scene, int spp, int max_depth)
        : camera(std::move(cam)), scene(std::move(scene)), spp(spp), max_depth(max_depth) {
}

void Integrator::render() const {
    Vec2i resolution = camera->getImage()->getResolution();
    int cnt = 0;
    Sampler sampler;
#pragma omp parallel for schedule(dynamic), default(none), shared(resolution, cnt), private(sampler)
    for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0,10000);
//        sampler.setSeed(omp_get_thread_num());
        sampler.setSeed(dist(rd));
        int num_sample = (int) std::sqrt(spp);
        for (int dy = 0; dy < resolution.y(); dy++) {
            Vec3f L(0, 0, 0);
            // TODO: generate #spp rays for each pixel and use Monte Carlo integration to compute radiance.
            for (int i = 0; i < num_sample; ++i) {
                for (int j = 0; j < num_sample; ++j) {
                    Ray ray = camera->generateRay((float) dx + (float) i / (float) num_sample,
                                                  (float) dy + (float) j / (float) num_sample);
                    L += radiance(ray, sampler);
                }
            }
            L = L / spp;
            camera->getImage()->setPixel(dx, dy, L);
        }
    }
}

Vec3f Integrator::radiance(Ray &ray, Sampler &sampler) const {
    Vec3f L(0, 0, 0);
    Vec3f beta(1, 1, 1);
    bool isDelta = false;
    for (int i = 0; i < max_depth; ++i) {
        /// Compute radiance (direct + indirect)
        Interaction interaction{};
        if (!scene->intersect(ray, interaction)) break;
        interaction.wo = ray.direction;
        if (i == 0 && interaction.type == Interaction::Type::LIGHT) {
            return scene->getLight()->emission(Vec3f(0, 0, 0), interaction.wo);
        }
        if (interaction.type == Interaction::Type::LIGHT) break;

        L += beta.cwiseProduct(directLighting(interaction, sampler));

        float pdf = interaction.material->sample(interaction, sampler);
        Vec3f BSDF = interaction.material->evaluate(interaction);
        float cosine = interaction.wi.dot(interaction.normal.normalized());
        beta = beta.cwiseProduct(BSDF * cosine / pdf);

        ray = Ray(interaction.pos, interaction.wi);
    }
    return L;
}

Vec3f Integrator::directLighting(Interaction &interaction, Sampler &sampler) const {
    Vec3f L(0, 0, 0);
    // Compute direct lighting.

    if (!interaction.material->isDelta()) {
        std::shared_ptr<Light> light = scene->getLight();
        float pdf = light->pdf(interaction, Vec3f(0, 0, 0));
        std::shared_ptr<BSDF> material = interaction.material;
        Vec3f light_sample = light->sample(interaction, nullptr, sampler);
        Ray shadow_ray(interaction.pos, (light_sample - interaction.pos).normalized());
        if (!scene->isShadowed(shadow_ray)) {
            float cosine = shadow_ray.direction.dot(interaction.normal.normalized());
            L = light->emission(Vec3f(), shadow_ray.direction).cwiseProduct(material->evaluate(interaction)) * cosine /
                std::pow((light_sample - interaction.pos).norm(), 2) / pdf;
        }
    }
    return L;
}