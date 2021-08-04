#version 410

uniform sampler2D textureSampler;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform float materialSpecular;

uniform vec3 cameraPosition;

in vec2 vertexUV;
in vec3 vertexNormal;
in vec3 fragmentPosition;
in vec4 vertexColor;

layout (location = 0) out vec4 fragColor;

void main()
{
    /*vec3 normal = normalize(vertexNormal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);

    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    vec3 diffuseColor = diffuseFactor * lightColor;

    vec3 viewerDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normal);

    float specularFactor = pow(max(dot(viewerDirection, reflectionDirection), 0.0), 32);
    vec3 specularColor = materialSpecular * specularFactor * lightColor;

    fragColor = texture(textureSampler, vertexUV) * (vec4(ambientColor, 1.0) + vec4(specularColor, 1.0) + vec4(diffuseColor, 1.0));*/

    float depthValue = texture(textureSampler, vertexUV).r;
    fragColor = vec4(vec3(depthValue), 1.0);
}
