#version 410

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;

out VS_OUT
{
    vec2 textureCoord;
    vec3 vertexPosition;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 transformationMatrix;

void main()
{
    vsOut.textureCoord = vertexTextureCoord;
    vsOut.vertexPosition = vertexPosition;

    gl_Position = transformationMatrix * vec4(vertexPosition, 1.0);
}
