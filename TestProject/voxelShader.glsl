#version 460 core

uniform vec3 cameraPos = vec3(0, 1, 0);
uniform vec3 cameraForward;
uniform float collisionDistance = 0.001;
uniform int maxSteps = 32;
uniform float maxDistance = 10;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform writeonly image2D renderTarget;

float SignedDistanceFromSphere(vec3 sphereCenter, float sphereRadius, vec3 point);
float SignedDistanceFromCube(vec3 cubeCenter, float cubeSize, vec3 point);
bool HitSphere(vec3 sphereCenter, float sphereRadius, vec3 rayOrigin, vec3 rayDirection);

void main()
{
    vec2 dimensions = imageSize(renderTarget);
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    // BOUNDS CHECK
    /* if (pixelCoord.x >= dimensions.x || pixelCoord.y >= dimensions.y) */
        /* return; */
    /* float aspectRatio = dimensions.x / dimensions.y; */

    float x = -(float(pixelCoord.x * 2 - dimensions.x) / dimensions.x); //[-1, 1]
    float y = -(float(pixelCoord.y * 2 - dimensions.y) / dimensions.y); //[-1, 1]

    vec3 origin = vec3(0,0,0);

    //Camera
    /* vec3 cameraPos = vec3(10.0, 10.0, 10.0); */
    vec3 worldUp = vec3(0, 1, 0);
    /* vec3 cameraLookTarget = vec3(0, 0.5, 1.0); */
    /* vec3 cameraForward = normalize(cameraLookTarget - cameraPos); */
    vec3 cameraRight = normalize(cross(worldUp, cameraForward));
    vec3 cameraUp = cross(cameraForward, cameraRight);
    float fov = 90.0;
    float focalLength = 1.0; //distance between camera origin and viewport

    //Viewport
    float viewportHeight = 2.0f;
    float viewportWidth = viewportHeight * (dimensions.x / dimensions.y); //2.6
    vec3 viewportU = viewportWidth * cameraRight; //(2.0, 0.0, 0.0)
    vec3 viewportV = viewportHeight * cameraUp; //(0.0, -2.6, 0.0)
    vec3 pixelDeltaU = viewportU / dimensions.x;
    vec3 pixelDeltaV = viewportV / dimensions.y;
    vec3 viewportUpperLeft = cameraPos + focalLength * cameraForward - viewportU / 2.0 - viewportV / 2.0;
    vec3 pixel00Pos = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
    vec3 pixelPos = pixel00Pos + (pixelCoord.x * pixelDeltaU) + (pixelCoord.y * pixelDeltaV);

    vec3 rayOrigin = pixelPos;
    vec3 rayDirection = normalize(pixelPos - cameraPos);

    float sphereRadius = 0.5;
    vec3 sphereCenter = vec3(0,0,0);

    int stepCount = 0;
    float distance;
    float totalDistance = 0;
    bool hit = false;
    vec3 hitNormal;

    while(totalDistance < maxDistance)
    {
        /* distance = SignedDistanceFromSphere(sphereCenter, sphereRadius, rayOrigin); */
        distance = SignedDistanceFromCube(sphereCenter, sphereRadius, rayOrigin);
        if(distance < collisionDistance)
        {
            hit = true;
            hitNormal = normalize(rayOrigin - sphereCenter);
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

        rayOrigin += distance * rayDirection;
        totalDistance += distance;

        stepCount++;
    }

    /* HitSphere(sphereCenter, sphereRadius, rayOrigin, rayDirection); */

    vec4 color = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0, 0, 1.0, 1.0), 0.5 * (rayDirection.y + 1.0));

    if(hit && distance > 0)
    {
        imageStore(renderTarget, pixelCoord, vec4(0.5 * (hitNormal + vec3(1,1,1)), 1.0));
    }
    else if(stepCount > 20)
    {
        imageStore(renderTarget, pixelCoord, vec4(1,1,1,1));
        /* imageStore(renderTarget, pixelCoord, vec4(1,1,1,1) * 1.0 - ((float(stepCount) / float(maxSteps)))); */
    }
    /* imageStore(renderTarget, pixelCoord, vec4(pixelPos.x / (viewportWidth * 0.5), pixelPos.y / (viewportHeight * 0.5), 0.0, 0.1)); */
    /* imageStore(renderTarget, pixelCoord, vec4((14.0 - distance), (14.0 - distance), 0.0, 0.1)); */
    /* imageStore(renderTarget, pixelCoord, vec4(gl_LocalInvocationID.x/16.0, gl_LocalInvocationID.y/16.0, 0.0, 1.0)); */
}

float SignedDistanceFromSphere(vec3 sphereCenter, float sphereRadius, vec3 point)
{
    return distance(sphereCenter, point) - sphereRadius;
}

float SignedDistanceFromCube(vec3 cubeCenter, float cubeHalfWidth, vec3 point)
{
    point -= cubeCenter; // ->to origin
    vec3 distance = abs(point) - (vec3(1,1,1) * cubeHalfWidth);
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
