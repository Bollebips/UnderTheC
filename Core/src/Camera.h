#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#include <cglm/struct.h>

typedef struct Camera
{
    mat4s Transform;
    float LinearSpeed;
    float AngularSpeed;

    vec3s AxisInput;
    vec2s RotationInput;

    float NearPlaneDistance;
    float FarPlaneDistance;
    float VerticalFov;
} Camera;

void CameraProcessInput(Camera* camera, float deltaTime);
void CameraRecieveInput(Camera* camera, int key, int action);

#endif // CAMERA_H
