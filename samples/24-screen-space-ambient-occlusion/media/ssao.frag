#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec2 textureCoord;
} fsIn;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

uniform sampler2D ssaoNoiseTexture;
uniform sampler1D ssaoKernelTexture;

uniform mat4 projection;

void main()
{
    vec3 fragmentPosition = texture(positionTexture, fsIn.textureCoord).xyz;
    vec3 normal = texture(normalTexture, fsIn.textureCoord).rgb;

    float radius = 0.5;
    float bias = 0.025;

    vec2 screenSize = textureSize(positionTexture, 0);
    vec2 noiseSize = textureSize(ssaoNoiseTexture, 0);
    float kernelSize = textureSize(ssaoKernelTexture, 0);

    vec2 noiseScale = screenSize / noiseSize;

    vec3 randomVec = normalize(texture(ssaoNoiseTexture, fsIn.textureCoord * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for (int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePosition = TBN * texture(ssaoKernelTexture, i).xyz;

        if (dot(samplePosition, normal) < 0.0)
            samplePosition *= -1.0;

        samplePosition = fragmentPosition + samplePosition * radius;

        vec4 offsetUV = projection * vec4(samplePosition, 1.0);
        offsetUV.xyz /= offsetUV.w;
        offsetUV.xy = offsetUV.xy * 0.5 + 0.5;

        vec4 offsetPosition = texture(positionTexture, offsetUV.xy);

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragmentPosition.z - offsetPosition.z));

        occlusion += (samplePosition.z >= offsetPosition.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    fragmentColor = vec4(vec3(occlusion), 1.0);
}
