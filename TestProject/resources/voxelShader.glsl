#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

uniform mat4 cameraTransform;

const float vFov = 65;

const float nearPlaneDistance = 0.01;
const float farPlaneDistance = 50.0;

const float collisionDistance = 0.0001;
const int maxSteps = 32;

const int voxelBrickSize = 4;
const uint64_t voxelBrick = 0xA5A5F000FF88FFFFul;
//1010 0101 1010 0101;
//1111 0000 0000 0000;
//1111 1111 1000 1000;
//1111 1111 1111 1111;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform writeonly image2D renderTarget;

struct Ray
{
    vec3 Origin;
    vec3 Direction;
};

float SignedDistanceFromSphere(vec3 sphereCenter, float sphereRadius, vec3 point);
float SignedDistanceFromCube(vec3 cubeCenter, float cubeSize, vec3 point);
bool HitSphere(vec3 sphereCenter, float sphereRadius, vec3 rayOrigin, vec3 rayDirection);

void main()
{
    vec2 dimensions = imageSize(renderTarget);
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 clipSpacePixelCoord = vec2 //[-1, 1]
    (
        -(float(pixelCoord.x * 2 - dimensions.x) / dimensions.x),
        -(float(pixelCoord.y * 2 - dimensions.y) / dimensions.y)
    );

    //Camera
    vec3 cameraRight = vec3(cameraTransform[0]);
    vec3 cameraUp = vec3(cameraTransform[1]);
    vec3 cameraForward = vec3(cameraTransform[2]);
    vec3 cameraPos = vec3(cameraTransform[3]);

    //Viewport
    float viewportHeight = nearPlaneDistance * tan(radians(vFov * 0.5)) * 2.0;
    float viewportWidth = viewportHeight * (dimensions.x / dimensions.y);
    vec3 viewportU = viewportWidth * cameraRight;
    vec3 viewportV = viewportHeight * cameraUp;

    vec3 pixelPos =
        cameraPos +
        -viewportU * clipSpacePixelCoord.x * 0.5 +
        -viewportV * clipSpacePixelCoord.y * 0.5 +
        cameraForward * nearPlaneDistance;

    Ray ray = Ray(pixelPos, normalize(pixelPos - cameraPos));

    float cubeWidth = 1.0 / voxelBrickSize;

    int stepCount = 0;
    float totalDistance = 0;
    float minDistance = farPlaneDistance;
    bool hit = false;
    vec3 hitNormal;

    while(totalDistance < farPlaneDistance)
    {
        float distance = farPlaneDistance ;
        vec3 hitCubeCenter = vec3(0);

        for(int i = 0; i < voxelBrickSize * voxelBrickSize * voxelBrickSize; ++i)
        {
            if(((1ul << i) & voxelBrick) == 0)
            {
                continue;
            }

            uvec3 cubeID = uvec3
            (
                i % voxelBrickSize,
                (i / voxelBrickSize) / voxelBrickSize,
                (i / voxelBrickSize) % voxelBrickSize
            );

            vec3 cubeCenter = vec3(0) + (cubeID * cubeWidth);

            float newDistance = abs(SignedDistanceFromCube(cubeCenter, cubeWidth, ray.Origin));

            if(newDistance < distance)
            {
                distance = newDistance;
                hitCubeCenter = cubeCenter;
            } 

            minDistance = min(distance, minDistance);
        }

        if(distance < collisionDistance)
        {
            hit = true;
            hitNormal = normalize(ray.Origin - hitCubeCenter);
            vec3 absoluteNormal = abs(hitNormal);
            float maxComponent = max(max(absoluteNormal.x, absoluteNormal.y), absoluteNormal.z);
            vec3 normal = vec3
            (
                step(maxComponent, absoluteNormal.x) * sign(hitNormal.x),
                step(maxComponent, absoluteNormal.y) * sign(hitNormal.y),
                step(maxComponent, absoluteNormal.z) * sign(hitNormal.z)
            );
    
            hitNormal = normalize(normal);
            break;
        }

        ray.Origin += distance * ray.Direction;
        totalDistance += distance;

        stepCount++;
    }

    if(hit && totalDistance > 0)
    {
        imageStore(renderTarget, pixelCoord, vec4(0.5 * (hitNormal + 1.0), 1.0));
    }
}

float SignedDistanceFromSphere(vec3 sphereCenter, float sphereRadius, vec3 point)
{
    return distance(sphereCenter, point) - sphereRadius;
}

float SignedDistanceFromCube(vec3 cubeCenter, float cubeWidth, vec3 point)
{
    point -= cubeCenter; // ->to origin
    vec3 distance = abs(point) - (cubeWidth * 0.5);
    float maxComponent = max(max(distance.x, distance.y), distance.z);
    return length(max(distance, 0.0)) + min(maxComponent, 0.0);
}

bool HitSphere(vec3 sphereCenter, float sphereRadius, vec3 rayOrigin, vec3 rayDirection)
{
    vec3 oc = sphereCenter - rayOrigin;
    float a = dot(rayDirection, rayDirection);
    float b = -2.0 * dot(rayDirection, oc);
    float c = dot(oc, oc) - sphereRadius*sphereRadius;
    float discriminant = b*b - 4*a*c;
    return (discriminant >= 0);
}
