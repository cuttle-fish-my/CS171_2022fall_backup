#include "scene.h"

#include <utility>
#include <iostream>

void Scene::addGeometry(std::shared_ptr<Geometry> &geometry) {
    geometries.push_back(geometry);
}

void Scene::setLight(const std::shared_ptr<Light> &new_light) {
    light = new_light;
}

bool Scene::isShadowed(Ray &shadow_ray) {
    Interaction interaction{};
    return intersect(shadow_ray, interaction) && interaction.type == interaction.Type::GEOMETRY;
}

bool Scene::intersect(Ray &ray, Interaction &interaction) {
    bool isIntersect = false;
    for (const auto &geometry: geometries) {
        if (geometry->intersect(ray, interaction)) {
            isIntersect = true;
        }
    }
    if (light->intersect(ray, interaction)) {
        isIntersect = true;
    }
    return isIntersect;
}

const std::shared_ptr<Light> &Scene::getLight() const {
    return light;
}

void Scene::setAmbient(Vec3f ambient) {
    ambient_light = std::move(ambient);
}

const Vec3f &Scene::getAmbient() const {
    return ambient_light;
}

void addTetrahedronToScene(std::shared_ptr<Scene> &scene, std::shared_ptr<Material> &mat,
                           const Vec3f &v0,
                           const Vec3f &v1,
                           const Vec3f &v2,
                           const Vec3f &v3) {
    std::shared_ptr<Geometry>
            f0 = std::make_shared<Triangle>(v0, v2, v1);
    std::shared_ptr<Geometry>
            f1 = std::make_shared<Triangle>(v0, v1, v3);
    std::shared_ptr<Geometry>
            f2 = std::make_shared<Triangle>(v1, v2, v3);
    std::shared_ptr<Geometry>
            f3 = std::make_shared<Triangle>(v0, v3, v2);
    f0->setMaterial(mat);
    f1->setMaterial(mat);
    f2->setMaterial(mat);
    f3->setMaterial(mat);
    scene->addGeometry(f0);
    scene->addGeometry(f1);
    scene->addGeometry(f2);
    scene->addGeometry(f3);
}

void addEllipsoidToScene(std::shared_ptr<Scene> &scene, std::shared_ptr<Material> &mat,
                         const Vec3f &p,
                         const Vec3f &a,
                         const Vec3f &b,
                         const Vec3f &c) {
    std::shared_ptr<Geometry> ellipsoid = std::make_shared<Ellipsoid>(p, a, b, c);
    ellipsoid->setMaterial(mat);
    scene->addGeometry(ellipsoid);
}