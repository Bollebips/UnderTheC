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
const uint64_t voxelBrick_OLD = 0x505A000F88FFFFFFul;
//0101 0000 0101 1010
//0000 0000 0000 1111
//1000 1000 1111 1111
//1111 1111 1111 1111

// 1
//1 1 1 1 1 1 1 1
                            
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

struct DDA
{
    vec3 NormalizedOrigin;
    vec3 InitialSideDistace;
    vec3 CellDeltaDistance;
    vec3 TotalDeltaDistance;
    ivec3 CellStep;
    ivec3 Chunk;
    ivec3 CurrentStepDirection;
    float CellSize;
    float TotalDistance;
};

struct VoxelBrick
{
    uint Occupancy;
    uint AbsoluteIndex;
    int Level;
    vec3 Center;
    // TODO: Specify max tree depth
    uint ParentIndeces[5];
};

struct VoxelHit
{
    bool IsHit;
    ivec3 HitVoxelIndex;
    int VoxelIndex;
    vec3 HitNormal;
};

void StepLevelDownOctree(inout DDA dda, inout VoxelBrick voxelBrick, VoxelHit hit)
{
    uint nrOfSiblingsToSkip = 0;
    uint occupancyMask = voxelBrick.Occupancy;
    occupancyMask >>= hit.VoxelIndex;
    while (occupancyMask > 0)
    {
        occupancyMask &= (occupancyMask - 1);
        nrOfSiblingsToSkip++;
    }

    voxelBrick.ParentIndeces[voxelBrick.Level] = voxelBrick.AbsoluteIndex;
    voxelBrick.AbsoluteIndex += nrOfSiblingsToSkip;
    voxelBrick.Level++;
    dda.CellSize = pow(0.5f, voxelBrick.Level);
    voxelBrick.Center += dda.CellSize * (vec3(hit.HitVoxelIndex) - 0.5f); 
}

void StepLevelUpOctree(inout DDA dda, inout VoxelBrick voxelBrick, const VoxelHit hit)
{
    voxelBrick.AbsoluteIndex = voxelBrick.ParentIndeces[voxelBrick.Level];
    voxelBrick.Level--;
    dda.CellSize = pow(0.5f, voxelBrick.Level);
    voxelBrick.Center -= dda.CellSize * (vec3(hit.HitVoxelIndex) - 0.5f); 
}

VoxelHit CheckVoxelHit(const VoxelBrick voxelBrick, const vec3 hitPosition, const ivec3 stepDirection)
{
    float voxelSize = pow(0.5f, voxelBrick.Level);

    ivec3 voxelAxisIndex = ivec3(ceil(hitPosition - voxelBrick.Center));

    int voxelIndex = voxelAxisIndex.x + voxelAxisIndex.y * 2 + voxelAxisIndex.z * 4;
    return VoxelHit
    ( 
        ((1 << voxelIndex) & octree[voxelBrick.AbsoluteIndex]) != 0,
        voxelAxisIndex,
        voxelIndex,
        stepDirection * -1
    );
}

DDA StartDDA(const Ray ray)
{
    float chunkSize = 1f;
    float cellSize = chunkSize;
    int level = 0;
    int minLevel = 1;

    DDA dda;
    dda.CellSize = chunkSize;
    dda.NormalizedOrigin = ray.Origin / cellSize;
    // TODO: handle 0 division
    dda.CellDeltaDistance = abs(1.0f / ray.Direction);
    dda.Chunk = ivec3(floor(ray.Origin / chunkSize));
    dda.CellStep = ivec3(sign(ray.Direction));

    dda.InitialSideDistace.x = ray.Direction.x < 0 ?
        dda.NormalizedOrigin.x - (floor(dda.NormalizedOrigin.x)):
        (floor(dda.NormalizedOrigin.x) + 1) - dda.NormalizedOrigin.x;

    dda.InitialSideDistace.y = ray.Direction.y < 0 ?
        dda.NormalizedOrigin.y - (floor(dda.NormalizedOrigin.y)):
        (floor(dda.NormalizedOrigin.y) + 1) - dda.NormalizedOrigin.y;

    dda.InitialSideDistace.z = ray.Direction.z < 0 ?
        dda.NormalizedOrigin.z - (floor(dda.NormalizedOrigin.z)):
        (floor(dda.NormalizedOrigin.z) + 1) - dda.NormalizedOrigin.z;

    dda.TotalDeltaDistance = dda.InitialSideDistace * dda.CellDeltaDistance * cellSize;
    dda.TotalDistance = 0.0f;

    return dda;
}

void StepDDA(inout DDA dda)
{
    if(dda.TotalDeltaDistance.x < dda.TotalDeltaDistance.y && dda.TotalDeltaDistance.x < dda.TotalDeltaDistance.z)
    {
        dda.TotalDistance = dda.TotalDeltaDistance.x;
        dda.TotalDeltaDistance.x += dda.CellDeltaDistance.x * dda.CellSize;
        dda.CurrentStepDirection = ivec3(dda.CellStep.x, 0, 0);
    }
    else if(dda.TotalDeltaDistance.y < dda.TotalDeltaDistance.x && dda.TotalDeltaDistance.y < dda.TotalDeltaDistance.z)
    {
        dda.TotalDistance = dda.TotalDeltaDistance.y;
        dda.TotalDeltaDistance.y += dda.CellDeltaDistance.y * dda.CellSize;
        dda.CurrentStepDirection = ivec3(0, dda.CellStep.y, 0);
    }
    else
    {
        dda.TotalDistance = dda.TotalDeltaDistance.z;
        dda.TotalDeltaDistance.z += dda.CellDeltaDistance.z * dda.CellSize;
        dda.CurrentStepDirection = ivec3(0, 0, dda.CellStep.z);
    }
}

VoxelHit TraverseChunk(inout DDA dda, in VoxelBrick voxelBrick, vec3 hitPos)
{
    int maxLevel = 0;
    VoxelHit hit;

    //TODO: Check if we should step down a level or step up
            
    while(voxelBrick.Level >= 0)
    {
        hit = CheckVoxelHit(voxelBrick, hitPos, dda.CurrentStepDirection);
        //find voxel octree level we can traverse
        if(hit.IsHit)
        {
            if(voxelBrick.Level < maxLevel)
            {
                StepLevelDownOctree(dda, voxelBrick, hit);
            }
            else
            {
                return hit;
            }
        }
        else
        {
            //Stepping INSIDE the voxel brick
            ivec3 previousHitVoxelIndex = hit.HitVoxelIndex;
            dda.CellSize = pow(0.5f, voxelBrick.Level + 1);
            StepDDA(dda);
            ivec3 nextVoxelIndexToCheck = previousHitVoxelIndex + dda.CurrentStepDirection;
            //if(any(lessThan(nextVoxelIndexToCheck, uvec3(0))) || any(greaterThan(nextVoxelIndexToCheck, uvec3(1))))
           // if (nextVoxelIndexToCheck.x < 0 || nextVoxelIndexToCheck.x > 1 ||
           //     nextVoxelIndexToCheck.y < 0 || nextVoxelIndexToCheck.y > 1 ||
           //     nextVoxelIndexToCheck.z < 0 || nextVoxelIndexToCheck.z > 1)
           if(false)
           {
               //go to sibling of parent
               return hit;
               //StepLevelUpOctree(dda, voxelBrick, hit);
           }
           else
           {
               return hit;
               //go to sibling of self
               StepLevelUpOctree(dda, voxelBrick, hit);
               hit.HitVoxelIndex = nextVoxelIndexToCheck;
               StepLevelDownOctree(dda, voxelBrick, hit);
           }
        }
    }
    
    return hit;
}

bool IsChunkOccupied(const ivec3 chunkIndex)
{
    return chunkIndex == ivec3(0);
}

VoxelHit TraverseDDA(const Ray ray, float maxDistance)
{
    DDA dda = StartDDA(ray);

    VoxelHit result = VoxelHit(false, ivec3(0), 0, vec3(0));
    VoxelBrick voxelBrick;

    while(dda.TotalDistance < maxDistance && result.IsHit == false)
    {
        // TODO: Change this to also use CheckVoxelHit instead of a separate chunk function
        // the voxelBrick should be retrieved from the chunk it's checking
        if(IsChunkOccupied(dda.Chunk))
        {
            //get chunk data
            voxelBrick.Occupancy = octree[0];
            voxelBrick.AbsoluteIndex = 0;
            voxelBrick.Level = 0;
            voxelBrick.Center = vec3(0.5f);

            vec3 hitPos = ray.Origin + ray.Direction * dda.TotalDistance;
            result = TraverseChunk(dda, voxelBrick, hitPos);

            //if(result.IsHit)
            {
                break;
            }
        }

        StepDDA(dda);
        dda.Chunk += dda.CurrentStepDirection;
    }

    vec3 collisionPoint = ray.Origin + ray.Direction * dda.TotalDistance;

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

    VoxelHit result = TraverseDDA(ray, farPlaneDistance);
    if(result.IsHit)
    {
        imageStore(renderTarget, pixelCoord, vec4(0.5f * (result.HitNormal + 1.0f), 1.0f));
    }
    else if(false)
    {
    while(totalDistance < farPlaneDistance)
    {
        float distance = farPlaneDistance;
        vec3 hitCubeCenter = vec3(0.0f);

        for(int i = 0; i < voxelBrick_OLD * voxelBrick_OLD * voxelBrick_OLD; ++i)
        {
            if(((1ul << i) & voxelBrick_OLD) == 0)
            {
                continue;
            }

            uvec3 cubeID = uvec3
            (
                i % voxelBrick_OLD,
                (i / voxelBrick_OLD) / voxelBrick_OLD,
                (i / voxelBrick_OLD) % voxelBrick_OLD
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
}
