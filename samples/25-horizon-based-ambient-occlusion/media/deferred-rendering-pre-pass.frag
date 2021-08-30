#version 410

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    mat3 TBN;
} fsIn;

layout (location = 0) out vec3 fsPosition;
layout (location = 1) out vec3 fsNormal;
layout (location = 2) out vec4 fsAlbedo;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMapTexture;

void main()
{
    fsPosition = fsIn.fragmentPosition * 0.5 + 0.5;

    vec2 normalMapSize = textureSize(normalMapTexture, 0);

    if (normalMapSize.x == 0 || normalMapSize.y == 0)
    {
        fsNormal = fsIn.normal;
    } else
    {
        fsNormal = texture(normalMapTexture, fsIn.textureCoord).rgb;
        fsNormal = fsNormal * 2 - 1;
        fsNormal = fsIn.TBN * fsNormal;
    }

    fsNormal = normalize(fsNormal) * 0.5 + 0.5;

    fsAlbedo = texture(diffuseTexture, fsIn.textureCoord);
}
