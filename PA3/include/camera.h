#ifndef CS171_HW3_INCLUDE_CAMERA_H_
#define CS171_HW3_INCLUDE_CAMERA_H_

#include "core.h"
#include "ray.h"
#include "image.h"

class Camera {
public:
    Camera();

    std::vector<Ray> generateRay(float x, float y, int sample_freq = 2, float angle = 22.6 * PI / 180.f);

    void lookAt(const Vec3f &look_at, const Vec3f &ref_up = {0, 0, 1});

    void setPosition(const Vec3f &pos);

    [[nodiscard]] Vec3f getPosition() const;

    void setFov(float new_fov);

    [[nodiscard]] float getFov() const;

    void setImage(std::shared_ptr<ImageRGB> &img);

    [[nodiscard]] std::shared_ptr<ImageRGB> &getImage();

private:
    Vec3f position;
    Vec3f forward;
    Vec3f up;
    Vec3f right;
    float focal_len;
    float fov;

    std::shared_ptr<ImageRGB> image;
};

#endif //CS171_HW3_INCLUDE_CAMERA_H_
