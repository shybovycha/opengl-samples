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

uniform vec3 lightPosition;
uniform vec3 lightColor;
// uniform vec3 ambientColor;
// uniform vec3 diffuseColor;
// uniform float materialSpecular;
uniform vec3 cameraPosition;

float shadowCalculation()
{
    vec2 shadowMapCoord = fsIn.fragmentPositionInLightSpace.xy * 0.5 + 0.5;
    float occluderDepth = texture(shadowMap, shadowMapCoord).r;

    float thisDepth = fsIn.fragmentPositionInLightSpace.z * 0.5 + 0.5;

    return occluderDepth < thisDepth ? 1.0 : 0.0;
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

    // calculate shadow
    float shadow = shadowCalculation();

    vec3 lighting = ((shadow * (diffuse + specular)) + ambient) * color;

    fragmentColor = vec4(lighting, 1.0);
}
