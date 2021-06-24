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

/*float shadowCalculation(vec4 fragmentPositionInLightSpace, vec3 normal, vec3 lightDirection)
{
    // perform perspective divide
    vec3 projectedCoords = fragmentPositionInLightSpace.xyz / fragmentPositionInLightSpace.w;

    // normalize
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projectedCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projectedCoords.z;

    // check whether current frag pos is in shadow; add bias to prevent mouray effect
    // float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    // float bias = 0.005;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    if (projectedCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}*/

float shadowCalculation()
{
    vec3 position = fsIn.fragmentPositionInLightSpace.xyz * 0.5 + 0.5;
    float depth = texture(shadowMap, position.xy).r;

    return depth < position.z ? 0.0 : 1.0;
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
    float shadow = shadowCalculation(); //fsIn.fragmentPositionInLightSpace, normal, lightDirection);

    vec3 lighting = ((shadow * (diffuse + specular)) + ambient) * color;

    fragmentColor = vec4(lighting, 1.0);
}
