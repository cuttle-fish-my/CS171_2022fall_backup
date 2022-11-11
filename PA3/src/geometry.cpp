#include "geometry.h"

#include <utility>
#include <iostream>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
        : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
    normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
    Vec3f d = ray.direction;
    Vec3f o = ray.origin;
    if (d.dot(normal) == 0) {
        if ((o - v0).dot(normal) == 0) {
            return true;
        } else {
            return false;
        }
    }
    Vec3f s = o - v0;
    Vec3f e1 = v1 - v0;
    Vec3f e2 = v2 - v0;
    Vec3f s1 = d.cross(e2);
    Vec3f s2 = s.cross(e1);
    Vec3f res = (1 / s1.dot(e1)) * Vec3f(s2.dot(e2), s1.dot(s), s2.dot(d));
    float t = res.x(), b1 = res.y(), b2 = res.z();
    if (t < RAY_DEFAULT_MIN || t > RAY_DEFAULT_MAX) return false;
    if (b1 >= 0 && b2 >= 0 && (b1 + b2) <= 1) {
        if (interaction.dist > t) {
            interaction.dist = t;
            interaction.type = interaction.Type::GEOMETRY;
            interaction.normal = normal.normalized();
            interaction.pos = ray(t);
            interaction.model = material->evaluate(interaction);
        }
        return true;
    }
    return false;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, const Vec3f &normal, const Vec3f &tangent)
        : Geometry(),
          position(std::move(position)),
          size(std::move(dimension)),
          normal(normal.normalized()),
          tangent(tangent.normalized()),
          cotangent(normal.cross(tangent).normalized()) {
    coordinate_transfer << tangent.x(), cotangent.x(), normal.x(),
            tangent.y(), cotangent.y(), normal.y(),
            tangent.z(), cotangent.z(), normal.z();
}

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
    float t = normal.dot(position - ray.origin) / normal.dot(ray.direction);
    if (t < RAY_DEFAULT_MIN || t > RAY_DEFAULT_MAX) return false;
    Vec3f p = ray(t);
    if (std::abs(tangent.dot(p - position)) <= size.x() / 2 &&
        std::abs(cotangent.dot(p - position)) <= size.y() / 2) {
        if (interaction.dist > t) {
            interaction.dist = t;
            interaction.pos = p;
            interaction.normal = normal.normalized();
            interaction.type = interaction.Type::GEOMETRY;
            interaction.uv =
                    (Vec2f{(p - position).dot(tangent) / (size.x() / 2), (p - position).dot(cotangent) / (size.y() / 2)} +
                     Vec2f{1.f, 1.f}) / 2;
            Vec3f tmp_norm = interaction.normal;
            if (material) {
                interaction.model = material->evaluate(interaction);
                if (interaction.normal != tmp_norm) {
                    interaction.normal = (coordinate_transfer * interaction.normal).normalized();
                }
                if (interaction.pos != p) {
                    interaction.pos = interaction.pos.cwiseProduct(normal);
                    interaction.pos += p;
                }
            }
        }
        return true;
    }
    return false;
}

Vec2f Rectangle::getSize() const {
    return size;
}

Vec3f Rectangle::getNormal() const {
    return normal;
}

Vec3f Rectangle::getTangent() const {
    return tangent;
}

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c)
        : p(p), a(a), b(b), c(c) {
    Eigen::Matrix4f T, R, S;
    T << 1, 0, 0, p.x(),
            0, 1, 0, p.y(),
            0, 0, 1, p.z(),
            0, 0, 0, 1;
    Vec3f norm_a = a.normalized();
    Vec3f norm_b = b.normalized();
    Vec3f norm_c = c.normalized();
    R << norm_a.x(), norm_b.x(), norm_c.x(), 0,
            norm_a.y(), norm_b.y(), norm_c.y(), 0,
            norm_a.z(), norm_b.z(), norm_c.z(), 0,
            0, 0, 0, 1;
    S << a.norm(), 0, 0, 0,
            0, b.norm(), 0, 0,
            0, 0, c.norm(), 0,
            0, 0, 0, 1;
    M = T * R * S;
    M_inverse = M.inverse();
    M_transpose_inverse = M.transpose().inverse();
}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const {
    Vec4f new_origin{ray.origin.x(), ray.origin.y(), ray.origin.z(), 1};
    Vec4f new_direction{ray.direction.x(), ray.direction.y(), ray.direction.z(), 0};
    new_origin = M_inverse * new_origin;
    new_direction = M_inverse * new_direction;
    Ray new_ray{Vec3f{new_origin.x(), new_origin.y(), new_origin.z()},
                Vec3f{new_direction.x(), new_direction.y(), new_direction.z()}.normalized()};
    Vec3f L = -new_ray.origin;
    float t_ca = L.dot(new_ray.direction);
    float d_square = L.norm() * L.norm() - t_ca * t_ca;
    if (t_ca < 0 || d_square > 1) {
        return false;
    }
    float t_hc = std::sqrt(1 - d_square);
    float t = (t_ca - t_hc) / new_direction.norm();
    float t_prime = (t_ca + t_hc) / new_direction.norm();
    if ((t < RAY_DEFAULT_MIN && t_prime < RAY_DEFAULT_MIN) || t > RAY_DEFAULT_MAX) {
        return false;
    }
    if (interaction.dist > t) {
        interaction.dist = t;
        interaction.pos = ray(t);
        Vec3f new_pos = new_ray(t_ca - t_hc);
        Vec4f normal = M_transpose_inverse * Vec4f{new_pos.x(), new_pos.y(), new_pos.z(), 0};
        interaction.normal = Vec3f{normal.x(), normal.y(), normal.z()}.normalized();
        interaction.type = interaction.Type::GEOMETRY;
        interaction.model = material->evaluate(interaction);
    }
    return true;
}
