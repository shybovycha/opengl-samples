#version 410

struct TVertex
{
    vec3 position;
    vec2 uv;
};

layout (location = 0) in TVertex vert;

out vec2 uv;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vert.position, 1.0);
    uv = vert.uv;
}
