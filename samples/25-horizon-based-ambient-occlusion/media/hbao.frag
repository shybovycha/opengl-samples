#version 410

layout (location = 0) out vec4 fragmentColor;

in VS_OUT {
    vec3 fragmentPosition;
    vec2 textureCoord;
} fsIn;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

uniform sampler2D hbaoNoiseTexture;
uniform sampler1D hbaoKernelTexture;

uniform mat4 projection;

const float PI = 3.14;
const float NUM_SAMPLE_DIRECTIONS = 10.0;
const float NUM_SAMPLE_STEPS = 10.0;
const float INTENSITY = 2.0;

const float radius = 30;
const float bias = 0.5;

void main()
{
    vec3 fragmentPosition = texture(positionTexture, fsIn.textureCoord).xyz;
    vec3 normal = texture(normalTexture, fsIn.textureCoord).rgb;

    vec2 screenSize = textureSize(positionTexture, 0);
    vec2 noiseSize = textureSize(hbaoNoiseTexture, 0);

    vec2 noiseScale = screenSize / noiseSize;

    const float stepPixels = radius / (NUM_SAMPLE_STEPS + 1);
    const float alpha = 2 * PI / float(NUM_SAMPLE_DIRECTIONS);

    float occlusion = 0.0;

    for (float i = 0; i < NUM_SAMPLE_DIRECTIONS; ++i)
    {
        float angle = alpha * i;

        vec4 random = texture(hbaoNoiseTexture, fsIn.textureCoord * noiseScale);

        // vec2 dir = vec2(cos(angle), sin(angle));
        // vec2 cosSin = random.xy;
        // vec2 direction = vec2(dir.x * cosSin.x - dir.y * cosSin.y, dir.x * cosSin.y + dir.y * cosSin.x);
        // vec2 direction = rotateDirection(vec2(cos(angle), sin(angle)), random.xy);

        vec2 direction = vec2(cos(angle) * random.x - sin(angle) * random.y, cos(angle) * random.y + sin(angle) * random.x);

        float rayPixels = random.z * (stepPixels + 1.0);

        for (float t = 0; t < NUM_SAMPLE_STEPS; ++t)
        {
            vec2 sampleUV = round(rayPixels * direction) * (1.0 / screenSize) + fsIn.textureCoord;
            vec3 samplePosition = texture(positionTexture, sampleUV).xyz;

            rayPixels += stepPixels;

            vec3 sampleDirection = samplePosition - fragmentPosition;
            float v1 = dot(sampleDirection, sampleDirection);
            float v2 = dot(normal, sampleDirection) * 1.0 / sqrt(v1);
            occlusion += clamp(v2 - bias, 0.0, 1.0) * clamp(v1 * (-1.0 / (radius * radius)) + 1.0, 0.0, 1.0);
        }
    }

    occlusion *= INTENSITY / (NUM_SAMPLE_DIRECTIONS * NUM_SAMPLE_STEPS);
    occlusion = clamp(occlusion, 0.0, 1.0);

    fragmentColor = vec4(vec3(occlusion), 1.0);
}
