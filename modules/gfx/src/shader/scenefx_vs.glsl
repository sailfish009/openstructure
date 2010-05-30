void main()
{    
  gl_Position = ftransform();
  // relative screen coordinates
  gl_TexCoord[0] = gl_MultiTexCoord0;
}

