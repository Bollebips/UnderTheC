#include "Camera.h"

#include <Logger.h>
#include <GLFW/glfw3.h>

void CameraProcessInput(Camera* camera, float deltaTime)
{
    vec3 worldUp;
    mat4 inverseTransform;
    glm_mat4_inv(camera->Transform.raw, inverseTransform);
    glm_vec3_rotate_m4(inverseTransform, GLM_YUP, worldUp);
    glm_vec3_normalize(worldUp);

    vec3 translation;
    glm_vec3_scale_as(camera->AxisInput.raw, camera->LinearSpeed * deltaTime, translation);
    glm_translate(camera->Transform.raw, translation);

    glm_rotate_x(camera->Transform.raw, camera->RotationInput.x * camera->AngularSpeed * deltaTime, camera->Transform.raw);
    glm_rotate(camera->Transform.raw, camera->RotationInput.y * camera->AngularSpeed * deltaTime, worldUp);
}

void CameraRecieveInput(Camera* camera, int key, int action)
{
    int value = action == GLFW_PRESS ? 1 : -1;

    switch(key)
    {
        case GLFW_KEY_W:
            camera->AxisInput.z += value;
            break;
        case GLFW_KEY_S:
            camera->AxisInput.z -= value;
            break;
        case GLFW_KEY_D:
            camera->AxisInput.x += value;
            break;
        case GLFW_KEY_A:
            camera->AxisInput.x -= value;
            break;
        case GLFW_KEY_E:
            camera->AxisInput.y += value;
            break;
        case GLFW_KEY_Q:
            camera->AxisInput.y -= value;
            break;
        case GLFW_KEY_UP:
            camera->RotationInput.x += value;
            break;
        case GLFW_KEY_DOWN:
            camera->RotationInput.x -= value;
            break;
        case GLFW_KEY_RIGHT:
            camera->RotationInput.y += value;
            break;
        case GLFW_KEY_LEFT:
            camera->RotationInput.y -= value;
            break;
    }
}
