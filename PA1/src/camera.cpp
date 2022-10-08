#include "camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 up, float lastX, float lastY, float yaw, float pitch)
        : Pos(pos), WorldUp(up), Yaw(yaw), Pitch(pitch),
          camSpeed(SPEED), camSensitivity(SENSITIVITY),
          Zoom(ZOOM), Front(0.0f, 0.0f, -1.0f),
          firstMouse(true), lastX(lastX), lastY(lastY) {
    updCamVec();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Pos, Pos + Front, Up);
}

void Camera::processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Pos += camSpeed * Front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Pos -= camSpeed * Front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Pos -= camSpeed * Right;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Pos += camSpeed * Right;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        Pos -= camSpeed * Up;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Pos += camSpeed * Up;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Camera::scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
    Zoom -= (float) yOffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::mouse_callback(GLFWwindow *window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = (float) xPos;
        lastY = (float) yPos;
        firstMouse = false;
    }
    float xOffset = (float) xPos - lastX;
    float yOffset = lastY - (float) yPos;

    lastX = (float) xPos;
    lastY = (float) yPos;

    xOffset *= camSensitivity;
    yOffset *= camSensitivity;

    Yaw += xOffset;
    Pitch += yOffset;

    if (Pitch > 89.0f) {
        Pitch = 89.0f;
    }
    if (Pitch < -89.0f) {
        Pitch = -89.0f;
    }

    updCamVec();
}

void Camera::updCamVec() {
    glm::vec3 front;
    front.x = float(cos(glm::radians(Pitch)) * cos(glm::radians(Yaw)));
    front.z = float(cos(glm::radians(Pitch)) * sin(glm::radians(Yaw)));
    front.y = float(sin(glm::radians(Pitch)));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

