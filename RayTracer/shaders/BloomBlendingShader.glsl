#version 460 core

layout(rgba32f, binding = 0) uniform readonly image2D source;
layout(rgba32f, binding = 1) uniform readonly image2D bloom;
layout(rgba32f, binding = 2) uniform writeonly image2D outputImage;

layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	vec3 sourcePixel = vec3(imageLoad(source, texelCoord));
	vec3 value = sourcePixel + vec3(imageLoad(bloom, texelCoord)) / 2.0;
	value = max(value, sourcePixel);

    imageStore(outputImage, texelCoord, vec4(value, 1.0));
}