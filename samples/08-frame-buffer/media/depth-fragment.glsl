#version 410

uniform vec3 cameraPosition;

in vec4 vertexColor;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = vertexColor;
}
