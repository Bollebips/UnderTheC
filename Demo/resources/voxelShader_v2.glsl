#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform writeonly image2D renderTarget;

layout(location = 0) uniform mat4 cameraTransform;
layout(location = 1) uniform vec4 viewportDimensions;

//const float chunkSize = 1.0f;
//const int voxelsPerChunk = 4;

//              1
//    /     |      |     \
//   1      0      1      1
//  / \    / \    / \    / \
// 2  2   0  0   2  2   D  D
// 01 01  00 00  01 01  10 10
// 00 00  00 00  00 00  11 11

//   1      1      1      1
//  / \    / \    / \    / \
// D  C   D  C   F  F   F  F
// 10 00  10 00  11 11  11 11
// 11 11  11 11  11 11  11 11

layout(location = 2) uniform uint octree[9];

#define cameraRight vec3(cameraTransform[0])
#define cameraUp vec3(cameraTransform[1])
#define cameraForward vec3(cameraTransform[2])
#define cameraPos vec3(cameraTransform[3])

#define nearPlaneDistance viewportDimensions.z
#define farPlaneDistance viewportDimensions.w

bool IsChunkOccupied(const ivec3 chunkIndex)
{
    return chunkIndex == ivec3(0);
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
            float(pixelCoord.y * 2 - dimensions.y) / dimensions.y);

    vec3 viewportU = viewportDimensions.x * cameraRight;
    vec3 viewportV = viewportDimensions.y * cameraUp;

    vec3 pixelPos =
        cameraPos +
            viewportU * clipSpacePixelCoord.x * 0.5f +
            viewportV * clipSpacePixelCoord.y * 0.5f +
            cameraForward * nearPlaneDistance;

    // Initialize

    vec3 rayOrigin = pixelPos;
    vec3 rayDir = normalize(pixelPos - cameraPos);
    ivec3 rayStepDirection = ivec3(sign(rayDir));
    ivec3 rayCurrentStepDirection;

    int maxVoxelLevel = 4;
    float chunkScale = 1f;

    int currentVoxelParent; // the parent of the current voxel being traversed
    int currentVoxelIndex; // the child index of the current voxel (0-7)
    vec3 currentVoxelPos; // position of the current voxel
    int currentVoxelLevel = 4; // level down the voxel hierarchy. Higher level -> higher up the hierarchy
    float currentVoxelScale = float(chunkScale) * pow(2, currentVoxelLevel - maxVoxelLevel);

    int minVoxelLevel = 4; // TEMP

    ivec3 currentChunk = ivec3(floor(rayOrigin / chunkScale));
    vec3 rayOriginRelativeToVoxel = rayOrigin / currentVoxelScale;

    float totalRayDistance = 0f;
    vec3 totalDistanceToNextAxisIntersection;
    vec3 distanceBetweenAxisIntersections = abs(1.0f / rayDir) * currentVoxelScale;
    vec3 initialDistanceToAxisIntersection;
    // TODO: This only calculates the distance to the next chunk, but doesn't take into account the current chunk/voxel we are in
    initialDistanceToAxisIntersection.x = rayDir.x < 0 ? rayOriginRelativeToVoxel.x - (floor(rayOriginRelativeToVoxel.x)) : (floor(rayOriginRelativeToVoxel.x) + 1) - rayOriginRelativeToVoxel.x;
    initialDistanceToAxisIntersection.y = rayDir.y < 0 ? rayOriginRelativeToVoxel.y - (floor(rayOriginRelativeToVoxel.y)) : (floor(rayOriginRelativeToVoxel.y) + 1) - rayOriginRelativeToVoxel.y;
    initialDistanceToAxisIntersection.z = rayDir.z < 0 ? rayOriginRelativeToVoxel.z - (floor(rayOriginRelativeToVoxel.z)) : (floor(rayOriginRelativeToVoxel.z) + 1) - rayOriginRelativeToVoxel.z;
    // still need to multiply by the normalized distance between intersections, since the calculations above don't take that into account
    initialDistanceToAxisIntersection *= distanceBetweenAxisIntersections;

    totalDistanceToNextAxisIntersection = initialDistanceToAxisIntersection * currentVoxelScale;

    bool rayHit = false;

    while (totalRayDistance <= farPlaneDistance && rayHit == false)
    {
        if (currentChunk == ivec3(0))
        {
            // remove if we didn't hit a voxel
            rayHit = true;

            currentVoxelParent = 0;
            // determine which child voxel we hit
            vec3 hitPos = rayOrigin + rayDir * totalRayDistance;
            vec3 parentVoxelCenter = currentVoxelPos + (float(currentVoxelScale) / 2.0f);
            ivec3 currentVoxelAxisIndex = ivec3(ceil(hitPos - parentVoxelCenter));
            currentVoxelIndex = currentVoxelAxisIndex.x + currentVoxelAxisIndex.y * 2 + currentVoxelAxisIndex.z * 4;

            break;
        }

        // STEP through chunk
        if (totalDistanceToNextAxisIntersection.x < totalDistanceToNextAxisIntersection.y && totalDistanceToNextAxisIntersection.x < totalDistanceToNextAxisIntersection.z)
        {
            totalRayDistance = totalDistanceToNextAxisIntersection.x;
            totalDistanceToNextAxisIntersection.x += distanceBetweenAxisIntersections.x * currentVoxelScale;
            rayCurrentStepDirection = ivec3(rayStepDirection.x, 0, 0);
        }
        else if (totalDistanceToNextAxisIntersection.y < totalDistanceToNextAxisIntersection.x && totalDistanceToNextAxisIntersection.y < totalDistanceToNextAxisIntersection.z)
        {
            totalRayDistance = totalDistanceToNextAxisIntersection.y;
            totalDistanceToNextAxisIntersection.y += distanceBetweenAxisIntersections.y * currentVoxelScale;
            rayCurrentStepDirection = ivec3(0, rayStepDirection.y, 0);
        }
        else
        {
            totalRayDistance = totalDistanceToNextAxisIntersection.z;
            totalDistanceToNextAxisIntersection.z += distanceBetweenAxisIntersections.z * currentVoxelScale;
            rayCurrentStepDirection = ivec3(0, 0, rayStepDirection.z);
        }

        currentChunk += rayCurrentStepDirection;
    }

    if (rayHit)
    {
        imageStore(renderTarget, pixelCoord, vec4(float(currentVoxelIndex) / 8.0f, 0.0f, 0.0f, 1.0f));
    }
}
