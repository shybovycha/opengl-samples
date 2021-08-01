#version 410

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} fsIn;

layout (location = 0) out vec4 fragmentColor;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D albedoTexture;

void main()
{
    fragmentColor = texture(albedoTexture, fsIn.textureCoord);
}
