#version 430

in GS_OUT
{
    vec4 vertexPosition;
    vec4 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} fsIn;

uniform sampler2D diffuseTexture;

void main()
{
    vec4 color = texture(diffuseTexture, fsIn.textureCoords);

    // TODO: add lighting component here
    gl_FragColor = color;
}
