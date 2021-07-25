#version 410

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexUV;

out VS_OUT
{
    vec3 textureCoords;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * vec4(vertexPosition, 1.0);
    gl_Position = pos.xyww;
    vsOut.textureCoords = vertexPosition;
}
