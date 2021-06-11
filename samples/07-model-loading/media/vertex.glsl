#version 410

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 vertexNormal;

out vec2 outputUV;
out vec3 outputNormal;
out vec3 fragmentPosition;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    fragmentPosition = vec3(model * vec4(vertexPosition, 1.0));

    outputUV = vertexUV;
    outputNormal = vertexNormal;
}
