#version 410

layout (location = 0) out vec4 fragmentColor;

uniform vec4 color;

void main()
{
  fragmentColor = color;
}
