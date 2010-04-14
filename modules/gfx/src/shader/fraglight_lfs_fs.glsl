uniform sampler2D shadow_map;
uniform bool shadow_flag;
uniform float depth_bias;
uniform float epsilon;
uniform float shadow_multiplier;
uniform bool lighting_flag;
uniform bool two_sided_flag;
uniform bool fog_flag;

// copy from basic_lfs_fs !
float CalcShadowFactor(in vec4 coord, in vec2 o)
{
  // get original depth value of line projected towards light
  float d = texture2D(shadow_map, coord.xy+o*epsilon).x+depth_bias;
  return d<=coord.z ? shadow_multiplier : 1.0;
}

// modified copy from basic_fl_vs !
bool DirectionalLight(in vec3 normal,
                      in float shin,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular,
                      inout bool lflag)
{
  float n_vp = max(0.0, dot(normal, normalize(gl_LightSource[0].position.xyz)));

  lflag = n_vp>0.0;
  if(n_vp==0.0 && two_sided_flag) return false;

  float pf = 0.0;
  if(n_vp>0.0 && shin>0.0) {
    float n_hv = max(0.0, dot(normal, normalize(gl_LightSource[0].halfVector.xyz)));
    pf=pow(n_hv, shin);
  }

  ambient  += gl_LightSource[0].ambient;
  diffuse  += gl_LightSource[0].diffuse * n_vp;
  specular += gl_LightSource[0].specular * pf;

  return true;
}

void main()
{
  bool lflag=false;

  if(lighting_flag) {
    vec3 normal = normalize(gl_TexCoord[0].stp);

    vec4 amb = vec4(0.0);
    vec4 diff = vec4(0.0);
    vec4 spec = vec4(0.0);
    vec4 color = vec4(0.0);
    
    if(DirectionalLight(normal, gl_FrontMaterial.shininess, amb, diff, spec,lflag)) {
   
        color  = gl_FrontLightModelProduct.sceneColor  +
                 (amb  * gl_FrontMaterial.ambient * gl_Color) +
                 (diff * gl_FrontMaterial.diffuse * gl_Color) +
                 (spec * gl_FrontMaterial.specular);
    } else {
      bool dummy;
      DirectionalLight(-normal, gl_BackMaterial.shininess, amb, diff, spec, dummy);

      color = gl_BackLightModelProduct.sceneColor  +
              (amb  * gl_BackMaterial.ambient * gl_Color) +
              (diff * gl_BackMaterial.diffuse * gl_Color) +
              (spec * gl_BackMaterial.specular);

    }
    
    gl_FragColor = color;

  } else {
    gl_FragColor = gl_Color;
  }

  float fog = fog_flag ? clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0) : 1.0;
  if(shadow_flag) {
    vec4 coord = gl_TexCoord[1]/gl_TexCoord[1].w;
    float sum = 0.0;
    if(lflag) {
      sum += 0.18*CalcShadowFactor(coord, vec2(-0.7, -0.7));
      sum += 0.18*CalcShadowFactor(coord, vec2(0.7, -0.7));
      sum += 0.18*CalcShadowFactor(coord, vec2(0.7, 0.7));
      sum += 0.18*CalcShadowFactor(coord, vec2(-0.7, 0.7));
      sum += 0.28*CalcShadowFactor(coord, vec2(0, 0));
    } else {
      sum = shadow_multiplier;
    }

    gl_FragColor = vec4(sum*mix(gl_Fog.color.rgb, gl_FragColor.rgb, fog),gl_FragColor.a);
  } else {
    gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_FragColor.rgb, fog);
    gl_FragColor.a = gl_Color.a;
  }
}
