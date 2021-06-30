#version 410

layout(triangles) in;

/*
 this shader will be emitting data to 4 shadow map layers as per 4 frustum splits;
 each triangle fed into this geometry shader will be projected into each shadow map layer;
 hence this shader will emit 4 layers * 3 vertices (per each input primitive / geometry) = 12 vertices;

 note how the input vertices are in the world space, so we need to project them to the clip space in here
*/
layout(triangle_strip, max_vertices = 12) out;

out gl_PerVertex
{
  vec4 gl_Position;
};

out int gl_Layer;

uniform mat4 lightViewProjectionMatrix[4]; // as per 4 frustum splits

void main()
{
    for (int split = 0; split < 4; ++split)
    {
        // input primitive index
        for (int i = 0; i < gl_in.length(); ++i)
        {
            // in a 3D texture, which layer do we project our input primitive to
            gl_Layer = split;

            // project an input primitive using the corresponding projection matrix from the uniform
            gl_Position = lightViewProjectionMatrix[split] * gl_in[i].gl_Position;

            EmitVertex();
        }

        EndPrimitive();
    }
}
