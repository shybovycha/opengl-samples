#version 430

in vec4 fragmentPosition;

struct PointLight
{
    vec3 lightPosition;
    float farPlane;
    mat4 projectionViewMatrices[6];
};

layout (std430, binding = 5) buffer pointLightData
{
    PointLight pointLight;
};

void main()
{
    float distance = length(fragmentPosition.xyz - pointLight.lightPosition) / pointLight.farPlane;

    gl_FragDepth = distance;
}
