#ifndef SCENE_H_
#define SCENE_H_

#include <vector>

#include "camera.h"
#include "image.h"
#include "geometry.h"
#include "light.h"
#include "interaction.h"
#include "config.h"

class Scene {
public:
    Scene() = default;

    void addObject(std::shared_ptr<TriangleMesh> &geometry);

    [[nodiscard]] const std::shared_ptr<Light> &getLight() const;

    void setLight(const std::shared_ptr<Light> &new_light);

    bool isShadowed(Ray &shadow_ray);

    bool intersect(Ray &ray, Interaction &interaction);

    BVHNode *getBVHNode();

    void setBVHRoot(BVHNode *root);

    BVHNode *buildBVH(int start, int end);

    int findSplit(int start, int end);

    void bvhIntersect(BVHNode *root, Interaction &interaction, Ray &ray);

    void setTriangles(std::vector<Triangle> new_Triangles);

    void DFS(BVHNode *root);

    [[nodiscard]] std::vector<LBVHNode>getLBVH() const;

    void lbvhIntersect(int idx, Interaction &interaction, Ray &ray);

//    void buildLBVH()

private:
    std::vector<std::shared_ptr<TriangleMesh>> objects;
    std::shared_ptr<Light> light;
    BVHNode *bvhNode{nullptr};
    std::vector<Triangle> Triangles;
    std::vector<LBVHNode> LBVH{};
};

void initSceneFromConfig(const Config &config, std::shared_ptr<Scene> &scene);

#endif //SCENE_H_
