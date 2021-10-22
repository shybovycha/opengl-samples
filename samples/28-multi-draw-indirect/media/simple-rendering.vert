#version 430

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;
layout (location = 3) in  uint objectID;

out VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    flat uint objectID;
} vsOut;

struct ObjectData
{
    mat4 transform;
    vec4 color;
    // uint[] diffuseTextures;
};

layout (std430, binding = 4) buffer StaticObjectData
{
    ObjectData[] objectData;
};

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vsOut.fragmentPosition = vertexPosition;
    vsOut.normal = vertexNormal;
    vsOut.textureCoord = vec2(1.0 - vertexTextureCoord.x, vertexTextureCoord.y);
    vsOut.objectID = objectID;

    // mat4 model = objectData[objectID].transform;

    gl_Position = projection * view * vec4(vertexPosition, 1.0);
}
