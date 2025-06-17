#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform writeonly image2D renderTarget;

layout(location = 0) uniform mat4 cameraTransform;
layout(location = 1) uniform vec4 viewportDimensions;


const float collisionDistance = 0.001f;
const int maxSteps = 32;

const int voxelBrickSize = 4;
const float cubeWidth = 1.0f / voxelBrickSize;
const uint64_t voxelBrick = 0x5A5A000F88FFFFFFul;
//0101 1010 0101 1010
//0000 0000 0000 1111
//1000 1000 1111 1111
//1111 1111 1111 1111

// 1
//1 1 1 1 1 1 1 1

//0101 1010 0101 1010
//0000 0000 0000 1111

//1000 1000 1111 1111
//1111 1111 1111 1111

layout (location = 2) uniform uint octree[9];

#define cameraRight vec3(cameraTransform[0])
#define cameraUp vec3(cameraTransform[1])
#define cameraForward vec3(cameraTransform[2])
#define cameraPos vec3(cameraTransform[3])

#define nearPlaneDistance viewportDimensions.z
#define farPlaneDistance viewportDimensions.w

struct Ray
{
    vec3 Origin;
    vec3 Direction;
};

struct RayDestination
{
    bool WasHit;
};

RayDestination StepDDA(Ray ray)
{
    vec3 cellOrigin = floor(ray.Origin);

    vec3 firstStep = floor(ray.Origin + ray.Direction);

    float deltaX = 1.0f / ray.Direction.x;
    float deltaY = 1.0f / ray.Direction.y;
    float deltaZ = 1.0f / ray.Direction.z;

    ray.Origin = 

    return RayDestination(false);
}

float SignedDistanceFromCube(vec3 cubeCenter, float cubeWidth, vec3 point)
{
    point -= cubeCenter; // ->to origin
    vec3 distance = abs(point) - (cubeWidth * 0.5f);
    float maxComponent = max(max(distance.x, distance.y), distance.z);
    return length(max(distance, 0.0f)) + min(maxComponent, 0.0f);
}

void main()
{
    vec2 dimensions = imageSize(renderTarget);
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    if (pixelCoord.x >= dimensions.x || pixelCoord.y >= dimensions.y)
    {
        return;
    }

    vec2 clipSpacePixelCoord = vec2 //[-1, 1]
    (
        float(pixelCoord.x * 2 - dimensions.x) / dimensions.x,
        float(pixelCoord.y * 2 - dimensions.y) / dimensions.y
    );

    vec3 viewportU = viewportDimensions.x * cameraRight;
    vec3 viewportV = viewportDimensions.y * cameraUp;

    vec3 pixelPos =
        cameraPos +
        viewportU * clipSpacePixelCoord.x * 0.5f +
        viewportV * clipSpacePixelCoord.y * 0.5f +
        cameraForward * nearPlaneDistance;

    Ray ray = Ray(pixelPos, normalize(pixelPos - cameraPos));

    int stepCount = 0;
    float totalDistance = 0.0f;
    float minDistance = farPlaneDistance;
    bool hit = false;
    vec3 hitNormal;

    while(totalDistance < farPlaneDistance)
    {
        StepDDA(ray);
        float distance = farPlaneDistance;
        vec3 hitCubeCenter = vec3(0.0f);

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
        imageStore(renderTarget, pixelCoord, vec4(0.5f * (hitNormal + 1.0f), 1.0f));
    }
}
