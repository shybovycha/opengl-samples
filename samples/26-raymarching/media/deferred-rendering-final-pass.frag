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

uniform sampler2D lightSpaceCoord;
uniform sampler2D shadowMap;

uniform vec3 cameraPosition;
uniform vec3 sunDirection;
uniform vec4 sunColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

float attenuation_constant = 1.0;
float attenuation_linear = 0.09;
float attenuation_quadratic = 0.032;

const float NB_STEPS = 10;
const float G_SCATTERING = 0.4;

#define M_PI 3.1415926535897932384626433832795

// volumetric light (aka light scattering) using raymarching
vec4 raymarchLightScattering()
{
    vec3 worldPos = texture(positionTexture, fsIn.textureCoord).xyz * 2 - 1;
    vec3 startPosition = cameraPosition;
    vec3 endRayPosition = worldPos;

    vec3 rayVector = endRayPosition.xyz- startPosition;

    float rayLength = length(rayVector);
    vec3 rayDirection = rayVector / rayLength;

    float stepLength = rayLength / NB_STEPS;

    vec3 raymarchingStep = rayDirection * stepLength;

    vec3 currentPosition = startPosition;

    vec4 accumFog = vec4(0.0);

    for (int i = 0; i < NB_STEPS; i++)
    {
        // effectively perform shadow mapping
        vec4 worldInLightSpace = lightSpaceMatrix * vec4(currentPosition, 1.0f);
        worldInLightSpace /= worldInLightSpace.w;

        vec3 lightSpaceTextureCoord = texture(lightSpaceCoord, fsIn.textureCoord).xyz;
        float shadowMapValue = texture(shadowMap, lightSpaceTextureCoord.xy).r;

        if (shadowMapValue > worldInLightSpace.z)
        {
            // Mie scaterring approximated with Henyey-Greenstein phase function
            float lightDotView = dot(rayDirection, sunDirection);
            float scattering = 1.0f - G_SCATTERING * G_SCATTERING;

            scattering /= (4.0f * M_PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * lightDotView, 1.5f));

            accumFog += scattering * sunColor;
        }

        currentPosition += raymarchingStep;
    }

    accumFog /= NB_STEPS;

    return accumFog;
}

void main()
{
    vec3 fragmentPosition = texture(positionTexture, fsIn.textureCoord).xyz * 2 - 1;
    vec3 normal = texture(normalTexture, fsIn.textureCoord).rgb * 2 - 1;
    vec4 albedoColor = texture(albedoTexture, fsIn.textureCoord);

    vec3 viewDirection = normalize(cameraPosition - fragmentPosition);

    vec4 lighting = albedoColor * 0.3;

    for (int i = 0; i < pointLightData.pointLight.length(); ++i)
    {
        PointLight light = pointLightData.pointLight[i];

        vec3 lightDirection = normalize(light.position - fragmentPosition);

        float lightDistance = length(light.position - fragmentPosition);
        float attenuation = 1.0 / (attenuation_constant + (attenuation_linear * lightDistance) + (attenuation_quadratic * lightDistance * lightDistance));

        vec4 diffuse = max(dot(normal, lightDirection), 0.0) * albedoColor * light.color;

        lighting += diffuse * attenuation;
    }

        vec4 worldInLightSpace = lightSpaceMatrix * vec4(fragmentPosition, 1.0f);
        worldInLightSpace /= worldInLightSpace.w;

        vec3 lightSpaceTextureCoord = texture(lightSpaceCoord, fsIn.textureCoord).xyz;
        vec4 shadowMapValue = texture(shadowMap, lightSpaceTextureCoord.xy);

    lighting = vec4(texture(lightSpaceCoord, worldInLightSpace.xy * 0.5 + 0.5).z); // raymarchLightScattering();

    fragmentColor = lighting;
}
