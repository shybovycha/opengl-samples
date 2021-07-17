#version 430

layout (triangles) in;

// we emit 6 triangles for one input triangle - to be written to 6 textures of the cubemap
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 projectionViewMatrices[6];

out vec4 fragmentPosition;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;

        for (int vertex = 0; vertex < 3; ++vertex)
        {
            fragmentPosition = gl_in[vertex].gl_Position;
            gl_Position = projectionViewMatrices[face] * fragmentPosition;
            EmitVertex();
        }

        EndPrimitive();
    }
}
