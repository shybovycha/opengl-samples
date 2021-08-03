#version 430

layout (triangles) in;

// we emit 6 triangles for one input triangle - to be written to 6 textures of the cubemap
layout (triangle_strip, max_vertices = 18) out;

in VS_OUT
{
    vec4 vertexPosition;
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} gsIn[];

out GS_OUT
{
    vec4 vertexPosition;
    vec4 fragmentPosition;
    vec3 normal;
    vec2 textureCoords;
} gsOut;

uniform mat4 projectionViewMatrices[6];

// out vec4 fragmentPosition;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;

        for (int vertex = 0; vertex < 3; ++vertex)
        {
            gsOut.vertexPosition = gsIn[vertex].vertexPosition;
            gsOut.fragmentPosition = projectionViewMatrices[face] * gsIn[vertex].vertexPosition;
            gsOut.normal = gsIn[vertex].normal;
            gsOut.textureCoords = gsIn[vertex].textureCoords;

            gl_Position = projectionViewMatrices[face] * gsIn[vertex].vertexPosition;

            EmitVertex();
        }

        EndPrimitive();
    }
}
