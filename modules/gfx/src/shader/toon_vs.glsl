void main()
{
  // transformed position
  gl_Position = ftransform();

  vec4 ec_Pos = gl_ModelViewMatrix* gl_Vertex;
  // for some reason, the fog and z coordinate are sign toggled...
  gl_FogFragCoord = -ec_Pos.z;

  vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
  // since a directional light is used, the position is not needed
  gl_TexCoord[0].stp=normal;

  gl_FrontColor=gl_Color;
  gl_BackColor=gl_Color;
}
