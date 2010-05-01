uniform sampler2D shadow_map;
uniform bool shadow_flag;
uniform float shadow_depth_bias;
uniform float shadow_epsilon;
uniform float shadow_multiplier;
uniform bool fog_flag;

float CalcShadowFactor(in vec4 coord, in vec2 o)
{
  // get original depth value of line projected towards light
  float d = texture2D(shadow_map, coord.xy+o*shadow_epsilon).x+shadow_depth_bias;
  return d<=coord.z ? shadow_multiplier : 1.0;
}

void main()
{
  float fog = fog_flag ? clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0) : 1.0;
  if(shadow_flag) {
    vec4 coord=gl_TexCoord[1]/gl_TexCoord[1].w;
    float sum = 0.0;
    sum += 0.18*CalcShadowFactor(coord, vec2(-0.7, -0.7));
    sum += 0.18*CalcShadowFactor(coord, vec2(0.7, -0.7));
    sum += 0.18*CalcShadowFactor(coord, vec2(0.7, 0.7));
    sum += 0.18*CalcShadowFactor(coord, vec2(-0.7, 0.7));
    sum += 0.28*CalcShadowFactor(coord, vec2(0, 0));

    gl_FragColor = vec4(sum*mix(gl_Fog.color.rgb, gl_Color.rgb, fog),gl_Color.a);
  } else {
    gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_Color.rgb, fog);
    gl_FragColor.a = gl_Color.a;
  }
}

