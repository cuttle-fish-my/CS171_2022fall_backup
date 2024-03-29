#ifndef ACCEL_H_
#define ACCEL_H_

#include <utility>

#include "core.h"
#include "ray.h"

struct AABB {
    // the minimum and maximum coordinate for the AABB
    Vec3f low_bnd;
    Vec3f upper_bnd;

    /// test intersection with given ray.
    /// ray distance of entrance and exit point are recorded in t_in and t_out
    AABB() : low_bnd(0, 0, 0), upper_bnd(0, 0, 0) {}

    AABB(Vec3f low, Vec3f upper) : low_bnd(std::move(low)), upper_bnd(std::move(upper)) {}

    /// construct an AABB from three vertices of a triangle.
    AABB(const Vec3f &v1, const Vec3f &v2, const Vec3f &v3);

    /// Construct AABB by merging two AABBs
    AABB(const AABB &a, const AABB &b);

    bool intersect(const Ray &ray, float *t_in, float *t_out);

    /// Get the AABB center
    [[nodiscard]] Vec3f getCenter() const { return (low_bnd + upper_bnd) / 2; }

    /// Get the length of a specified side on the AABB
    [[nodiscard]] float getDist(int dim) const { return upper_bnd[dim] - low_bnd[dim]; }

    /// Check whether the AABB is overlapping with another AABB
    [[nodiscard]] bool isOverlap(const AABB &other) const;
};

struct BVHNode {
    BVHNode *left;
    BVHNode *right;
    // bounding box of current node.
    int right_idx{-1};
    AABB aabb;
    // index of triangles in current BVH leaf node.
//    std::vector<int> triangles;
    int triangles_begin_idx{};
    int triangles_end_idx{};
};

struct LBVHNode {
    AABB aabb;
    int triangle_begin_idx{-1};
    union {
        int right_idx{-1};
        int triangle_end_idx;
    };

    LBVHNode(AABB aabb) : aabb(std::move(aabb)){};
};
// You may need to add your code for BVH construction here.

#endif //ACCEL_H_