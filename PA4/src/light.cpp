#include "light.h"

#include <utility>
#include <iostream>
#include "utils.h"

Light::Light(Vec3f pos, Vec3f color) :
        position(std::move(pos)), radiance(std::move(color)) {}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &size) :
        Light(pos, color), size(size) {
    Vec3f v1, v2, v3, v4;
    v1 = pos + Vec3f(size.x() / 2, 0.f, -size.y() / 2);
    v2 = pos + Vec3f(-size.x() / 2, 0.f, -size.y() / 2);
    v3 = pos + Vec3f(-size.x() / 2, 0.f, size.y() / 2);
    v4 = pos + Vec3f(size.x() / 2, 0.f, size.y() / 2);
    light_mesh = TriangleMesh({v1, v2, v3, v4}, {Vec3f(0, -1, 0)}, {0, 1, 2, 0, 2, 3}, {0, 0, 0, 0, 0, 0});
}

Vec3f SquareAreaLight::emission(const Vec3f &pos, const Vec3f &dir) const {
    float cosine = -dir.dot(Vec3f(0, -1, 0));
    return radiance * std::max(cosine, 0.0f);
}

float SquareAreaLight::pdf(const Interaction &interaction, Vec3f pos) {
    return 1 / (size.x() * size.y());
}

Vec3f SquareAreaLight::sample(Interaction &interaction, float *pdf, Sampler &sampler) const {
    Vec2f coef = sampler.get2D();
    coef = coef * 2 - Vec2f(1, 1);
    return position + coef.x() * Vec3f(1, 0, 0) * size.x() / 2 + coef.y() * Vec3f(0, 0, 1) * size.y() / 2;
}

bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
    if (light_mesh.intersect(ray, interaction)) {
        interaction.type = Interaction::Type::LIGHT;
        return true;
    }
    return false;
}

