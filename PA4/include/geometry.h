#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "core.h"
#include "ray.h"
#include "interaction.h"
#include "bsdf.h"
#include "accel.h"

#include <vector>
#include <optional>

class Triangle {
public:
    Triangle() = default;

    Triangle(std::vector<Vec3f> vertices, std::vector<Vec3f> normals, AABB aabb);
    [[nodiscard]] AABB getAABB() const;
    [[nodiscard]] unsigned int getMortonCode() const;
    bool intersect(Ray &ray, Interaction &interaction) const;
    void setMaterial(std::shared_ptr<BSDF> &new_bsdf);
    void setMortonCode(unsigned int mortonCode);
    std::vector<Vec3f> getVertices();

private:
    AABB Box;
    unsigned int MortonCode{};
    std::vector<Vec3f> vertices;
    std::vector<Vec3f> normals;
    std::shared_ptr<BSDF> bsdf;
};

class TriangleMesh {
public:
    TriangleMesh() = default;

    TriangleMesh(std::vector<Vec3f> vertices,
                 std::vector<Vec3f> normals,
                 std::vector<int> v_index,
                 std::vector<int> n_index);

    bool intersect(Ray &ray, Interaction &interaction) const;

    void setMaterial(std::shared_ptr<BSDF> &new_bsdf);

    void buildBVH();

    std::vector<Vec3f> getVertices();

    std::vector<Vec3f> getNormals();

    std::vector<int> getVIndex();

    std::vector<int> getNIndex();

private:
    bool intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i &v_idx, const Vec3i &n_idx) const;

    void bvhHit(BVHNode *p, Interaction &interaction,
                Ray &ray) const;

    std::shared_ptr<BSDF> bsdf;
    BVHNode *bvh{};

    std::vector<Vec3f> vertices;
    std::vector<Vec3f> normals;
    std::vector<int> v_indices;
    std::vector<int> n_indices;
};

#endif // GEOMETRY_H_
