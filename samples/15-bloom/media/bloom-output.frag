#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} fsIn;

uniform sampler2D blurOutput;

void main()
{
    vec3 result = texture(blurOutput, fsIn.textureCoord).rgb;

    // TODO: gamma-correction, since output color might have its components greater than 1 due to blur

    fragmentColor = vec4(result, 1.0);
}
