#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec2 textureCoord;
} fsIn;

uniform sampler2D shadowMap;

void main()
{
    float depth = texture(shadowMap, fsIn.textureCoord).r;

    fragmentColor = vec4(vec3(depth), 1.0);
}
