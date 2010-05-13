uniform vec3 wpos;
uniform vec3 wdir;
uniform float wlen;
uniform float rad;
uniform sampler2D depth_map;

void main()
{
  float depth = texture2D(depth_map,gl_TexCoord[0].xy).r;
  if(depth>=1.0) {
    //discard;
  }
  float d = length(cross(wdir,vec3(gl_FragCoord.xy,depth)-wpos))/wlen;
  if(d<rad) {
    gl_FragColor.rgb=vec3(0,1,0);
    gl_FragColor.a=1.0;
  } else {
    gl_FragColor.rgb=vec3(1,0,0);
    gl_FragColor.a=1.0;
  }
}
