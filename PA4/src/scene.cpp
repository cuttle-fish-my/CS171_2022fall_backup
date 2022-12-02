#include "scene.h"
#include "load_obj.h"
#include "utils.h"

#include <utility>
#include <iostream>

void Scene::addObject(std::shared_ptr<TriangleMesh> &mesh) {
    objects.push_back(mesh);
}

void Scene::setLight(const std::shared_ptr<Light> &new_light) {
    light = new_light;
}

bool Scene::isShadowed(Ray &shadow_ray) {
    Interaction in;
    return intersect(shadow_ray, in) && in.type == Interaction::Type::GEOMETRY;
}

bool Scene::intersect(Ray &ray, Interaction &interaction) {
    light->intersect(ray, interaction);
    if (!LBVH.empty()) {
        auto *t_in = new float;
        auto *t_out = new float;
        bool hit = bvhNode->aabb.intersect(ray, t_in, t_out);
        delete t_in;
        delete t_out;
        if (!hit) {
            return false;
        }
        lbvhIntersect(0, interaction, ray);
    } else {
        for (const auto &obj: objects) {
            Interaction cur_it;
            if (obj->intersect(ray, cur_it) && (cur_it.dist < interaction.dist)) {
                interaction = cur_it;
            }
        }
    }
    return interaction.type != Interaction::Type::NONE;
}

const std::shared_ptr<Light> &Scene::getLight() const {
    return light;
}

BVHNode *Scene::getBVHNode() {
    return bvhNode;
}

BVHNode *Scene::buildBVH(int start, int end) {
    if (end - start <= 8) {
        AABB new_aabb = Triangles[start].getAABB();
        for (int i = start + 1; i <= end; ++i) {
            new_aabb = AABB(new_aabb, Triangles[i].getAABB());
        }
        return new BVHNode{.left=nullptr, .right=nullptr,
                .aabb=new_aabb,
                .triangles_begin_idx=start,
                .triangles_end_idx=end};
    }
    int split = findSplit(start, end);
    BVHNode *left = buildBVH(start, split);
    BVHNode *right = buildBVH(split + 1, end);
    return new BVHNode{.left=left, .right=right, .aabb=AABB(left->aabb, right->aabb)};
}

int Scene::findSplit(int start, int end) {
    unsigned int startCode = Triangles[start].getMortonCode();
    unsigned int endCode = Triangles[end].getMortonCode();

    if (startCode == endCode) {
        return (start + end) / 2;
    }

    int commonPrefix = __builtin_clz(startCode ^ endCode);

    int split = start;
    int step = end - start;

    do {
        step = (step + 1) >> 1;
        int newSplit = split + step;
        if (newSplit < end) {
            unsigned int splitCode = Triangles[newSplit].getMortonCode();
            int splitPrefix = __builtin_clz(startCode ^ splitCode);
            if (splitPrefix > commonPrefix) {
                split = newSplit;
            }
        }
    } while (step > 1);
    return split;
}

void Scene::setBVHRoot(BVHNode *root) {
    bvhNode = root;
}

void Scene::bvhIntersect(BVHNode *root, Interaction &interaction, Ray &ray) {
    if (!root->left && !root->right) {
        for (int i = root->triangles_begin_idx; i <= root->triangles_end_idx; ++i) {
            Interaction curr_it;
            if (Triangles[i].intersect(ray, curr_it) && curr_it.dist < interaction.dist) {
                interaction = curr_it;
            }
        }
        return;
    } else if (!root->left || !root->right) {
        BVHNode *child = root->left ? root->left : root->right;
        Interaction curr_it;
        bvhIntersect(child, curr_it, ray);
        if (curr_it.dist < interaction.dist) {
            interaction = curr_it;
        }
        return;
    }
    auto *left_min = new float, *left_max = new float, *right_min = new float, *right_max = new float;
    bool left_hit = root->left->aabb.intersect(ray, left_min, left_max);
    bool right_hit = root->right->aabb.intersect(ray, right_min, right_max);
    float l_min = *left_min, r_min = *right_min;
    delete left_min;
    delete left_max;
    delete right_min;
    delete right_max;
    if (!left_hit && !right_hit) {
        return;
    } else if (left_hit && right_hit) {
        BVHNode *child = l_min < r_min ? root->left : root->right;
        float t_min = std::max(l_min, r_min);
        Interaction curr_it;
        bvhIntersect(child, curr_it, ray);
        if (curr_it.dist < interaction.dist) {
            interaction = curr_it;
        }
        if (curr_it.dist > t_min) {
            child = (child == root->right) ? root->left : root->right;
            bvhIntersect(child, curr_it, ray);
            if (curr_it.dist < interaction.dist) {
                interaction = curr_it;
            }
        }
    } else {
        BVHNode *child = left_hit ? root->left : root->right;
        Interaction curr_it;
        bvhIntersect(child, curr_it, ray);
        if (curr_it.dist < interaction.dist) {
            interaction = curr_it;
        }
    }
}

void Scene::lbvhIntersect(int idx, Interaction &interaction, Ray &ray) {
    int begin_idx = LBVH.at(idx).triangle_begin_idx;
    int end_idx = LBVH.at(idx).triangle_end_idx;
    int right_idx = LBVH.at(idx).right_idx;
    int left_idx = idx + 1;
//    leaf node
    if (begin_idx != -1) {
        for (int i = begin_idx; i <= end_idx; ++i) {
            Interaction curr_it;
            if (Triangles[i].intersect(ray, curr_it) && curr_it.dist < interaction.dist) {
                interaction = curr_it;
            }
        }
        return;
    }
//    node with only left child
    else if (right_idx == -1) {
        Interaction curr_it;
        lbvhIntersect(left_idx, curr_it, ray);
        if (curr_it.dist < interaction.dist) {
            interaction = curr_it;
        }
        return;
    }
//    node with only right child
    else if (right_idx - idx == 1) {
        Interaction curr_it;
        lbvhIntersect(right_idx, curr_it, ray);
        if (curr_it.dist < interaction.dist) {
            interaction = curr_it;
        }
        return;
    }

    auto *left_min = new float, *left_max = new float, *right_min = new float, *right_max = new float;
    bool left_hit = LBVH[left_idx].aabb.intersect(ray, left_min, left_max);
    bool right_hit = LBVH[right_idx].aabb.intersect(ray, right_min, right_max);
    float l_min = *left_min, r_min = *right_min;
    delete left_min;
    delete left_max;
    delete right_min;
    delete right_max;
    if (!left_hit && !right_hit) {
        return;
    } else if (left_hit && right_hit) {
        int child_idx = l_min <= r_min ? left_idx : right_idx;
        float t_min = std::max(l_min, r_min);
        Interaction curr_it;
        lbvhIntersect(child_idx, curr_it, ray);
        if (curr_it.dist < interaction.dist) {
            interaction = curr_it;
        }
        if (curr_it.dist > t_min) {
            child_idx = (l_min > r_min) ? left_idx : right_idx;
            lbvhIntersect(child_idx, curr_it, ray);
            if (curr_it.dist < interaction.dist) {
                interaction = curr_it;
            }
        }

    } else {
        int child_idx = left_hit ? left_idx : right_idx;
        Interaction curr_it;
        lbvhIntersect(child_idx, curr_it, ray);
        if (curr_it.dist < interaction.dist) {
            interaction = curr_it;
        }
    }
}

void Scene::setTriangles(std::vector<Triangle> new_Triangles) {
    Triangles = std::move(new_Triangles);
}

void Scene::DFS(BVHNode *root) {
    LBVH.emplace_back(root->aabb);
    int root_idx = (int) LBVH.size() - 1;

    if (!root->left && !root->right) {
        LBVH.at(root_idx).triangle_begin_idx = root->triangles_begin_idx;
        LBVH.at(root_idx).triangle_end_idx = root->triangles_end_idx;
        return;
    }
    if (root->left) DFS(root->left);
    if (root->right) {
        LBVH.at(root_idx).right_idx = (int) LBVH.size();
        DFS(root->right);
    }

}

std::vector<LBVHNode> Scene::getLBVH() const {
    return LBVH;
}


void initSceneFromConfig(const Config &config, std::shared_ptr<Scene> &scene) {
    // add square light to scene.
    std::shared_ptr<Light> light = std::make_shared<SquareAreaLight>(Vec3f(config.light_config.position),
                                                                     Vec3f(config.light_config.radiance),
                                                                     Vec2f(config.light_config.size));
    scene->setLight(light);
    // init all materials.
    std::map<std::string, std::shared_ptr<BSDF>> mat_list;
    for (const auto &mat: config.materials) {
        std::shared_ptr<BSDF> p_mat;
        switch (mat.type) {
            case MaterialType::DIFFUSE: {
                p_mat = std::make_shared<IdealDiffusion>(Vec3f(mat.color));
                mat_list[mat.name] = p_mat;
                break;
            }
            case MaterialType::SPECULAR: {
                p_mat = std::make_shared<IdealSpecular>();
                mat_list[mat.name] = p_mat;
                break;
            }
            default: {
                std::cerr << "unsupported material type!" << std::endl;
                exit(-1);
            }
        }
    }
    // add mesh objects to scene. Translation and scaling are directly applied to vertex coordinates.
    // then set corresponding material by name.
    std::cout << "loading obj files..." << std::endl;
    std::vector<Triangle> Triangles;
    Vec3f lower_bnd{1e10, 1e10, 1e10}, upper_bnd{-1e10, -1e10, -1e10};
    for (auto &object: config.objects) {
        auto mesh_obj = makeMeshObject(object.obj_file_path, Vec3f(object.translate), object.scale);
        std::vector<Vec3f> v = mesh_obj->getVertices(), n = mesh_obj->getNormals();
        std::vector<int> v_idx = mesh_obj->getVIndex(), n_idx = mesh_obj->getNIndex();
        for (int i = 0; i < v_idx.size(); i += 3) {
            std::vector<Vec3f> Vertices{v.at(v_idx.at(i + 0)),
                                        v.at(v_idx.at(i + 1)),
                                        v.at(v_idx.at(i + 2))};
            std::vector<Vec3f> Normals{n.at(n_idx.at(i + 0)),
                                       n.at(n_idx.at(i + 1)),
                                       n.at(n_idx.at(i + 2))};
            AABB aabb(v.at(v_idx.at(i + 0)), v.at(v_idx.at(i + 1)), v.at(v_idx.at(i + 2)));
            Triangle t(Vertices, Normals, aabb);
            t.setMaterial(mat_list[object.material_name]);
            Triangles.push_back(t);
            lower_bnd = lower_bnd.cwiseMin(v.at(v_idx.at(i + 0)));
            upper_bnd = upper_bnd.cwiseMax(v.at(v_idx.at(i + 0)));
            lower_bnd = lower_bnd.cwiseMin(v.at(v_idx.at(i + 1)));
            upper_bnd = upper_bnd.cwiseMax(v.at(v_idx.at(i + 1)));
            lower_bnd = lower_bnd.cwiseMin(v.at(v_idx.at(i + 2)));
            upper_bnd = upper_bnd.cwiseMax(v.at(v_idx.at(i + 2)));
        }
    }
    for (auto &triangle: Triangles) {
        Vec3f v0 = triangle.getVertices().at(0);
        Vec3f v1 = triangle.getVertices().at(1);
        Vec3f v2 = triangle.getVertices().at(2);
        Vec3f v = (v0 + v1 + v2) / 3;
        v = (v - lower_bnd).array() / (upper_bnd - lower_bnd).array();
        triangle.setMortonCode(utils::morton3D(v));
    }
    std::sort(Triangles.begin(), Triangles.end(),
              [=](const Triangle &a, const Triangle &b) { return a.getMortonCode() < b.getMortonCode(); });
    scene->setTriangles(Triangles);
    std::cout << "Building BVH" << std::endl;
    scene->setBVHRoot(scene->buildBVH(0, (int) Triangles.size() - 1));
    std::cout << "Finished building BVH" << std::endl;
    scene->DFS(scene->getBVHNode());
}