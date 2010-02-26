void main()
{
  // transformed position
  gl_Position = ftransform();

  vec4 ec_Pos = gl_ModelViewMatrix* gl_Vertex;
  // for some reason, the fog and z coordinate are sign toggled...
  gl_FogFragCoord = -ec_Pos.z;

  gl_TexCoord[0]=gl_MultiTexCoord0;

  gl_FrontColor=gl_Color;
  gl_BackColor=gl_Color;
}
