#include "bsdf.h"
#include "utils.h"

#include <utility>

IdealDiffusion::IdealDiffusion(Vec3f color) : color(std::move(color)) {}

Vec3f IdealDiffusion::evaluate(Interaction &interaction) const {
    return Vec3f{INV_PI, INV_PI, INV_PI}.cwiseProduct(color);
}

float IdealDiffusion::pdf(Interaction &interaction) const {
    Vec3f dir = interaction.wi;
    float theta = std::acos(dir.z());
    return std::cos(theta) / PI;
}

float IdealDiffusion::sample(Interaction &interaction, Sampler &sampler) const {
    Vec2f samples = sampler.get2D();
    float xi_1 = samples.x();
    float xi_2 = samples.y();
    float theta = std::acos(std::sqrt(1 - xi_1));
    float phi = 2 * PI * xi_2;
    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);
    interaction.wi = Vec3f(x, y, z);
    float PDF = pdf(interaction);
    Mat3f R = Eigen::Quaternion<float>::FromTwoVectors(Vec3f(0, 0, 1), interaction.normal).toRotationMatrix();
    interaction.wi = (R * interaction.wi).normalized();
    return PDF;
}

/// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
    return false;
}

Vec3f IdealSpecular::evaluate(Interaction &interaction) const {
    float cosine = interaction.wi.dot(interaction.normal.normalized());
    return Vec3f{1, 1, 1} / cosine;
}

bool IdealSpecular::isDelta() const {
    return true;
}

float IdealSpecular::sample(Interaction &interaction, Sampler &sampler) const {
    interaction.wi = (interaction.wo + 2 * (-interaction.wo).dot(interaction.normal) * interaction.normal).normalized();
    return 1.0f;
}

float IdealSpecular::pdf(Interaction &interaction) const {
    return 1.0f;
}
