#version 430

layout (location = 0) in vec3 vertexPosition;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 modelTransformation;

void main()
{
    gl_Position = modelTransformation * vec4(vertexPosition, 1.0);
}
