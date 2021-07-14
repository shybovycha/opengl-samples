#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoord;
} fsIn;

uniform sampler2D blurInput;

uniform bool isHorizontalBlur;

float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
    vec2 textureOffset = 1.0 / textureSize(blurInput, 0);
    vec3 result = texture(blurInput, fsIn.textureCoord).rgb * weight[0];

    if (isHorizontalBlur) {
        for (int i = 1; i < 5; ++i) {
            result += texture(blurInput, fsIn.textureCoord + vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(blurInput, fsIn.textureCoord - vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for (int i = 1; i < 5; ++i) {
            result += texture(blurInput, fsIn.textureCoord + vec2(0.0, textureOffset.y * i)).rgb * weight[i];
            result += texture(blurInput, fsIn.textureCoord - vec2(0.0, textureOffset.y * i)).rgb * weight[i];
        }
    }

    fragmentColor = vec4(result, 1.0);
}
