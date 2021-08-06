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
const float INTENSITY = 3.0;

const float radius = 0.15;
const float bias = 0.025;

void main()
{
    vec3 fragmentPosition = texture(positionTexture, fsIn.textureCoord).xyz;
    vec3 normal = texture(normalTexture, fsIn.textureCoord).rgb;

    vec2 screenSize = textureSize(positionTexture, 0);
    vec2 noiseSize = textureSize(hbaoNoiseTexture, 0);

    vec2 noiseScale = screenSize / noiseSize;

    const float theta = 2 * PI / float(NUM_SAMPLE_DIRECTIONS);
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    mat2 deltaRotationMatrix = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
    vec2 deltaUV = vec2(1.0, 0.0) * (radius / (float(NUM_SAMPLE_DIRECTIONS * NUM_SAMPLE_STEPS) + 1.0));
    vec3 sampleNoise = texture(hbaoNoiseTexture, fsIn.textureCoord * noiseScale).xyz;
    mat2 rotationMatrix = mat2(sampleNoise.x, sampleNoise.y, -sampleNoise.y, sampleNoise.x);
    deltaUV = rotationMatrix * deltaUV;
    float jitter = sampleNoise.z;
    float occlusion = 0.0;

    for (int i = 0; i < NUM_SAMPLE_DIRECTIONS; ++i)
    {
        deltaUV = deltaRotationMatrix * deltaUV;
        vec2 sampleDirUV = deltaUV;
        float oldAngle = bias;

        for (int j = 0; j < NUM_SAMPLE_STEPS; ++j)
        {
            vec2 sampleUV = fsIn.textureCoord + (jitter + float(j)) * sampleDirUV;
            vec3 samplePosition = texture(positionTexture, sampleUV).xyz;
            vec3 sampleDirection = (samplePosition - fragmentPosition);
            float gamma = (PI / 2.0) - acos(dot(normal, normalize(sampleDirection)));

            if (gamma > oldAngle)
            {
                float value = sin(gamma) - sin(oldAngle);
                float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragmentPosition.z - samplePosition.z));
                occlusion += value * rangeCheck;
                oldAngle = gamma;
            }
        }
    }

    occlusion = 1.0 - occlusion / float(NUM_SAMPLE_DIRECTIONS);
    occlusion = clamp(pow(occlusion, 1.0 + INTENSITY), 0.0, 1.0);

    fragmentColor = vec4(vec3(occlusion), 1.0);
}
