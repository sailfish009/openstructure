uniform sampler2D depth_map;
uniform sampler2D norm_map;
uniform sampler1D kernel;
uniform float step;
uniform vec2 i_vp;
uniform vec4 abcd;

vec3 unproject(in vec3 scr)
{ 
  vec3 tmp=vec3(scr.x*i_vp.x*2.0-1.0,scr.y*i_vp.y*2.0-1.0,scr.z);
  //float iw = 1.0/(tmp.z/abcd.w+abcd.z/abcd.w);
  //return iw*vec3(tmp.x/abcd.x,tmp.y/abcd.y,-1.0);
  vec4 tmp2 = gl_ProjectionMatrixInverse*vec4(tmp,1.0);
  return tmp2.xyz/tmp2.w;
}

float ao(in vec2 tab, in vec3 pos_p, in vec3 norm_p, in vec3 t_pos_p)
{
  float depth_q = texture2D(depth_map,gl_TexCoord[0].xy+tab.xy*i_vp).r;
  if(depth_q>=1.0) {
    return 0.0;
  }
  vec3 pos_q = vec3(gl_FragCoord.xy+tab.xy,depth_q);
  vec3 t_pos_q = unproject(pos_q);

  vec3 diff=t_pos_q-t_pos_p;
  float fac=1.0+dot(diff,diff)/5.0;
  return max(0.0,dot(normalize(norm_p),normalize(diff)))/fac;
}

void main()
{
  float depth_p = texture2D(depth_map,gl_TexCoord[0].xy).r;
  if(depth_p>=1.0) {
    gl_FragColor = vec4(0,0,0,1);
    return;
  }
  vec3 norm_p = (texture2D(norm_map,gl_TexCoord[0].xy).xyz-0.5)*2.0;
  vec3 pos_p = vec3(gl_FragCoord.xy,depth_p);
  vec3 t_pos_p = unproject(pos_p);

  float i;
  float sum=0.0;
  for(i=0.0;i<1.0;i+=step) {
    vec2 nn=texture1D(kernel,i).xy*40.0-20.0;
    sum+=ao(nn,pos_p,norm_p,t_pos_p);
  }
  sum*=step;

  gl_FragColor.rgb=vec3(sum,sum,sum);
  gl_FragColor.a=1.0;
}
