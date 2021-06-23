#version 410 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelTransformation;

void main()
{
    gl_Position = lightSpaceMatrix * modelTransformation * vec4(vertexPosition, 1.0);
}
