#version 430

in vec4 fragmentPosition;

// uniform vec3 lightPosition;
// uniform float farPlane;

struct PointLight
{
    vec3 lightPosition;
    float farPlane;
    mat4 projectionViewMatrices[6];
};

layout (std430, binding = 1) buffer pointLightData
{
    PointLight pointLight;
};

void main()
{
    float distance = length(fragmentPosition.xyz - pointLight.lightPosition);

    distance /= pointLight.farPlane;

    gl_FragDepth = distance;
}
