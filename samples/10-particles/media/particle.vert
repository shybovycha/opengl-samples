#version 410

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoord;

out VS_OUT
{
    vec2 textureCoord;
    vec4 vertexColor;
} vsOut;

out gl_PerVertex {
    vec4 gl_Position;
};

uniform mat4 transformationMatrix;
uniform float lifetime;

void main()
{
    float alpha = max(min(lifetime, 1.0), 0.0);

    vsOut.textureCoord = vertexTextureCoord;
    vsOut.vertexColor = vec4(1.0, 1.0, 1.0, alpha);

    gl_Position = transformationMatrix * vec4(vertexPosition, 1.0);
}
