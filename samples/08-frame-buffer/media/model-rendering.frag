#version 410

layout (location = 0) out vec4 fragmentColor;

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoord;

uniform sampler2D diffuseTexture;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform float materialSpecular;
uniform vec3 cameraPosition;

void main()
{
    vec3 color = texture(diffuseTexture, textureCoord).rgb;
    vec3 normal = normalize(normal);

    // diffuse
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDirection = normalize(cameraPosition - fragmentPosition);
    float spec = materialSpecular;
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    spec = pow(max(dot(normal, halfwayDirection), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = (ambientColor + diffuse + specular) * color;

    fragmentColor = vec4(lighting, 1.0);
}
