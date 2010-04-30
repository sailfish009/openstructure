void main()
{    
  gl_Position = ftransform();
  gl_TexCoord[0].st = 0.5*(gl_Position.xy+vec2(1.0,1.0));
}

