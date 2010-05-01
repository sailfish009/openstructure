uniform sampler2D data;

void main()
{
  gl_FragColor=texture2D(data,gl_TexCoord[0].xy);
}
