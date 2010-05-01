uniform sampler2D scene;
uniform sampler2D depth;
uniform vec2 scalef;
uniform vec2 zmorph;
uniform bool shadow_flag;
uniform sampler2D shadow_map;
uniform float shadow_depth_bias;
uniform float shadow_epsilon;
uniform float shadow_multiplier;
uniform bool occl_flag;
uniform sampler2D occl_map;

float CalcShadowFactor(in vec4 coord, in vec2 o)
{
  // get original depth value of line projected towards light
  float d = texture2D(shadow_map, coord.xy+o*shadow_epsilon).x+shadow_depth_bias;
  return d<=coord.z ? shadow_multiplier : 1.0;
}

void main()
{
  float shadow_factor=1.0;
  if(shadow_flag) {
    // TODO: calculate appropriate coord from glTexCoord[1].xy and the depth map
    vec4 coord;
    shadow_factor = 0.0;
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(-0.7, -0.7));
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(0.7, -0.7));
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(0.7, 0.7));
    shadow_factor += 0.18*CalcShadowFactor(coord, vec2(-0.7, 0.7));
    shadow_factor += 0.28*CalcShadowFactor(coord, vec2(0, 0));
  }

  float occl_factor=1.0;
  if(occl_flag) {
    occl_factor=texture2D(occl_map,gl_TexCoord[0].xy).r;
  }
    
  vec4 scene_color=texture2D(scene,gl_TexCoord[0].xy);
  gl_FragColor.rgb = shadow_factor*occl_factor*scene_color.rgb;
  gl_FragColor.a = scene_color.a;
}
