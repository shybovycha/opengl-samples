#version 430

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;

out VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 cameraPosition;

void main()
{
    vsOut.fragmentPosition = vec3(model * vec4(vertexPosition, 1.0));
    vsOut.normal = mat3(transpose(inverse(model))) * vertexNormal;
    vsOut.textureCoords = vertexTextureCoord;

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
