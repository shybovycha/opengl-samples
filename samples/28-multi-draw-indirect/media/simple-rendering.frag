#version 460

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    flat uint objectID;
    flat uint instanceID;
} fsIn;

struct ObjectData
{
    vec2 albedoTextureSize;
    vec2 normalTextureSize;
    vec2 emissionTextureSize;
    uint instanceDataOffset;
};

layout (std430, binding = 4) buffer StaticObjectData
{
    ObjectData[] objectData;
};

uniform sampler2DArray albedoTextures;
uniform sampler2DArray normalTextures;
uniform sampler2DArray emissionTextures;

void main()
{
    vec2 maxTextureSize = textureSize(albedoTextures, 0).xy;
    vec2 uvFactor = objectData[fsIn.objectID].albedoTextureSize / maxTextureSize;
    vec2 uv = vec2(fsIn.textureCoord.x, fsIn.textureCoord.y) * uvFactor;

    vec4 albedo = texture(albedoTextures, vec3(uv.x, uv.y, fsIn.objectID));

    fragmentColor = albedo;
}
