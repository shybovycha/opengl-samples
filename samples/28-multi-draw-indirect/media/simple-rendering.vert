#version 460

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;

out VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    flat uint objectID;
    flat uint instanceID;
} vsOut;

struct ObjectData
{
    vec2 albedoTextureSize;
    vec2 normalTextureSize;
    vec2 emissionTextureSize;
    uint instanceDataOffset; // use this field to get instance data: StaticObjectInstanceData.transformation[StaticObjectData.objectData[gl_DrawID].instanceDataOffset + gl_InstanceID]
};

layout (std430, binding = 4) buffer StaticObjectData
{
    ObjectData[] objectData;
};

layout (std430, binding = 5) buffer StaticObjectInstanceData
{
    mat4[] transformations;
};

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vsOut.fragmentPosition = vertexPosition;
    vsOut.normal = vertexNormal;
    vsOut.textureCoord = vec2(vertexTextureCoord.x, vertexTextureCoord.y);
    vsOut.objectID = gl_DrawID;
    vsOut.instanceID = gl_InstanceID;

    uint objectInstanceIndex = objectData[gl_DrawID].instanceDataOffset + gl_InstanceID;

    mat4 model = transformations[objectInstanceIndex];

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
