uniform sampler2D scene_map;
uniform sampler2D depth_map;
uniform vec2 i_vp;
uniform vec4 abcd;
uniform bool shadow_flag;
uniform sampler2D shadow_map;
uniform float shadow_depth_bias;
uniform float shadow_epsilon;
uniform float shadow_multiplier;
uniform bool occl_flag;
uniform sampler2D occl_map;
uniform float occl_mult;
uniform bool dark_flag;
uniform sampler2D dark_map;
uniform float dark_mult;

vec3 unproject(in vec3 scr)
{ 
  vec3 tmp=vec3(scr.x*i_vp.x*2.0-1.0,scr.y*i_vp.y*2.0-1.0,scr.z);
  float iw = 1.0/(tmp.z/abcd.w+abcd.z/abcd.w);
  return iw*vec3(tmp.x/abcd.x,tmp.y/abcd.y,-1.0);
}

float CalcShadowFactor(in vec4 coord, in vec2 o)
{
  // get original depth value of line projected towards light
  float d = texture2D(shadow_map, coord.xy+o*shadow_epsilon).x+shadow_depth_bias;
  return d<=coord.z ? shadow_multiplier : 1.0;
}

void main()
{
  vec4 scene_color=texture2D(scene_map,gl_TexCoord[0].xy);
  gl_FragColor.a = scene_color.a;

  float shadow_factor=1.0;
  if(shadow_flag) {
    float depth = texture2D(depth_map,gl_TexCoord[0].xy).r;
    vec4 tcoord = gl_ModelViewProjectionMatrixInverse*vec4(gl_FragCoord.xy*i_vp*2.0-1.0,depth,1.0);
    vec4 coord = gl_TextureMatrix[0]*tcoord;
    coord/=coord.w;
    shadow_factor = 0.0;
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(-0.7, -0.7));
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(0.7, -0.7));
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(0.7, 0.7));
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(-0.7, 0.7));
    shadow_factor += 0.28*CalcShadowFactor(coord, vec2(0, 0));
  }

  float occl_factor=1.0;
  if(occl_flag) {
    occl_factor=max(0.0,1.0-occl_mult*texture2D(occl_map,gl_TexCoord[0].xy).r);
  }

  float dark_factor=1.0;
  if(dark_flag) {
    dark_factor=max(0.0,1.0-dark_mult*(1.0-texture2D(dark_map,gl_TexCoord[0].xy).r));
  }
    
  gl_FragColor.rgb = shadow_factor*occl_factor*dark_factor*scene_color.rgb;
}
