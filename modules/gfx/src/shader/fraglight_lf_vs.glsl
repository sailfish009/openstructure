uniform bool occlusion_flag;

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

  if(occlusion_flag) {
    // ambient color
    gl_TexCoord[2] = gl_MultiTexCoord0;
    gl_FrontColor.rgb = gl_MultiTexCoord0.xyz;
    gl_BackColor.rgb = gl_MultiTexCoord0.xyz;
    gl_FrontColor.a = gl_Color.a;
    gl_BackColor.a = gl_Color.a;
  } else {
    gl_FrontColor=gl_Color;
    gl_BackColor=gl_Color;
  }
}
