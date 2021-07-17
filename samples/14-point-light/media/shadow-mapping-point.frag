#version 430

in vec4 fragmentPosition;

// out float fragmentDepth;

// uniform vec3 lightPosition;
// uniform float farPlane;

//struct PointLight
//{
    uniform vec3 lightPosition;
    uniform float farPlane;
//    mat4 projectionViewMatrices[6];
//};

//layout (std430, binding = 5) buffer pointLightData
//{
//    PointLight pointLight;
//};

void main()
{
    float distance = length(fragmentPosition.xyz - lightPosition);

    distance /= farPlane;

    gl_FragDepth = distance;
}
