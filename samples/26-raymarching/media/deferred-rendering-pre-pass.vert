#version 410

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;
// layout (location = 3) in vec3 vertexTangent;
// layout (location = 4) in vec3 vertexBitangent;

out VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    vec3 lightSpaceCoord;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    vsOut.fragmentPosition = vec3(model * vec4(vertexPosition, 1.0));
    vsOut.normal = transpose(inverse(mat3(view * model))) * vertexNormal;
    vsOut.textureCoord = vertexTextureCoord;

    vec4 lightSpaceCoord = lightSpaceMatrix * model * vec4(vertexPosition, 1.0);
    vsOut.lightSpaceCoord = lightSpaceCoord.xyz / lightSpaceCoord.w;

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
