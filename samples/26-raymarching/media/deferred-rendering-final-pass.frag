#version 460

#extension GL_ARB_bindless_texture : require

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

layout(bindless_sampler) uniform sampler2D positionTexture;
layout(bindless_sampler) uniform sampler2D normalTexture;
layout(bindless_sampler) uniform sampler2D albedoTexture;

layout(bindless_sampler) uniform sampler2D lightSpaceCoord;
layout(bindless_sampler) uniform sampler2D shadowMap;

uniform vec3 cameraPosition;
uniform vec3 sunDirection;
uniform vec4 sunColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

float attenuation_constant = 1.0;
float attenuation_linear = 0.09;
float attenuation_quadratic = 0.032;

const float NB_STEPS = 30;
const float G_SCATTERING = 0.858;

#define M_PI 3.1415926535897932384626433832795

void main()
{
    vec3 fragmentPosition = texture(positionTexture, fsIn.textureCoord).xyz * 2 - 1;
    vec3 normal = texture(normalTexture, fsIn.textureCoord).rgb * 2 - 1;
    vec4 albedoColor = texture(albedoTexture, fsIn.textureCoord);

    vec3 viewDirection = normalize(cameraPosition - fragmentPosition);

    vec3 startPosition = cameraPosition;
    vec3 endRayPosition = fragmentPosition;

    vec3 rayVector = endRayPosition.xyz- startPosition;

    float rayLength = length(rayVector);
    vec3 rayDirection = normalize(rayVector);

    float stepLength = rayLength / NB_STEPS;

    vec3 raymarchingStep = rayDirection * stepLength;

    vec3 currentPosition = startPosition;

    vec4 accumFog = vec4(0.0);

    for (int i = 0; i < NB_STEPS; i++)
    {
        // basically perform shadow mapping
        vec4 worldInLightSpace = lightSpaceMatrix * vec4(currentPosition, 1.0f);
        worldInLightSpace /= worldInLightSpace.w;

        vec2 lightSpaceTextureCoord1 = (worldInLightSpace.xy * 0.5) + 0.5; // [-1..1] -> [0..1]
        float shadowMapValue1 = texture(shadowMap, lightSpaceTextureCoord1.xy).r;

        if (shadowMapValue1 > worldInLightSpace.z)
        {
            // Mie scaterring approximated with Henyey-Greenstein phase function
            float lightDotView = dot(normalize(rayDirection), normalize(-sunDirection));

            float scattering = 1.0 - G_SCATTERING * G_SCATTERING;
            scattering /= (4.0f * M_PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * lightDotView, 1.5f));

            accumFog += scattering * sunColor;
        }

        currentPosition += raymarchingStep;
    }

    accumFog /= NB_STEPS;

    // fade rays away
    // accumFog *= currentPosition / NB_STEPS);

    // lighting *= accumFog;

    vec4 fragmentPositionInLightSpace1 = lightSpaceMatrix * vec4(fragmentPosition, 1.0);
    fragmentPositionInLightSpace1 /= fragmentPositionInLightSpace1.w;

    vec2 shadowMapCoord1 = fragmentPositionInLightSpace1.xy * 0.5 + 0.5;

    // vec3 fragmentPositionInLightSpace2 = texture(lightSpaceCoord, fsIn.textureCoord).xyz;

    // vec2 shadowMapCoord2 = fragmentPositionInLightSpace2.xy;

    float thisDepth = fragmentPositionInLightSpace1.z;
    float occluderDepth = texture(shadowMap, shadowMapCoord1).r;

    float shadowFactor = 0.0;

    if (thisDepth > 0.0 && occluderDepth < 1.0 && thisDepth < occluderDepth)
    {
        shadowFactor = 1.0;
    }

    fragmentColor = ((albedoColor * shadowFactor * 0.3) + (accumFog * 0.7));
}
