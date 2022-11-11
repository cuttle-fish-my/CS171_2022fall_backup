#include "light.h"

#include <utility>

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(Vec3f pos, Vec3f color) :
        position(std::move(pos)), color(std::move(color)) {}

Vec3f Light::getColor() const {
    return color;
}

std::vector<LightSample> Light::getSamples() const {
    return sample;
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension,
                                 const Vec3f &normal,
                                 const Vec3f &tangent) :
        Light(pos, color),
        rectangle(pos, dimension, normal, tangent) {
    Vec2f size = rectangle.getSize();
    float x_min = position.x() - size.x() / 2 + size.x() / (SAMPLE_NUM) / 2;
    float y_min = position.y() - size.y() / 2 + size.y() / (SAMPLE_NUM) / 2;
    for (int i = 0; i < SAMPLE_NUM; ++i) {
        for (int j = 0; j < SAMPLE_NUM; ++j) {
            Vec3f POS{
                    x_min + (float) i * size.x() / (SAMPLE_NUM),
                    y_min + (float) j * size.y() / (SAMPLE_NUM),
                    position.z()
            };
            sample.push_back(LightSample{color, POS});
        }
    }
}


bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here.
    float origin_dist = interaction.dist;
    bool isIntersect = rectangle.intersect(ray, interaction);
    if (isIntersect && interaction.dist < origin_dist) {
        interaction.type = interaction.Type::LIGHT;
    }
    return isIntersect;
}




