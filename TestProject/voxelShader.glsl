#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

uniform vec3 cameraPos = vec3(0, 1, 0);
uniform vec3 cameraForward;
uniform float collisionDistance = 0.0001;
uniform int maxSteps = 32;
uniform float maxDistance = 50;

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

    float x = -(float(pixelCoord.x * 2 - dimensions.x) / dimensions.x); //[-1, 1]
    float y = -(float(pixelCoord.y * 2 - dimensions.y) / dimensions.y); //[-1, 1]

    vec3 origin = vec3(0,0,0);

    //Camera
    // TODO: Move this out of the shader
    vec3 worldUp = vec3(0, 1, 0);
    vec3 cameraRight = normalize(cross(worldUp, cameraForward));
    vec3 cameraUp = cross(cameraForward, cameraRight);
    float vFov = 90.0;
    float focalLength = 0.01; //distance between camera origin and viewport

    //Viewport
    // TODO: Move this out of the shader
    float viewportHeight = 0.02;
    float viewportWidth = viewportHeight * (dimensions.x / dimensions.y); //2.6
    vec3 viewportU = viewportWidth * cameraRight; //(2.0, 0.0, 0.0)
    vec3 viewportV = viewportHeight * cameraUp; //(0.0, -2.6, 0.0)
    vec3 pixelDeltaU = viewportU / dimensions.x;
    vec3 pixelDeltaV = viewportV / dimensions.y;
    vec3 viewportUpperLeft = cameraPos + focalLength * cameraForward - viewportU / 2.0 - viewportV / 2.0;
    vec3 pixel00Pos = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
    vec3 pixelPos = pixel00Pos + (pixelCoord.x * pixelDeltaU) + (pixelCoord.y * pixelDeltaV);

    Ray ray = Ray(pixelPos, normalize(pixelPos - cameraPos));

    float cubeWidth = 1.0 / voxelBrickSize;

    int stepCount = 0;
    float totalDistance = 0;
    float minDistance = maxDistance;
    bool hit = false;
    vec3 hitNormal;

    while(totalDistance < maxDistance)
    {
        float distance = maxDistance;
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

            /* vec3 repetitiveRayOrigin = vec3(fract(ray.Origin.x) - 0.5, fract(ray.Origin.y) - 0.5, fract(ray.Origin.z) - 0.5); */
            /* vec3 repetitiveRayOrigin = vec3(mod(ray.Origin.x, 2.0) - 1.0, mod(ray.Origin.y, 2.0) - 1.0, mod(ray.Origin.z, 2.0) - 1.0); */
            float newDistance = SignedDistanceFromCube(cubeCenter, cubeWidth, ray.Origin);

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
            /* vec3 repetitiveRayOrigin = vec3(fract(ray.Origin.x) - 0.5, fract(ray.Origin.y) - 0.5, fract(ray.Origin.z) - 0.5); */
            /* vec3 repetitiveRayOrigin = vec3(mod(ray.Origin.x, 2.0) - 1.0, mod(ray.Origin.y, 2.0) - 1.0, mod(ray.Origin.z, 2.0) - 1.0); */
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

    vec4 color = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0, 0, 1.0, 1.0), 0.5 * (ray.Direction.y + 1.0));

    if(hit && totalDistance > 0)
    {
        imageStore(renderTarget, pixelCoord, vec4(0.5 * (hitNormal + 1.0), 1.0));
    }
    else
    {
        /* imageStore(renderTarget, pixelCoord, vec4(1) * (1.0 - minDistance)); */
    }
}

float SignedDistanceFromSphere(vec3 sphereCenter, float sphereRadius, vec3 point)
{
    return distance(sphereCenter, point) - sphereRadius;
}

float SignedDistanceFromCube(vec3 cubeCenter, float cubeWidth, vec3 point)
{
    point -= cubeCenter; // ->to origin
    vec3 distance = abs(point) - (cubeWidth / 2.0);
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
