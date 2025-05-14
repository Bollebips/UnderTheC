#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform image2D renderTarget;

void main()
{
    ivec2 pixelCoord = ivec2(gl_LocalInvocationID.xy);

    imageStore(renderTarget, vec4(1.0, 0.0, 0.0, 1.0));
}
