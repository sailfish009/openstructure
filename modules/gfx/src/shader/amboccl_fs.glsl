uniform sampler2D depth;
uniform sampler2D norm;

void main()
{
  float val = texture2D(depth,gl_TexCoord[0].xy).r;
  gl_FragColor.rgb=vec3(val,val,val);
  gl_FragColor.a=1.0;
}
