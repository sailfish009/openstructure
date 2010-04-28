uniform bool lighting_flag;
uniform bool two_sided_flag;
uniform bool fog_flag;
uniform bool occlusion_flag;
varying vec4 ambient_color;

// copy from basic_fl_vs !
bool DirectionalLight(in vec3 normal,
                      in float shin,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular)
{
  float n_vp = max(0.0, dot(normal, normalize(gl_LightSource[0].position.xyz)));

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
  if(lighting_flag) {
    vec3 normal = normalize(gl_TexCoord[0].stp);

    vec4 amb = vec4(0.0);
    vec4 diff = vec4(0.0);
    vec4 spec = vec4(0.0);
    vec4 color = vec4(0.0);

    /* 
      For ambient occlusion, this blends the local ambient color together with
      the fragment color at intensity given my the ambient material settings;
      ambient_color defaults to gl_Color, so for non ambient-occluded scenes,
      this is a noop
    */
    vec4 diff_color = gl_Color;
    if(occlusion_flag) {
      diff_color.rgb = mix(gl_Color.rgb,ambient_color.rgb,gl_FrontMaterial.ambient.rgb);
    }

    if(DirectionalLight(normal, gl_FrontMaterial.shininess, amb, diff, spec)) {

      color  = gl_FrontLightModelProduct.sceneColor  +
               (amb  * gl_FrontMaterial.diffuse * diff_color * ambient_color.a) +
               (diff * gl_FrontMaterial.diffuse * diff_color) +
               (spec * gl_FrontMaterial.specular);
    } else {
      DirectionalLight(-normal, gl_BackMaterial.shininess, amb, diff, spec);

      color = gl_BackLightModelProduct.sceneColor  +
              (amb  * gl_BackMaterial.ambient * diff_color * ambient_color.a) +
              (diff * gl_BackMaterial.diffuse * diff_color) +
              (spec * gl_BackMaterial.specular);
    }
    
    gl_FragColor = color;

  } else {
    gl_FragColor = gl_Color;
  }

  if(fog_flag) {
    float fog = clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0);
    gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_FragColor.rgb, fog);
  }
  gl_FragColor.a = gl_Color.a;
}
