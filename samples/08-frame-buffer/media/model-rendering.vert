#version 410

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;

out vec3 fragmentPosition;
out vec3 normal;
out vec2 textureCoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    fragmentPosition = vec3(model * vec4(vertexPosition, 1.0));
    normal = vertexNormal;
    textureCoord = vertexTextureCoord;

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
