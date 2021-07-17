#version 410

in VS_OUT
{
    vec3 textureCoords;
} fsIn;

out vec4 fragmentColor;

uniform samplerCube cubeMap;

void main() {
    fragmentColor = vec4(1.0, 1.0, 1.0, 1.0); //texture(cubeMap, fsIn.textureCoords);
}
