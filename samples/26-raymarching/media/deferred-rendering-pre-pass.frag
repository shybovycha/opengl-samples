#version 410

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    vec3 lightSpaceCoord;
} fsIn;

layout (location = 0) out vec3 fsPosition;
layout (location = 1) out vec3 fsNormal;
layout (location = 2) out vec4 fsAlbedo;
layout (location = 3) out vec3 fsLightSpaceCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMapTexture;

void main()
{
    fsPosition = fsIn.fragmentPosition * 0.5 + 0.5;

    fsNormal = texture(normalMapTexture, fsIn.textureCoord).rgb;

    if (length(fsNormal) == 0.0)
    {
        fsNormal = fsIn.normal;
    }

    fsNormal = fsNormal.xyz * 0.5 + 0.5;

    fsAlbedo = texture(diffuseTexture, fsIn.textureCoord);

    fsLightSpaceCoord = fsIn.lightSpaceCoord.xyz * 0.5 + 0.5;
}
