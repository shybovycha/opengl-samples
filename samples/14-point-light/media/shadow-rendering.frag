#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    vec4 fragmentPositionInLightSpace;
} fsIn;

uniform sampler2D shadowMap;
uniform sampler2D diffuseTexture;
uniform sampler2D specularMapTexture;
uniform sampler2D emissionMapTexture;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cameraPosition;

// TODO: make these params uniforms
// uniform vec3 ambientColor;
// uniform vec3 diffuseColor;
// uniform float materialSpecular;
uniform vec3 emissionColor; // = vec3(0.807, 0.671, 0.175);

float attenuation_constant = 1.0;
float attenuation_linear = 0.09;
float attenuation_quadratic = 0.032;

float shadowCalculation(vec3 normal, vec3 lightDirection)
{
    vec3 shadowMapCoord = (fsIn.fragmentPositionInLightSpace.xyz / fsIn.fragmentPositionInLightSpace.w) * 0.5 + 0.5;
    float occluderDepth = texture(shadowMap, shadowMapCoord.xy).r;
    float thisDepth = shadowMapCoord.z;

    if (thisDepth > 1.0)
    {
        return 0.0;
    }

    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, shadowMapCoord.xy + vec2(x, y) * texelSize).r;

            shadow += thisDepth - bias < pcfDepth  ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, fsIn.textureCoord).rgb;
    vec3 normal = normalize(fsIn.normal);

    // ambient
    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 lightDirection = normalize(lightPosition - fsIn.fragmentPosition);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDirection = normalize(cameraPosition - fsIn.fragmentPosition);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDirection), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // attenuation
    float distance = length(lightPosition - fsIn.fragmentPosition);
    float attenuation = 1.0 / (attenuation_constant + attenuation_linear * distance + attenuation_quadratic * (distance * distance));

    // calculate shadow; this represents a global directional light, like Sun
    float shadow = shadowCalculation(normal, lightDirection);

    // these are the multipliers from different light maps (read from corresponding textures)
    float specularCoefficient = texture(specularMapTexture, fsIn.textureCoord).r;
    float emissionCoefficient = texture(emissionMapTexture, fsIn.textureCoord).r;

    vec3 lighting = ((shadow * ((diffuse * attenuation) + (specular * specularCoefficient * attenuation))) + (ambient * attenuation)) * color + (emissionColor * emissionCoefficient);

    fragmentColor = vec4(lighting, 1.0);
}
