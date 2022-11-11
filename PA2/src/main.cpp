#include <utils.h>
#include <camera.h>
#include "enum.h"
#include <object.h>
#include <shader.h>
#include <bezier.h>


const int WIDTH = 1920;
const int HEIGHT = 1080;

Camera cam(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 1.0f, 0.0f), (float) WIDTH / 2, (float) HEIGHT / 2);


void mouse_callback(GLFWwindow *window, double xPos, double yPos) {
    cam.mouse_callback(window, xPos, yPos);
}

void scroll_callback(GLFWwindow *window, double xPos, double yPos) {
    cam.scroll_callback(window, xPos, yPos);
}

GLFWwindow *window;


BETTER_ENUM(RenderCase, int,
            uniform_single_bezier_surface,
            uniform_multi_bezier_surface,
            single_spline_surface,
            multi_spline_surface,
            adaptive_single_bezier_surface
)

/**
 * BETTER_ENUM is from https://aantron.github.io/better-enums/
 * You can add any render case as you like,
 * this is to allow you to demonstrate multi task in single executable file.
 * */


RenderCase choice = RenderCase::_values()[0];

std::vector<std::vector<vec3>> surface_control_points{
        std::vector<vec3>{
                vec3(-3.0f, -3.0f, 6.0f),
                vec3(-3.0f, -1.0f, 6.0f),
                vec3(-3.0f, 1.0f, 6.0f),
                vec3(-3.0f, 3.0f, 6.0f),
        },
        std::vector<vec3>{
                vec3(-1.0f, -3.0f, 0.0f),
                vec3(-1.0f, -1.0f, 0.0f),
                vec3(-1.0f, 1.0f, 0.0f),
                vec3(-1.0f, 3.0f, 0.0f),
        },
        std::vector<vec3>{
                vec3(1.0f, -3.0f, 0.0f),
                vec3(1.0f, -1.0f, 0.0f),
                vec3(1.0f, 1.0f, 0.0f),
                vec3(1.0f, 3.0f, 0.0f),
        },
        std::vector<vec3>{
                vec3(3.0f, -3.0f, 0.0f),
                vec3(3.0f, -1.0f, 0.0f),
                vec3(3.0f, 1.0f, 0.0f),
                vec3(3.0f, 3.0f, 0.0f),
        }
};

std::vector<vec3> curve_control_points{
        vec3(0.0f, -9.0f, 0.0f),
        vec3(0.0f, 0.0f, 2.0f),
        vec3(0.0f, 0.0f, 4.0f),
        vec3(0.0f, 0.0f, 6.0f),
};


int main() {
    for (size_t index = 0; index < RenderCase::_size(); ++index) {
        RenderCase render_case = RenderCase::_values()[index];
        std::cout << index << ". " << +render_case << "\n";
    }
    while (true) {
        std::cout << "choose a rendering case from [0:" << RenderCase::_size() - 1 << "]" << "\n";
        std::string input;
        std::cin >> input;
        if (isNumber(input) &&
            std::stoi(input) >= 0 &&
            std::stoi(input) < RenderCase::_size()) {
            choice = RenderCase::_values()[std::stoi(input)];
            break;
        } else {
            std::cout << "Wrong input.\n";
        }
    }
    /**
     * Choose a rendering case in the terminal.
     * */

    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw2");
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    std::vector<BezierSurface> surfaces;
    std::vector<BezierCurve> curves;
    std::vector<Object> objs;
    switch (choice) {
        case RenderCase::uniform_single_bezier_surface: {
            std::cout << +RenderCase::uniform_single_bezier_surface << "do something\n";
            surfaces.emplace_back(surface_control_points);
            objs.push_back(surfaces[0].generateObject(100, 100));
            break;
        }
        case RenderCase::uniform_multi_bezier_surface: {
            std::cout << +RenderCase::uniform_multi_bezier_surface << "do something\n";
            surfaces = read("assets/tea.bzs");
            for (auto surface: surfaces) {
                objs.push_back(surface.generateObject(100, 100));
            }
            break;
        }
        case RenderCase::single_spline_surface: {
            std::cout << +RenderCase::single_spline_surface << "do something\n";
            surfaces.emplace_back(surface_control_points);
            objs.push_back(surfaces[0].generateBSplineObject(100, 100, 3));
            break;
        }
        case RenderCase::multi_spline_surface:
            std::cout << +RenderCase::uniform_multi_bezier_surface << "do something\n";
            surfaces = read("assets/tea.bzs");
            for (auto surface: surfaces) {
                objs.push_back(surface.generateBSplineObject(100, 100, 1));
            }
            break;
        case RenderCase::adaptive_single_bezier_surface:
            std::cout << +RenderCase::adaptive_single_bezier_surface << "do something\n";
//            curves.emplace_back(curve_control_points);
//            objs.push_back(curves[0].generateAdaptiveObject());
            surfaces.emplace_back(surface_control_points);
            objs.push_back(surfaces[0].generateAdaptiveObject(100, 100));
            break;
    }

//    Shader shader("src/points_vec.glsl", "src/points_frag.glsl");
    Shader shader("src/vs.glsl", "src/fs.glsl");
    double lastFrame = 0;
    while (!glfwWindowShouldClose(window)) {
        cam.processInput(window);

        double currentFrame = glfwGetTime();
        double delta = currentFrame - lastFrame;
        lastFrame = currentFrame;
        printf("FPS=%f\n", 1 / delta);

//        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
//        glClearColor(0, 0, 0, 1);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSetCursorPosCallback(window, mouse_callback);

        glfwSetScrollCallback(window, scroll_callback);

        shader.use();

        shader.setVec3("viewPos", cam.Pos);
        shader.setVec3("objectColor", vec3(0.1f, 0.4f, 0.2f));
        shader.setVec3("dirLight.dir", vec3(-1.0f, -1.0f, -1.0f));
        shader.setVec3("dirLight.color", vec3(1.0f, 1.0f, 1.0f));


        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        for (const auto &obj: objs) {
            obj.drawElements();
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}
