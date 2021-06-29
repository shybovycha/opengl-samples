#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec2 textureCoord;
    vec3 vertexPosition;
} fsIn;

uniform sampler2D particleTexture;

void main()
{
    fragmentColor = vec4(1.0); // texture(particleTexture, fsIn.textureCoord)
}
