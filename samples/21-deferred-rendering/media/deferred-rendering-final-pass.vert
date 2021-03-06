#version 430

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;

out VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    vsOut.fragmentPosition = vertexPosition;
    vsOut.normal = vertexNormal;
    vsOut.textureCoord = vec2(1.0 - vertexTextureCoord.x, vertexTextureCoord.y);

    gl_Position = vec4(vertexPosition, 1.0);
}
