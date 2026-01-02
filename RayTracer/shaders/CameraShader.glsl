#version 460 core

layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

layout(location = 0) uniform float exposure;
layout(location = 1) uniform float gamma;

layout(local_size_x = 16, local_size_y = 16) in;

const float PI = 3.141592653589793f;

vec4 GetPixel(vec3 luminance)
{
	vec3 linearValue = luminance * PI * exposure / 0.89;
	linearValue.r = linearValue.r > 1 ? 1 : linearValue.r;
	linearValue.g = linearValue.g > 1 ? 1 : linearValue.g;
	linearValue.b = linearValue.b > 1 ? 1 : linearValue.b;

	vec3 gammaCorrectedValue;
	gammaCorrectedValue.r = pow(linearValue.r, gamma);
	gammaCorrectedValue.g = pow(linearValue.g, gamma);
	gammaCorrectedValue.b = pow(linearValue.b, gamma);

	return vec4(gammaCorrectedValue, 1.0);
}

void main()
{
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

	vec3 luminance = vec3(imageLoad(inputImage, pixelCoord));

	vec4 value = GetPixel(luminance);
	imageStore(outputImage, pixelCoord, value);
}