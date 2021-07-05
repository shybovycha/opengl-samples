#version 410

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;

out VS_OUT
{
    vec3 viewPosition;
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 p = model * vec4(vertexPosition, 1.0);
    vsOut.fragmentPosition = vec3(p) / p.w;

    vec4 p1 = view * model * vec4(vertexPosition, 1.0);
    vsOut.viewPosition = vec3(p1) / p1.w;

    vsOut.normal = vertexNormal;
    vsOut.textureCoord = vertexTextureCoord;

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
