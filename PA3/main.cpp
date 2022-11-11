#include <iostream>
#include <chrono>

#include "integrator.h"

#ifndef TESTBYGITHUB    // PLEASE DO NOT CHANGE
int scene_id = 2;
#else                   // PLEASE DO NOT CHANGE
int scene_id = 0;       // PLEASE DO NOT CHANGE
#endif                  // PLEASE DO NOT CHANGE

std::string grid_dir = "textures/grid.png";
std::string brick_diff_dir = "textures/diff.jpg";
std::string brick_norm_dir = "textures/norm.jpg";
std::string brick_disp_dir = "textures/disp.jpg";
std::string rock_diff_dir = "textures/rock/rocky_diff.jpg";
std::string rock_norm_dir = "textures/rock/rocky_norm.jpg";
std::string rock_disp_dir = "textures/rock/rocky_disp.jpg";
std::string stone_diff_dir = "textures/stone/stone_diff.jpg";
std::string stone_norm_dir = "textures/stone/stone_norm.jpg";
std::string stone_disp_dir = "textures/stone/stone_disp.jpg";

void setSceneById(std::shared_ptr<Scene> &scene, int id);

int main(int argc, char *argv[]) {
    setbuf(stdout, nullptr);
#ifndef TESTBYGITHUB                    // PLEASE DO NOT CHANGE
    Vec2i img_resolution(2000, 2000);
#else                                   // PLEASE DO NOT CHANGE
    Vec2i img_resolution(32, 32);     // PLEASE DO NOT CHANGE
#endif                                  // PLEASE DO NOT CHANGE
    Vec3f cam_pos(0, -3, 1.3);
    Vec3f cam_look_at(0, 0, 1);

//    Vec3f cam_pos(0, -3, 1);
//    Vec3f cam_look_at(0, 0, 0);


    // init image.
    std::shared_ptr<ImageRGB> rendered_img
            = std::make_shared<ImageRGB>(img_resolution.x(), img_resolution.y());
    // camera setting
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->setPosition(cam_pos);
    camera->lookAt(cam_look_at);
    camera->setFov(45);
    camera->setImage(rendered_img);
    // construct scene.
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    setSceneById(scene, scene_id);

    std::unique_ptr<PhongLightingIntegrator> integrator
            = std::make_unique<PhongLightingIntegrator>(camera, scene);
    std::cout << "Start Rendering..." << std::endl;
    auto start = std::chrono::steady_clock::now();
    // render scene
    integrator->render();
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\nRender Finished in " << time << "ms." << std::endl;
#ifndef TESTBYGITHUB                                    // PLEASE DO NOT CHANGE
    rendered_img->writeImgToFile("./result.png");
#else                                                   // PLEASE DO NOT CHANGE
    rendered_img->writeImgToFile("../submit.png");      // PLEASE DO NOT CHANGE
#endif                                                  // PLEASE DO NOT CHANGE
    std::cout << "Image saved to disk." << std::endl;
    return 0;
}

void setSceneById(std::shared_ptr<Scene> &scene, int id) {
    Vec3f light_pos(0, 0, 1.95f);
    Vec3f light_color(1, 1, 1);
    Vec2f light_dim(0.5, 0.3);
    Vec3f light_n(0, 0, -1);
    Vec3f light_t(1, 0, 0);
    std::shared_ptr<Light>
            square_light = std::make_shared<SquareAreaLight>(light_pos, light_color, light_dim, light_n, light_t);

    std::shared_ptr<Light>
            test_light = std::make_shared<SquareAreaLight>(light_pos+Vec3f{0,0,1000},light_color, light_dim, light_n, light_t);

    std::shared_ptr<Material>
            mat_white =
            std::make_shared<ConstColorMat>(Vec3f(0.7f, 0.7f, 0.7f));
    std::shared_ptr<Material>
            mat_red =
            std::make_shared<ConstColorMat>(Vec3f(0.8f, 0.f, 0.f));
    std::shared_ptr<Material>
            mat_green =
            std::make_shared<ConstColorMat>(Vec3f(0.f, 0.8f, 0.f));
    std::shared_ptr<Material>
            mat_yellow =
            std::make_shared<ConstColorMat>(Vec3f(1, 0.93, 0.6));
    std::shared_ptr<Material>
            mat_blue =
            std::make_shared<ConstColorMat>(Vec3f(0.2, 0.5, 0.9));
#ifndef TESTBYGITHUB
    std::shared_ptr<Material>
            mat_grid =
            std::make_shared<TextureMat>(grid_dir);
    std::shared_ptr<Material>
            mat_brick =
            std::make_shared<TextureMat>(brick_diff_dir, brick_norm_dir, brick_disp_dir);
    std::shared_ptr<Material>
            mat_rock =
            std::make_shared<TextureMat>(rock_diff_dir, rock_norm_dir, rock_disp_dir);
    std::shared_ptr<Material>
            mat_stone =
            std::make_shared<TextureMat>(stone_diff_dir, stone_norm_dir, stone_disp_dir);
#endif
    std::shared_ptr<Geometry>
            floor = std::make_shared<Rectangle>(Vec3f(0, 0, 0), Vec2f(2, 2), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
    std::shared_ptr<Geometry>
            ceiling = std::make_shared<Rectangle>(Vec3f(0, 0, 2), Vec2f(2, 2), Vec3f(0, 0, -1), Vec3f(1, 0, 0));
    std::shared_ptr<Geometry>
            wall_left = std::make_shared<Rectangle>(Vec3f(-1, 0, 1), Vec2f(2, 2), Vec3f(1, 0, 0), Vec3f(0, 0, 1));
    std::shared_ptr<Geometry>
            wall_right = std::make_shared<Rectangle>(Vec3f(1, 0, 1), Vec2f(2, 2), Vec3f(-1, 0, 0), Vec3f(0, 0, -1));
    std::shared_ptr<Geometry>
            wall_back = std::make_shared<Rectangle>(Vec3f(0, 1, 1), Vec2f(2, 2), Vec3f(0, -1, 0), Vec3f(1, 0, 0));

    switch (id) {
        case 0 : {
            floor->setMaterial(mat_white);
            ceiling->setMaterial(mat_white);
            wall_back->setMaterial(mat_white);
            wall_left->setMaterial(mat_red);
            wall_right->setMaterial(mat_green);
            scene->addGeometry(floor);
            scene->addGeometry(ceiling);
            scene->addGeometry(wall_left);
            scene->addGeometry(wall_right);
            scene->addGeometry(wall_back);
            addTetrahedronToScene(scene,
                                  mat_blue,
                                  Vec3f(-1, 0.2, 0.2),
                                  Vec3f(-0.5, -0.65, 0.2),
                                  Vec3f(0, 0.2, 0.2),
                                  Vec3f(-0.5, -0.3774, 1));
            addEllipsoidToScene(scene,
                                mat_yellow,
                                Vec3f(0.5, 0.4, 0.4),
                                Vec3f(0.3, 0.1, 0),
                                Vec3f(-0.1, 0.3, 0),
                                Vec3f(0, 0, 0.4));
            scene->setLight(square_light);
            scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
            break;
        }
        case 1: {
#ifndef TESTBYGITHUB
            for (int i = -15; i < 15; i += 4) {
                for (int j = -3; j < 40; j += 4) {
                    std::shared_ptr<Geometry> ground =
                            std::make_shared<Rectangle>(Vec3f((float) i, (float) j, 0), Vec2f(4, 4), Vec3f(0, 0, 1),
                                                        Vec3f(1, 0, 0));
                    ground->setMaterial(mat_grid);
                    scene->addGeometry(ground);
                }
            }
            scene->setLight(square_light);
            scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
#endif
            break;
        }
        case 2: {
#ifndef TESTBYGITHUB
            floor->setMaterial(mat_brick);
            ceiling->setMaterial(mat_white);
            wall_back->setMaterial(mat_rock);
            wall_left->setMaterial(mat_stone);
            wall_right->setMaterial(mat_rock);
            scene->addGeometry(floor);
            scene->addGeometry(ceiling);
            scene->addGeometry(wall_left);
            scene->addGeometry(wall_right);
            scene->addGeometry(wall_back);
            addTetrahedronToScene(scene,
                                  mat_blue,
                                  Vec3f(-1, 0.2, 0.2),
                                  Vec3f(-0.5, -0.65, 0.2),
                                  Vec3f(0, 0.2, 0.2),
                                  Vec3f(-0.5, -0.3774, 1));
            addEllipsoidToScene(scene,
                                mat_yellow,
                                Vec3f(0.5, 0.4, 0.4),
                                Vec3f(0.3, 0.1, 0),
                                Vec3f(-0.1, 0.3, 0),
                                Vec3f(0, 0, 0.4));
            scene->setLight(square_light);
            scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
#endif
            break;
        }
        case 3: {
#ifndef TESTBYGITHUB
            std::shared_ptr<Geometry> norm_wall =
                    std::make_shared<Rectangle>(Vec3f(0, 0, 1), Vec2f(2, 2),
                                                Vec3f(0, -1, 1).normalized(),
                                                Vec3f(0, 1, 1).normalized());
            norm_wall->setMaterial(mat_stone);
            scene->addGeometry(norm_wall);
            scene->setLight(test_light);
            scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
#endif
            break;
        }
        default:
            break;
    }

}
