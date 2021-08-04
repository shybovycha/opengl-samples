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
    vec2 screenSize = textureSize(positionTexture, 0);
    float radius = 0.5;
    float bias = 0.05;

    vec2 noiseScale = screenSize / textureSize(ssaoNoiseTexture, 0);
    float kernelSize = textureSize(ssaoKernelTexture, 0);

    vec3 fragPosition = texture(positionTexture, fsIn.textureCoord).xyz;
    vec3 normal = texture(normalTexture, fsIn.textureCoord).rgb;
    vec3 randomVec = texture(ssaoNoiseTexture, fsIn.textureCoord * noiseScale).xyz;
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for (int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 uniformSamplePosition = texture(ssaoKernelTexture, float(i)).xyz;

        vec3 samplePosition = TBN * uniformSamplePosition; // from tangent to view-space

        samplePosition = fragPosition + samplePosition * radius;

        vec4 offset = vec4(samplePosition, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range [0.0; 1.0]

        float sampleDepth = texture(positionTexture, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPosition.z - sampleDepth));

        occlusion += (sampleDepth >= samplePosition.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    fragmentColor = vec4(vec3(occlusion), 1.0);
}
