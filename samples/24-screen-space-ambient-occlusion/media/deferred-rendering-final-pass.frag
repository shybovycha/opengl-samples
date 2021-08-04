#version 430

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} fsIn;

layout (location = 0) out vec4 fragmentColor;

struct PointLight
{
    vec3 position;
    float strength;
    vec4 color;
    // float farPlane;
    // mat4 projectionViewMatrices[6];
};

layout (std430, binding = 5) buffer PointLightData
{
    PointLight pointLight[];
} pointLightData;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D albedoTexture;

uniform sampler2D ssaoNoiseTexture;
uniform sampler1D ssaoKernelTexture;

uniform vec3 cameraPosition;

uniform mat4 projection;

float attenuation_constant = 1.0;
float attenuation_linear = 0.09;
float attenuation_quadratic = 0.032;

void main()
{
    vec3 fragmentPosition = texture(positionTexture, fsIn.textureCoord).rgb;
    vec3 normal = texture(normalTexture, fsIn.textureCoord).rgb;
    vec4 albedoColor = texture(albedoTexture, fsIn.textureCoord);

    vec3 viewDirection = normalize(cameraPosition - fragmentPosition);

    vec4 lighting = albedoColor * 0.3;

    for (int i = 0; i < pointLightData.pointLight.length(); ++i)
    {
        PointLight light = pointLightData.pointLight[i];

        vec3 lightDirection = normalize(light.position - fragmentPosition);

        float lightDistance = length(light.position - fragmentPosition);
        float attenuation = 1.0 / (attenuation_constant + (attenuation_linear * lightDistance) + (attenuation_quadratic * lightDistance * lightDistance));

        vec4 diffuse = max(dot(normal, lightDirection), 0.0) * light.color;

        lighting += diffuse * attenuation;
    }

    // SSAO
    float radius = 0.1;
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
        vec3 randomNormal = normalize(texture(ssaoKernelTexture, i).xyz);

        // point randomNormal towards/inside the geometry
        if (dot(normal, randomNormal) < 0.0)
            randomNormal *= -1.0;

        // scale down the random offset to the subpixel level
        vec2 sampleOffset = randomNormal.xy * (radius / screenSize);

        float sampleDepth = texture(positionTexture, fsIn.textureCoord + sampleOffset).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragmentPosition.z - sampleDepth));

        occlusion += (sampleDepth >= fragmentPosition.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    fragmentColor = vec4(vec3(occlusion), 1.0);

    // fragmentColor = lighting;
}
