#version 410

// in vec4 gl_FragCoord;
// out float gl_FragDepth;

void main()
{
  gl_FragDepth = gl_FragCoord.z;
}
