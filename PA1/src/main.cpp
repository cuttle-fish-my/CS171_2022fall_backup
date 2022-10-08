#include <utils.h>
#include <mesh.h>
#include "shader.h"
#include "camera.h"

const int WIDTH = 3840;
const int HEIGHT = 2160;
Camera cam(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), (float) WIDTH / 2, (float) HEIGHT / 2);

void mouse_callback(GLFWwindow *window, double xPos, double yPos) {
    cam.mouse_callback(window, xPos, yPos);
}

void scroll_callback(GLFWwindow *window, double xPos, double yPos) {
    cam.scroll_callback(window, xPos, yPos);
}

GLFWwindow *window;


int main() {
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw1");
    Mesh mesh = Mesh("assets/bunny.obj");
    Mesh lightSource = Mesh("assets/sphere.obj");


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Shader shader("src/furVertexSource.vs", "src/fragmentSource.fs", "src/geometrySource.gs");
    Shader lightSourceShader("src/lightSourceVertexSource.vs", "src/lightSourceFragmentSource.fs");
//    Shader furShader("src/furVertexSource.vs", "src/furFragmentSource.fs", "src/geometrySource.gs");
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        cam.processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
//        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//            glfwSetWindowShouldClose(window, true);

        shader.use();

        shader.setVec3("dirLight.dir", glm::normalize(vec3(0.0f, 1.0f, 0.0f)));
        shader.setVec3("dirLight.color", vec3(0.0f, 0.5f, 0.0f));

        shader.setInt("NUM_OF_POINT_LIGHTS", 2);

        shader.setVec3("pointLights[0].pos", vec3(1.0f, -1.0f, 0.0f));
        shader.setVec3("pointLights[0].color", vec3(1.0f, 0.0f, 0.0f));

        shader.setFloat("pointLights[0].K0", 1.0f);
        shader.setFloat("pointLights[0].K1", 0.09f);
        shader.setFloat("pointLights[0].K2", 0.032f);

        shader.setVec3("pointLights[1].pos", vec3(-1.0f, 1.0f, 0.0f));
        shader.setVec3("pointLights[1].color", vec3(0.0f, 0.0f, 1.0f));

        shader.setFloat("pointLights[1].K0", 1.0f);
        shader.setFloat("pointLights[1].K1", 0.09f);
        shader.setFloat("pointLights[1].K2", 0.032f);

        shader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("spotLight.color", vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("spotLight.pos", cam.Pos);
        shader.setVec3("spotLight.dir", cam.Front);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(7.5f)));
        shader.setFloat("spotLight.softOff", glm::cos(glm::radians(10.5f)));
        shader.setVec3("viewPos", cam.Pos);

        shader.setFloat("spotLight.K0", 1.0f);
        shader.setFloat("spotLight.K1", 0.09f);
        shader.setFloat("spotLight.K2", 0.032f);

        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        shader.setMat4("projection_g", projection);
        shader.setMat4("projection_v", projection);
        shader.setMat4("view_v", view);
        shader.setMat4("view_g", view);
        shader.setMat4("model", model);
        mesh.draw();

        lightSourceShader.use();
        model = glm::translate(glm::mat4(1.0f), vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.02f));
        lightSourceShader.setMat4("model", model);
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setVec3("aColor", vec3(0.0f, 0.5f, 0.0f));
        lightSource.draw();

        model = glm::translate(glm::mat4(1.0f), vec3(1.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.02f));
        lightSourceShader.setMat4("model", model);
        lightSourceShader.setVec3("aColor", vec3(1.0f, 0.0f, 0.0f));
        lightSource.draw();

        model = glm::translate(glm::mat4(1.0f), vec3(-1.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.02f));
        lightSourceShader.setMat4("model", model);
        lightSourceShader.setVec3("aColor", vec3(0.0f, 0.0f, 1.0f));
        lightSource.draw();


        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;
}