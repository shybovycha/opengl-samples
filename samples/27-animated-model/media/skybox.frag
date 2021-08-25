#version 410

in VS_OUT
{
    vec3 textureCoords;
} fsIn;

out vec4 fragmentColor;

uniform samplerCube cubeMap;

void main() {
    fragmentColor = texture(cubeMap, fsIn.textureCoords);
}
