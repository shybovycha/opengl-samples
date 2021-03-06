#version 410

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} fsIn;

layout (location = 0) out vec3 fsPosition;
layout (location = 1) out vec3 fsNormal;
layout (location = 2) out vec4 fsAlbedo;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMapTexture;

void main()
{
    fsPosition = fsIn.fragmentPosition;

    fsNormal = texture(normalMapTexture, fsIn.textureCoord).rgb;

    if (length(fsNormal) == 0.0)
    {
        fsNormal = fsIn.normal;
    }

    fsAlbedo = texture(diffuseTexture, fsIn.textureCoord);
}
