#version 460 core

layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

uniform float filterRadius;

layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	float xScale = float(imageSize(inputImage).x) / float(imageSize(outputImage).x);
    float yScale = float(imageSize(inputImage).y) / float(imageSize(outputImage).y);

	// Take 9 samples around current texel:
	// a - b - c
	// d - e - f
	// g - h - i
	// === ('e' is the current texel) ===
	vec4 a = imageLoad(inputImage, ivec2(xScale * texelCoord.x - filterRadius, yScale * texelCoord.y + filterRadius));
	vec4 b = imageLoad(inputImage, ivec2(xScale * texelCoord.x,				   yScale * texelCoord.y + filterRadius));
	vec4 c = imageLoad(inputImage, ivec2(xScale * texelCoord.x + filterRadius, yScale * texelCoord.y + filterRadius));

	vec4 d = imageLoad(inputImage, ivec2(xScale * texelCoord.x - filterRadius, yScale * texelCoord.y			   ));
	vec4 e = imageLoad(inputImage, ivec2(xScale * texelCoord.x,				   yScale * texelCoord.y			   ));
	vec4 f = imageLoad(inputImage, ivec2(xScale * texelCoord.x + filterRadius, yScale * texelCoord.y			   ));

	vec4 g = imageLoad(inputImage, ivec2(xScale * texelCoord.x - filterRadius, yScale * texelCoord.y - filterRadius));
	vec4 h = imageLoad(inputImage, ivec2(xScale * texelCoord.x,				   yScale * texelCoord.y - filterRadius));
	vec4 i = imageLoad(inputImage, ivec2(xScale * texelCoord.x + filterRadius, yScale * texelCoord.y - filterRadius));

	// Apply weighted distribution, by using a 3x3 tent filter:
	//  1   | 1 2 1 |
	// -- * | 2 4 2 |
	// 16   | 1 2 1 |
	vec4 upsample = e * 4.0;
	upsample += (b + d + f + h) * 2.0;
	upsample += (a + c + g + i);
	upsample *= 1.0 / 16.0;

    imageStore(outputImage, texelCoord, vec4(vec3(upsample), 1.0));
}