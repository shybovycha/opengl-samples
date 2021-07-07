#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec4 viewPosition;
    vec4 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} fsIn;

uniform mat4 lightViewProjections[4];
uniform float splits[4];

uniform sampler2DArray shadowMaps;
uniform sampler2D diffuseTexture;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cameraPosition;

float shadowCalculation(vec3 normal, vec3 lightDirection)
{
    float cameraViewDepth = fsIn.viewPosition.z;

    for (int i = 0; i < 4; ++i)
    {
        if (cameraViewDepth < splits[i])
        {
            vec4 fragmentPositionInLightSpace = lightViewProjections[i] * fsIn.fragmentPosition;
            vec3 shadowPos1 = fragmentPositionInLightSpace.xyz / fragmentPositionInLightSpace.w;
            vec3 shadowMapCoord = shadowPos1 * 0.5 + 0.5;
            float thisDepth = shadowMapCoord.z;

            if (thisDepth > 1.0)
            {
                continue;
            }

            float occluderDepth = texture(shadowMaps, vec3(shadowMapCoord.xy, i)).r;

            float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

            return thisDepth - bias > occluderDepth ? 0.25 : 1.0;
        }
    }

    return 0.0;
}

void main()
{
    vec3 color = texture(diffuseTexture, fsIn.textureCoord).rgb;
    vec3 normal = normalize(fsIn.normal);

    // ambient
    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 lightDirection = normalize(lightPosition - vec3(fsIn.fragmentPosition) / fsIn.fragmentPosition.w);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDirection = normalize(cameraPosition - vec3(fsIn.fragmentPosition) / fsIn.fragmentPosition.w);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDirection), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // calculate shadow
    float shadow = shadowCalculation(normal, lightDirection);

    vec3 lighting = ((shadow * (diffuse + specular)) + ambient) * color;

    fragmentColor = vec4(lighting, 1.0);
}
