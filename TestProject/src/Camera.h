#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#include <Math/Vec3f.h>

typedef struct Camera
{
    Vec3f Position;
    Vec3f Forward;
    float Speed;
} Camera;

void CameraProcessInput(Camera* camera, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool moveUp, bool moveDown);

#endif // CAMERA_H
