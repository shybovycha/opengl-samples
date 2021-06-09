#version 410

uniform sampler2D textureSampler;

uniform vec3 lightPosition;
uniform vec3 lightColor;

in vec2 vertexUV;
in vec3 vertexNormal;
in vec3 fragmentPosition;

layout (location = 0) out vec4 fragColor;

void main()
{
    vec3 normal = normalize(vertexNormal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);

    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    vec3 diffuseColor = diffuseFactor * lightColor;

    fragColor = texture(textureSampler, vertexUV) * vec4(diffuseColor, 1.0);
}
