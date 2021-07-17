#version 430

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoords;

out VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 modelTransformation;

void main()
{
    vsOut.fragmentPosition = vec3(modelTransformation * vec4(vertexPosition, 1.0));
    vsOut.normal = vertexNormal;
    vsOut.textureCoords = vertexTextureCoords;

    gl_Position = modelTransformation * vec4(vertexPosition, 1.0);
}
