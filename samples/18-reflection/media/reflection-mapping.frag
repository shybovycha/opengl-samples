#version 430

in VS_OUT
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} fsIn;

uniform sampler2D diffuseTexture;

// uniform mat4 projectionViewMatrices[6];
// uniform mat4 modelPosition;

void main()
{
    gl_FragColor = texture(diffuseTexture, fsIn.textureCoords);
}
