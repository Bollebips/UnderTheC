#include "Camera.h"

void CameraProcessInput(Camera* camera, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool moveUp, bool moveDown)
{
    if(moveForward)
    {
        camera->Position.z += camera->Speed;
    }
    if(moveBackward)
    {
        camera->Position.z -= camera->Speed;
    }
    if(moveLeft)
    {
        camera->Position.x -= camera->Speed;
    }
    if(moveRight)
    {
        camera->Position.x += camera->Speed;
    }
    if(moveUp)
    {
        camera->Position.y += camera->Speed;
    }
    if(moveDown)
    {
        camera->Position.y -= camera->Speed;
    }
}
