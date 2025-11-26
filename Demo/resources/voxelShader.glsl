#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform writeonly image2D renderTarget;

layout(location = 0) uniform mat4 cameraTransform;
layout(location = 1) uniform vec4 viewportDimensions;

struct ParentVoxel
{
    ivec3 voxelAxisIndex;
    int octreeIndex;
};

const int maxDepth = 1;
int currentDepth = 0;
ParentVoxel[maxDepth] parentStack; // stores the index in the octree array
int currentChildSlotIndex = 0; // [0-7]
vec3 currentParentVoxelCenter;

const float collisionDistance = 0.001f;
const int maxSteps = 32;

const int voxelBrickSize = 4;
const float cubeWidth = 1.0f / voxelBrickSize;
const uint64_t voxelBrick_OLD = 0x505A000F88FFFFFFul;
//0101 0000 0101 1010
//0000 0000 0000 1111
//1000 1000 1111 1111
//1111 1111 1111 1111

// 1
//1 0 1 1 1 1 1 1

// 2  2   0  0   2  2   D  D
// 01 01  00 00  01 01  10 10
// 00 00  00 00  00 00  11 11

// D  C   D  C   F  F   F  F
// 10 00  10 00  11 11  11 11
// 11 11  11 11  11 11  11 11

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
    bool Hit;
    vec4 color;
};

RayDestination StepDDA(const Ray ray, float maxDistance)
{
    //TODO: scale with different cell size

    // TODO: consider making the smallest cellSize = 2
    // -> this way, a some calculations could be limited to ints and binary operations
    float cellSize = 1;

    float dx = ray.Direction.x;
    float dy = ray.Direction.y;
    float dz = ray.Direction.z;
    
    float ox = ray.Origin.x / cellSize;
    float oy = ray.Origin.y / cellSize;
    float oz = ray.Origin.z / cellSize;

    float cellSizeSqr = pow(cellSize, 2);

    // TODO: handle 0 division
    vec3 c = abs(1.0f / ray.Direction);

    ivec3 cell = ivec3(floor(ray.Origin / cellSize));
    ivec3 cellStep = ivec3(sign(ray.Direction));

    float ax = dx < 0 ?
        ox - (floor(ox)):
        (floor(ox) + 1) - ox;

    float ay = dy < 0 ?
        oy - (floor(oy)):
        (floor(oy) + 1) - oy;

    float az = dz < 0 ?
        oz - (floor(oz)):
        (floor(oz) + 1) - oz;

    float nextTx = ax * c.x * cellSize;
    float nextTy = ay * c.y * cellSize;
    float nextTz = az * c.z * cellSize;

    float tx = 0;
    float ty = 0;
    float tz = 0;

    float totalDistance = 0.0f;

    RayDestination result = RayDestination(false, vec4(0));
    vec3 hitPos = ray.Origin;

    while(totalDistance < maxDistance && result.Hit == false)
    {
        //chunk hit
        if(cell == ivec3(0, 0, 0))
        {
            result.Hit = true;
            currentParentVoxelCenter = vec3(cellSize) / 2;
            uint currentParentVoxelOccupancy = octree[0];
            vec3 voxelMin = vec3(0);
            vec3 voxelMax = vec3(1);
            ivec3 voxelAxisIndex = ivec3(round((hitPos - voxelMin) / (voxelMax - voxelMin)));
            currentChildSlotIndex = voxelAxisIndex.z + (2 * voxelAxisIndex.y) + (4 * voxelAxisIndex.x);
            result.color = vec4(0, 0, 0, 1);
            int colorIndex = 0;
            result.color[currentDepth] = currentChildSlotIndex / 7.0f;

            bool terminatedChunk = false;

            while(terminatedChunk == false)
            {
                if((currentParentVoxelOccupancy & (uint(1) << currentChildSlotIndex)) != 0)
                {
                    //PUSH through to child voxel
                    ivec3 centerMovementMultiplier = (voxelAxisIndex * 2) - 1;
                    cellSize /= 2.0f;
                    currentParentVoxelCenter += (cellSize / 2.0f) * centerMovementMultiplier;
                    voxelMin = currentParentVoxelCenter - vec3(cellSize);
                    voxelMax = currentParentVoxelCenter + vec3(cellSize);

                    //ivec3 voxelAxisIndex = ivec3(round((hitPos / cellSize) - trunc(hitPos / cellSize)));
                    // TODO: Find a formula without division? Maybe using sign()?
                    // TODO: parentStack[currentDepth] = ParentVoxel(voxelAxisIndex, currentChildSlotIndex);
                    currentDepth += 1;
                    voxelAxisIndex = ivec3(round((hitPos - voxelMin) / (voxelMax - voxelMin)));
                    currentChildSlotIndex = voxelAxisIndex.z + (2 * voxelAxisIndex.y) + (4 * voxelAxisIndex.x);
                    result.color[currentDepth] = currentChildSlotIndex / 7.0f;

                    if(currentDepth <= maxDepth)
                    {
                        terminatedChunk = true;
                    }

                    continue;
                }

                //ADVANCE to the next sibling voxel
                // TODO: assume that we travel through the same parent voxel
                // find which axis to step to, and flip that bit voxelAxisIndex corresponding to that axis
                // next, check if the flipped bit agrees with the direction of the ray
                // e.g. if dx > 0, then the bit of the x axis is only allowed to increase, not decrease
                // if we encounter no conflict, we are allowed to ADVANCE 

                

                terminatedChunk = true;
            }

            //result.normal = voxelAxisIndex;

            break;
        }

        if(nextTx < nextTy && nextTx < nextTz)
        {
            totalDistance = tx;
            tx = nextTx;
            nextTx += c.x * cellSize;
            cell.x += cellStep.x;
        }
        else if(nextTy < nextTx && nextTy < nextTz)
        {
            totalDistance = ty;
            ty = nextTy;
            nextTy += c.y * cellSize;
            cell.y += cellStep.y;
        }
        else
        {
            totalDistance = tz;
            tz = nextTz;
            nextTz += c.z * cellSize;
            cell.z += cellStep.z;
        }

        hitPos = ray.Origin + ray.Direction * totalDistance;
        //if(all(greaterThanEqual(point, vec3(0))) && all(lessThanEqual(point, vec3(cellSize))))
        
    }

    vec3 collisionPoint = ray.Origin + ray.Direction * totalDistance;

    return result;
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

    RayDestination result = StepDDA(ray, farPlaneDistance);
    if(result.Hit)
    {
        imageStore(renderTarget, pixelCoord, result.color);
    }
    else
    {
    //OLD
    /*while(totalDistance < farPlaneDistance)
    {
        float distance = farPlaneDistance;
        vec3 hitCubeCenter = vec3(0.0f);

        for(int i = 0; i < voxelBrickSize * voxelBrickSize * voxelBrickSize; ++i)
        {
            if(((1ul << i) & voxelBrick_OLD) == 0)
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
    }*/
    }
}
