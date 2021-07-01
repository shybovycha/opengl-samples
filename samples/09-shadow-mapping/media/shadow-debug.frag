#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec2 textureCoord;
} fsIn;

uniform sampler2DArray shadowMaps;
uniform int textureLayer;

void main()
{
    float depth = texture(shadowMaps, vec3(fsIn.textureCoord, textureLayer)).r;

    fragmentColor = vec4(vec3(depth), 1.0);
}
