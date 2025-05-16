#version 460 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform writeonly image2D renderTarget;

float DistanceFromSphere(vec3 sphereCenter, float sphereRadius, vec3 point);
bool HitSphere(vec3 sphereCenter, float sphereRadius, vec3 rayOrigin, vec3 rayDirection);

void main()
{
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 dimensions = imageSize(renderTarget);
    /* float aspectRatio = dimensions.x / dimensions.y; */

    float x = -(float(pixelCoord.x * 2 - dimensions.x) / dimensions.x); //[-1, 1]
    float y = -(float(pixelCoord.y * 2 - dimensions.y) / dimensions.y); //[-1, 1]

    vec3 origin = vec3(0,0,0);

    //Camera
    /* vec3 cameraPos = vec3(10.0, 10.0, 10.0); */
    vec3 cameraPos = vec3(0, 0, 0);
    /* vec3 cameraForward = normalize(origin - cameraPos); */
    /* float fov = 90.0; */
    float focalLength = 1.0;

    //Viewport
    float viewportHeight = 2.0f;
    float viewportWidth = viewportHeight * (dimensions.x / dimensions.y); //2.6
    vec3 viewportU = vec3(viewportWidth, 0, 0); //(2.0, 0.0, 0.0)
    vec3 viewportV = vec3(0, viewportHeight, 0); //(0.0, -2.6, 0.0)
    vec3 pixelDeltaU = viewportU / dimensions.x;
    vec3 pixelDeltaV = viewportV / dimensions.y;
    vec3 viewportUpperLeft = cameraPos + vec3(0, 0, focalLength) - viewportU / 2.0 - viewportV / 2.0;
    vec3 pixel00Pos = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
    vec3 pixelPos = pixel00Pos + (pixelCoord.x * pixelDeltaU) + (pixelCoord.y * pixelDeltaV);

    vec3 rayOrigin = pixelPos;
    vec3 rayDirection = pixelPos - cameraPos;

    float sphereRadius = 0.5;
    vec3 sphereCenter = vec3(0,0,1);

    /* float distance = DistanceFromSphere(origin, 0.5, rayOrigin); */

    vec4 color = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0, 0, 1.0, 1.0), 0.5 * (rayDirection.y + 1.0));

    if(HitSphere(sphereCenter, sphereRadius, rayOrigin, rayDirection))
    {
        imageStore(renderTarget, pixelCoord, color);
    }
    /* imageStore(renderTarget, pixelCoord, vec4(pixelPos.x / (viewportWidth * 0.5), pixelPos.y / (viewportHeight * 0.5), 0.0, 0.1)); */
    /* imageStore(renderTarget, pixelCoord, vec4((14.0 - distance), (14.0 - distance), 0.0, 0.1)); */
    /* imageStore(renderTarget, pixelCoord, vec4(gl_LocalInvocationID.x/16.0, gl_LocalInvocationID.y/16.0, 0.0, 1.0)); */
}

float DistanceFromSphere(vec3 sphereCenter, float sphereRadius, vec3 point)
{
    return distance(sphereCenter, point) - sphereRadius;
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
