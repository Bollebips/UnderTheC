#include "Camera.h"
#include <Logger.h>

void CameraProcessInput(Camera* camera, float deltaTime, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool moveUp, bool moveDown, bool lookDown, bool lookUp, bool lookLeft, bool lookRight)
{
    vec3 worldUp;
    mat4 inverseTransform;
    glm_mat4_inv(camera->Transform.raw, inverseTransform);
    glm_vec3_rotate_m4(inverseTransform, GLM_YUP, worldUp);
    glm_vec3_normalize(worldUp);

    if(moveForward)
    {
        vec3 translation;
        glm_vec3_scale_as(GLM_ZUP, camera->Speed * deltaTime, translation);
        glm_translate(camera->Transform.raw, translation);
    }
    if(moveBackward)
    {
        vec3 translation;
        glm_vec3_scale_as(GLM_ZUP, -1 * camera->Speed * deltaTime, translation);
        glm_translate(camera->Transform.raw, translation);
    }
    if(moveLeft)
    {
        vec3 translation;
        glm_vec3_scale_as(GLM_XUP, -1 * camera->Speed * deltaTime, translation);
        glm_translate(camera->Transform.raw, translation);
    }
    if(moveRight)
    {
        vec3 translation;
        glm_vec3_scale_as(GLM_XUP, camera->Speed * deltaTime, translation);
        glm_translate(camera->Transform.raw, translation);
    }
    if(moveUp)
    {
        vec3 translation;
        glm_vec3_scale_as(worldUp, camera->Speed * deltaTime, translation);
        glm_translate(camera->Transform.raw, translation);
    }
    if(moveDown)
    {
        vec3 translation;
        glm_vec3_scale_as(worldUp, -camera->Speed * deltaTime, translation);
        glm_translate(camera->Transform.raw, translation);
    }

    if(lookDown)
    {
        glm_rotate_x(camera->Transform.raw, -camera->AngularSpeed * deltaTime, camera->Transform.raw);
    }
    if(lookUp)
    {
        glm_rotate_x(camera->Transform.raw, camera->AngularSpeed * deltaTime, camera->Transform.raw);
    }
    if(lookLeft)
    {
        glm_rotate(camera->Transform.raw, -camera->AngularSpeed * deltaTime, worldUp);
    }
    if(lookRight)
    {
        glm_rotate(camera->Transform.raw, camera->AngularSpeed * deltaTime, worldUp);
    }
}
