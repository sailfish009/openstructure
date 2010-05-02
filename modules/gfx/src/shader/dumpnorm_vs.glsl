void main()
{
  gl_Position = ftransform();
  vec3 normal = gl_NormalMatrix * gl_Normal;
  gl_TexCoord[0].stp=normal;
}

