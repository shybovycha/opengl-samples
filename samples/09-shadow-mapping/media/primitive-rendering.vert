#version 410

layout (location = 0) in vec3 vertexPosition;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 transformation;

void main()
{
    gl_Position = transformation * vec4(vertexPosition, 1.0);
}
