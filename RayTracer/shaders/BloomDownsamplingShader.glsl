#version 460 core

layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
//layout(binding = 0) uniform sampler2D inputTexture;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    float xScale = float(imageSize(inputImage).x) / float(imageSize(outputImage).x);
    float yScale = float(imageSize(inputImage).y) / float(imageSize(outputImage).y);
//    float xScale = float(textureSize(inputTexture, 0).x) / float(imageSize(outputImage).x);
//    float yScale = float(textureSize(inputTexture, 0).y) / float(imageSize(outputImage).y);


    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    vec4 a = imageLoad(inputImage, ivec2(xScale * texelCoord.x - 2, yScale * texelCoord.y + 2));
    vec4 b = imageLoad(inputImage, ivec2(xScale * texelCoord.x,     yScale * texelCoord.y + 2));
    vec4 c = imageLoad(inputImage, ivec2(xScale * texelCoord.x + 2, yScale * texelCoord.y + 2));

    vec4 d = imageLoad(inputImage, ivec2(xScale * texelCoord.x - 2, yScale * texelCoord.y    ));
    vec4 e = imageLoad(inputImage, ivec2(xScale * texelCoord.x,     yScale * texelCoord.y    ));
    vec4 f = imageLoad(inputImage, ivec2(xScale * texelCoord.x + 2, yScale * texelCoord.y    ));

    vec4 g = imageLoad(inputImage, ivec2(xScale * texelCoord.x - 2, yScale * texelCoord.y - 2));
    vec4 h = imageLoad(inputImage, ivec2(xScale * texelCoord.x,     yScale * texelCoord.y - 2));
    vec4 i = imageLoad(inputImage, ivec2(xScale * texelCoord.x + 2, yScale * texelCoord.y - 2));

    vec4 j = imageLoad(inputImage, ivec2(xScale * texelCoord.x - 1, yScale * texelCoord.y + 1));
    vec4 k = imageLoad(inputImage, ivec2(xScale * texelCoord.x + 1, yScale * texelCoord.y + 1));
    vec4 l = imageLoad(inputImage, ivec2(xScale * texelCoord.x - 1, yScale * texelCoord.y - 1));
    vec4 m = imageLoad(inputImage, ivec2(xScale * texelCoord.x + 1, yScale * texelCoord.y - 1));

//    vec4 a = texture(inputTexture, vec2(xScale * texelCoord.x - 2, yScale * texelCoord.y + 2));
//    vec4 b = texture(inputTexture, vec2(xScale * texelCoord.x,     yScale * texelCoord.y + 2));
//    vec4 c = texture(inputTexture, vec2(xScale * texelCoord.x + 2, yScale * texelCoord.y + 2));
//
//    vec4 d = texture(inputTexture, vec2(xScale * texelCoord.x - 2, yScale * texelCoord.y    ));
//    vec4 e = texture(inputTexture, vec2(xScale * texelCoord.x,     yScale * texelCoord.y    ));
//    vec4 f = texture(inputTexture, vec2(xScale * texelCoord.x + 2, yScale * texelCoord.y    ));
//
//    vec4 g = texture(inputTexture, vec2(xScale * texelCoord.x - 2, yScale * texelCoord.y - 2));
//    vec4 h = texture(inputTexture, vec2(xScale * texelCoord.x,     yScale * texelCoord.y - 2));
//    vec4 i = texture(inputTexture, vec2(xScale * texelCoord.x + 2, yScale * texelCoord.y - 2));
//
//    vec4 j = texture(inputTexture, vec2(xScale * texelCoord.x - 1, yScale * texelCoord.y + 1));
//    vec4 k = texture(inputTexture, vec2(xScale * texelCoord.x + 1, yScale * texelCoord.y + 1));
//    vec4 l = texture(inputTexture, vec2(xScale * texelCoord.x - 1, yScale * texelCoord.y - 1));
//    vec4 m = texture(inputTexture, vec2(xScale * texelCoord.x + 1, yScale * texelCoord.y - 1));
    
    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    vec4 downsample = e * 0.125;
    downsample += (a + c + g + i) * 0.03125;
    downsample += (b + d + f + h) * 0.0625;
    downsample += (j + k + l + m) * 0.125;

    imageStore(outputImage, texelCoord, vec4(vec3(downsample), 1.0));
}