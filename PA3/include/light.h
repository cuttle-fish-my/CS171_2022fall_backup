#ifndef CS171_HW3_INCLUDE_LIGHT_H_
#define CS171_HW3_INCLUDE_LIGHT_H_

#include <vector>

#include "core.h"
#include "ray.h"
#include "geometry.h"

struct LightSample {
    Vec3f color;
    Vec3f position;
};

class Light {
public:
    explicit Light(Vec3f pos, Vec3f color);

    virtual ~Light() = default;

    virtual bool intersect(Ray &ray, Interaction &interaction) const = 0;

    [[nodiscard]] Vec3f getColor() const;

    [[nodiscard]] std::vector<LightSample> getSamples() const;


protected:
    /// position of light in world space
    Vec3f position;
    /// RGB color of the light
    Vec3f color;

    std::vector<LightSample> sample;

};

class SquareAreaLight : public Light {
public:
    explicit SquareAreaLight(const Vec3f &pos,
                             const Vec3f &color,
                             const Vec2f &dimension,
                             const Vec3f &normal,
                             const Vec3f &tangent);


    bool intersect(Ray &ray, Interaction &interaction) const override;


protected:
    Rectangle rectangle;
};

#endif //CS171_HW3_INCLUDE_LIGHT_H_
