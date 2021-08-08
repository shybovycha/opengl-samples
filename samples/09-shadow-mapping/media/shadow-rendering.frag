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
uniform vec3 cameraPosition;

float shadowCalculation1(vec3 normal, vec3 lightDirection)
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

float linearizeDepth(float depth)
{
    float nearPlane = 1.0;
    float farPlane = 100.0;

    return (2.0 * nearPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
}

float linstep(float _min, float _max, float v)
{
    return clamp((v - _min) / (_max - _min), 0, 1);
}

float reduceLightBleeding(float p_max, float Amount)
{
    // Remove the [0, Amount] tail and linearly rescale (Amount, 1].
    return linstep(Amount, 1, p_max);
}

// compute an upper bound on the probability that the currently shaded surface (at depth t) is occluded
float ChebyshevUpperBound(vec2 moments, float t)
{
    // One-tailed inequality valid if t > Moments.x
    if (t <= moments.x)
    {
        return 1.0;
    }

    // Compute variance
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.001);

    // Compute probabilistic upper bound
    float d = t - moments.x;
    return variance / (variance + d * d);
}

float shadowCalculation(vec3 normal, vec3 lightDirection)
{
    vec3 shadowMapCoord = (fsIn.fragmentPositionInLightSpace.xyz / fsIn.fragmentPositionInLightSpace.w) * 0.5 + 0.5;
    vec3 shadowMapSample = texture(shadowMap, shadowMapCoord.xy).rgb;
    vec2 moments = shadowMapSample.xy;
    float distanceToLight = shadowMapCoord.z; //linearizeDepth(shadowMapCoord.z);

    // Compute the Chebyshev upper bound.
    float p_max = ChebyshevUpperBound(moments, distanceToLight);

    return reduceLightBleeding(p_max, 0.5);
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
    float shadow = shadowCalculation(normal, lightDirection);

    vec3 lighting = ((shadow * (diffuse + specular)) + ambient) * color;

    fragmentColor = vec4(lighting, 1.0);
}
