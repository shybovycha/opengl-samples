#version 430

layout (location = 0) out vec4 fragmentColor;

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} fsIn;

uniform samplerCube reflectionMap;
uniform sampler2D reflectionMapTexture;
uniform sampler2D diffuseTexture;

uniform vec3 cameraPosition;

uniform mat4 model;

void main()
{
    vec3 reflectedDirection = reflect(normalize(fsIn.fragmentPosition - cameraPosition), fsIn.normal);
    vec4 reflectionColor = texture(reflectionMap, reflectedDirection);

    vec4 albedoColor = texture(diffuseTexture, fsIn.textureCoords);
    vec4 reflectionFactor = texture(reflectionMapTexture, fsIn.textureCoords);

    fragmentColor = mix(albedoColor, reflectionColor, reflectionFactor.r);
}
