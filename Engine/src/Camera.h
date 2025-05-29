#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#include <cglm/struct.h>

typedef struct Camera
{
    mat4s Transform;
    float Speed;
    float AngularSpeed;
    float NearPlaneDistance;
    float FarPlaneDistance;
    float VerticalFov;
} Camera;

void CameraProcessInput(Camera* camera, float deltaTime, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool moveUp, bool moveDown, bool lookDown, bool lookUp, bool lookLeft, bool lookRight);


#endif // CAMERA_H
