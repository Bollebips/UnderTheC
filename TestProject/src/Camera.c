#include "Camera.h"

void CameraProcessInput(Camera* camera, float deltaTime, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool moveUp, bool moveDown)
{
    if(moveForward)
    {
        camera->Position.z += camera->Speed * deltaTime;
    }
    if(moveBackward)
    {
        camera->Position.z -= camera->Speed * deltaTime;
    }
    if(moveLeft)
    {
        camera->Position.x -= camera->Speed * deltaTime;
    }
    if(moveRight)
    {
        camera->Position.x += camera->Speed * deltaTime;
    }
    if(moveUp)
    {
        camera->Position.y += camera->Speed * deltaTime;
    }
    if(moveDown)
    {
        camera->Position.y -= camera->Speed * deltaTime;
    }
}
