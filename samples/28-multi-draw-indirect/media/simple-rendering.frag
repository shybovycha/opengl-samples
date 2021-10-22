#version 430

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
    flat uint objectID;
} fsIn;

//struct ObjectData
//{
//    mat4 transform;
//    vec4 color;
//    // uint[] diffuseTextures;
//};
//
//layout (std430, binding = 4) buffer StaticObjectData
//{
//    ObjectData[] objectData;
//};

// uniform sampler2D diffuseTexture;

void main()
{
    // vec4 color = texture(diffuseTexture, fsIn.textureCoord);

    /*vec3 normal = normalize(fsIn.normal);

    // ambient
    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 lightDirection = normalize(lightPosition - fsIn.fragmentPosition);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDirection = normalize(cameraPosition - fsIn.fragmentPosition);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDirection), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = ((diffuse + specular) + ambient) * color;*/

    float r = (float(fsIn.objectID)) / 3.0;

    fragmentColor = vec4(r, 0.0, 0.4, 1.0); // objectData[fsIn.objectID].color; // color;
}
