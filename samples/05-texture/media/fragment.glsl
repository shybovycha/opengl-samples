#version 410

uniform sampler2D textureSampler;

in vec2 uv;

out vec4 fragColor;

void main()
{
    fragColor = texture(textureSampler, uv);
}
