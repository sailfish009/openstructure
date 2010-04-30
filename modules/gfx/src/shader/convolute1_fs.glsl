uniform sampler2D data;
uniform sampler1D kernel;
uniform int kernel_size;
uniform int vp_width;
uniform int vp_height;

void main()
{
  vec3 kernel_data[13];
  vec3 kmod=vec3(1.0/(float)vp_width,1.0/(float)vp_height,1.0);
  kernel_data[0]=kmod*vec3(-2,0,0.02);
  kernel_data[1]=kmod*vec3(-1,-1,0.141421);
  kernel_data[2]=kmod*vec3(-1,0,0.37606);
  kernel_data[3]=kmod*vec3(-1,1,0.141421);
  kernel_data[4]=kmod*vec3(0,-2,0.02);
  kernel_data[5]=kmod*vec3(0,-1,0.37606);
  kernel_data[6]=kmod*vec3(0,0,1);
  kernel_data[7]=kmod*vec3(0,1,0.37606);
  kernel_data[8]=kmod*vec3(0,2,0.02);
  kernel_data[9]=kmod*vec3(1,-1,0.141421);
  kernel_data[10]=kmod*vec3(1,0,0.37606);
  kernel_data[11]=kmod*vec3(1,1,0.141421);
  kernel_data[12]=kmod*vec3(2,0,0.02);

  float sum=0.0;
  float sum2=0.0;
  int i;
  for (i=0;i<13;i=i+1) {
    sum+=kernel_data[i].z*texture2D(data,gl_TexCoord[0].xy+kernel_data[i].xy).r;
    sum2+=kernel_data[i].z;
  }
  sum=sum/sum2;
  float delta=5.0*(sum-texture2D(data,gl_TexCoord[0].xy).r);
  if(delta<0.0) {
    gl_FragColor.rgb=vec3(1+delta,1+delta,1+delta);
  } else {
    gl_FragColor.rgb=vec3(1,1,1);
  }
  gl_FragColor.a=1.0;


/*
  float sum=0.0;
  float sum2=0.0;
  float isize=1.0/(float)(kernel_size);
  for (i=0;i<kernel_size;i=i+1) {
    float ii=(float(i)+0.5)*isize;
    vec3 kernel_value=texture1D(kernel,ii).xyz;
    vec2 offset = kernel_value.xy*2.0-1.0;
    sum+=kernel_value.z*texture2D(data,gl_TexCoord[0].xy+offset).r;
    sum2+=kernel_value.z;
  }
  sum=sum/sum2;
  float delta=(sum-texture2D(data,gl_TexCoord[0].xy).r);
  if(delta<0.0) {
    gl_FragColor.rgb=vec3(1+delta,1+delta,1+delta);
  } else {
    gl_FragColor.rgb=vec3(1,1,1);
  }
  gl_FragColor.a=1.0;
*/
}
