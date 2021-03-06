#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec2 textureCoord;
    vec4 vertexColor;
} fsIn;

uniform sampler2D particleTexture;

void main()
{
    fragmentColor = texture(particleTexture, fsIn.textureCoord);
}
