#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec2 textureCoord;
} fsIn;

uniform sampler2D diffuseTexture;

vec4 msaa( sampler2D tex, vec2 uv ) {
    vec2 resolution = textureSize(tex, 0);

    vec4 singleSample = texture(tex, uv);

    float a = (3.0 / 8.0) * (1.0 / resolution.x);
    float b = (1.0 / 8.0) * (1.0 / resolution.y);

    vec4 acc = vec4(0.0);

    acc += texture(tex, (uv + vec2(-a, b)));
    acc += texture(tex, (uv + vec2(a, -b)));
    acc += texture(tex, (uv + vec2(-b, -a)));
    acc += texture(tex, (uv + vec2(b, a)));
    acc /= 4.0;

    vec4 color = pow(acc, vec4(1.0 / 2.2));

    return color;
}

void main()
{
    vec4 color = msaa(diffuseTexture, fsIn.textureCoord);

    fragmentColor = color;
}
