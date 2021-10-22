#version 460

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    flat uint objectID;
} fsIn;

struct ObjectData
{
    mat4 transform;
    vec4 color;
};

layout (std430, binding = 4) buffer StaticObjectData
{
    ObjectData[] objectData;
};

void main()
{
    vec4 color = objectData[fsIn.objectID].color;

    fragmentColor = color;
}
