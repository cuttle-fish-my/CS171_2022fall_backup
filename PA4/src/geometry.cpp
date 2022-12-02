#include "geometry.h"

#include <utility>
#include <iostream>


TriangleMesh::TriangleMesh(std::vector<Vec3f> vertices, std::vector<Vec3f> normals,
                           std::vector<int> v_index, std::vector<int> n_index) :
        vertices(std::move(vertices)),
        normals(std::move(normals)),
        v_indices(std::move(v_index)),
        n_indices(std::move(n_index)),
        bvh(nullptr) {}

bool TriangleMesh::intersect(Ray &ray, Interaction &interaction) const {
    if (bvh != nullptr) {
        bvhHit(bvh, interaction, ray);
    } else {
        // If you did not implement BVH
        // directly loop through all triangles in the mesh and test intersection for each triangle.
        for (int i = 0; i < v_indices.size() / 3; i++) {
            Vec3i v_idx(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
            Vec3i n_idx(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
            Interaction temp;
            if (intersectOneTriangle(ray, temp, v_idx, n_idx) && (temp.dist < interaction.dist)) {
                interaction = temp;
            }
        }
    }
    return interaction.type != Interaction::Type::NONE;
}

void TriangleMesh::setMaterial(std::shared_ptr<BSDF> &new_bsdf) {
    bsdf = new_bsdf;
}

void TriangleMesh::buildBVH() {
    // TODO: your implementation
}

bool TriangleMesh::intersectOneTriangle(Ray &ray,
                                        Interaction &interaction,
                                        const Vec3i &v_idx,
                                        const Vec3i &n_idx) const {
    Vec3f v0 = vertices[v_idx[0]];
    Vec3f v1 = vertices[v_idx[1]];
    Vec3f v2 = vertices[v_idx[2]];
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pvec = ray.direction.cross(v0v2);
    float det = v0v1.dot(pvec);

    float invDet = 1.0f / det;

    Vec3f tvec = ray.origin - v0;
    float u = tvec.dot(pvec) * invDet;
    if (u < 0 || u > 1) return false;
    Vec3f qvec = tvec.cross(v0v1);
    float v = ray.direction.dot(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    float t = v0v2.dot(qvec) * invDet;
    if (t < ray.t_min || t > ray.t_max) return false;

    interaction.dist = t;
    interaction.pos = ray(t);
    interaction.normal = (u * normals[n_idx[1]] + v * normals[n_idx[2]]
                          + (1 - u - v) * normals[n_idx[0]]).normalized();
    interaction.material = bsdf;
    interaction.type = Interaction::Type::GEOMETRY;
    return true;
}

void TriangleMesh::bvhHit(BVHNode *p, Interaction &interaction,
                          Ray &ray) const {
    // TODO: traverse through the bvh and do intersection test efficiently.
}

std::vector<Vec3f> TriangleMesh::getVertices() {
    return vertices;
}

std::vector<Vec3f> TriangleMesh::getNormals() {
    return normals;
}

std::vector<int> TriangleMesh::getVIndex() {
    return v_indices;
}

std::vector<int> TriangleMesh::getNIndex() {
    return n_indices;
}

Triangle::Triangle(std::vector<Vec3f> vertices, std::vector<Vec3f> normals, AABB aabb) :
        Box(std::move(aabb)),
        vertices(std::move(vertices)),
        normals(std::move(normals)) {}

unsigned int Triangle::getMortonCode() const {
    return MortonCode;
}

AABB Triangle::getAABB() const {
    return Box;
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
    Vec3f v0 = vertices[0];
    Vec3f v1 = vertices[1];
    Vec3f v2 = vertices[2];
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pvec = ray.direction.cross(v0v2);
    float det = v0v1.dot(pvec);

    float invDet = 1.0f / det;

    Vec3f tvec = ray.origin - v0;
    float u = tvec.dot(pvec) * invDet;
    if (u < 0 || u > 1) return false;
    Vec3f qvec = tvec.cross(v0v1);
    float v = ray.direction.dot(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    float t = v0v2.dot(qvec) * invDet;
    if (t < ray.t_min || t > ray.t_max) return false;

    interaction.dist = t;
    interaction.pos = ray(t);
    interaction.normal = (u * normals[1] + v * normals[2]
                          + (1 - u - v) * normals[0]).normalized();
    interaction.material = bsdf;
    interaction.type = Interaction::Type::GEOMETRY;
    return true;
}

void Triangle::setMaterial(std::shared_ptr<BSDF> &new_bsdf) {
    bsdf = new_bsdf;
}

void Triangle::setMortonCode(unsigned int mortonCode) {
    MortonCode = mortonCode;
}

std::vector<Vec3f> Triangle::getVertices() {
    return vertices;
}

