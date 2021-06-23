#version 410 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelTransformation;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = lightSpaceMatrix * modelTransformation * vec4(vertexPosition, 1.0);
}
