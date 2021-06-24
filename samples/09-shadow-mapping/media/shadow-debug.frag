#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    // vec3 fragmentPosition;
    // vec3 normal;
    vec2 textureCoord;
    // vec4 fragmentPositionInLightSpace;
} fsIn;

uniform sampler2D shadowMap;
// uniform sampler2D diffuseTexture;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cameraPosition;

void main()
{
    float depth = texture(shadowMap, fsIn.textureCoord).r;
    fragmentColor = vec4(vec3(depth), 1.0);
}
