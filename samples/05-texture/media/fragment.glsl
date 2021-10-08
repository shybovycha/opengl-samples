#version 410

uniform sampler2D textureSampler;

in vec2 outputUV;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(textureSampler, outputUV);
}
