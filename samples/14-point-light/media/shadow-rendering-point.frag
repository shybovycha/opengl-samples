#version 430

layout (location = 0) out vec4 fragmentColor;

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} fsIn;

uniform samplerCube shadowMap;
uniform sampler2D diffuseTexture;
uniform sampler2D specularMapTexture;
uniform sampler2D emissionMapTexture;

// uniform vec3 lightPosition;
// uniform float farPlane;

struct PointLight
{
    vec3 lightPosition;
    float farPlane;
    mat4 projectionViewMatrices[6];
};

layout (std430, binding = 1) buffer pointLightData
{
    PointLight pointLight;
};

uniform vec3 lightColor;
uniform vec3 cameraPosition;

// TODO: make these params uniforms
// uniform vec3 ambientColor;
// uniform vec3 diffuseColor;
// uniform float materialSpecular;
uniform vec3 emissionColor;

float attenuation_constant = 1.0;
float attenuation_linear = 0.09;
float attenuation_quadratic = 0.032;

float shadowCalculation(vec3 normal)
{
    vec3 shadowMapCoord = fsIn.fragmentPosition - pointLight.lightPosition;
    float occluderDepth = texture(shadowMap, shadowMapCoord).r * pointLight.farPlane;
    float thisDepth = shadowMapCoord.z;

    if (thisDepth > 1.0)
    {
        return 0.0;
    }

    float bias = max(0.05 * (1.0 - dot(normal, shadowMapCoord)), 0.005);

    // PCF
    /*float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, shadowMapCoord.xy + vec2(x, y) * texelSize).r;

            shadow += thisDepth - bias < pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;*/

    float shadow = (thisDepth - bias) < length(shadowMapCoord) ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, fsIn.textureCoords).rgb;
    vec3 normal = normalize(fsIn.normal);

    // ambient
    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 lightDirection = normalize(pointLight.lightPosition - fsIn.fragmentPosition);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDirection = normalize(cameraPosition - fsIn.fragmentPosition);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDirection), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // attenuation
    float lightRadius = 10.0;
    float distance = length(pointLight.lightPosition - fsIn.fragmentPosition) / lightRadius;
    float attenuation = 1.0 / (attenuation_constant + attenuation_linear * distance + attenuation_quadratic * (distance * distance));

    // calculate shadow; this represents a global directional light, like Sun
    float shadow = shadowCalculation(normal);

    // these are the multipliers from different light maps (read from corresponding textures)
    float specularCoefficient = texture(specularMapTexture, fsIn.textureCoords).r;
    float emissionCoefficient = texture(emissionMapTexture, fsIn.textureCoords).r;

    // vec3 lighting = ((shadow * ((diffuse * attenuation) + (specular * specularCoefficient * attenuation))) + (ambient * attenuation)) * color + (emissionColor * emissionCoefficient);
    vec3 lighting = ((shadow * ((diffuse) + (specular * specularCoefficient))) + (ambient)) * color + (emissionColor * emissionCoefficient);

    fragmentColor = vec4(lighting, 1.0);
}
