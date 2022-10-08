#ifndef CS171_HW1_CAMERA_H
#define CS171_HW1_CAMERA_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.02f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;

class Camera {
public:
    glm::vec3 Pos;
    glm::vec3 Front;
    glm::vec3 Up{};
    glm::vec3 Right{};
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float camSpeed;
    float camSensitivity;
    float Zoom;

    bool firstMouse;

    float lastX{}, lastY{};

    explicit Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float lastX = 0.0, float lastY = 0.0, float yaw = YAW, float pitch = PITCH);

    [[nodiscard]] glm::mat4 GetViewMatrix() const;

    void processInput(GLFWwindow *window);

    void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

    void mouse_callback(GLFWwindow *window, double xPos, double yPos);

private:
    void updCamVec();
};


#endif //CS171_HW1_CAMERA_H
