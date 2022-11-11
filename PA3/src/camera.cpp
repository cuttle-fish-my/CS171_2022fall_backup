#include "camera.h"

#include <cmath>

Camera::Camera()
        : position(0, -1, 0), fov(45), focal_len(1) {
    lookAt({0, 0, 0}, {0, 0, 1});
}

std::vector<Ray> Camera::generateRay(float dx, float dy, int sample_freq, float angle) {
    // TODO: Your code here
    // You need to generate ray according to screen coordinate (dx, dy)
    std::vector<Ray> rays;
    Vec2i resolution = image->getResolution();
    Vec2f center{dx + 0.5f, dy + 0.5f};
    Eigen::Matrix2f rotation_matrix;
    rotation_matrix << std::cos(angle), -std::sin(angle), std::sin(angle), std::cos(angle);
    float ratio = (float) resolution.x() / (float) resolution.y();
    float x, y;
    for (int i = 0; i < sample_freq; ++i) {
        float new_dx = (float) i / (float) (sample_freq) - 0.5f;

        for (int j = 0; j < sample_freq; ++j) {
            float new_dy = (float) j / (float) (sample_freq) - 0.5f;

            Vec2f rotated_coordinate = rotation_matrix * Vec2f{new_dx, new_dy} + center;

            x = (2 * rotated_coordinate.x() / (float) resolution.x() - 1) * focal_len * ratio *
                std::tan(getFov() * PI / 180.f / 2);

            y = (2 * rotated_coordinate.y() / (float) resolution.y() - 1) * focal_len *
                std::tan(getFov() * PI / 180.f / 2);

            Vec3f ray_dir = forward + x * right + y * up;
            rays.emplace_back(getPosition(), ray_dir.normalized());
        }
    }
    return rays;
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
    // TODO: Your code here
    forward = (look_at - position).normalized();
    right = forward.cross(ref_up).normalized();
    up = right.cross(forward).normalized();
}

void Camera::setPosition(const Vec3f &pos) {
    position = pos;
}

Vec3f Camera::getPosition() const {
    return position;
}

void Camera::setFov(float new_fov) {
    fov = new_fov;
}

float Camera::getFov() const {
    return fov;
}

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
    image = img;
}

std::shared_ptr<ImageRGB> &Camera::getImage() {
    return image;
}

