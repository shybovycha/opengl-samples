#version 410

layout (location = 0) in vec3 vertexPosition;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(vertexPosition, 1.0);
}
