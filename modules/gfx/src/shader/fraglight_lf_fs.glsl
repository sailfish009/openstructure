uniform bool lighting_flag;
uniform bool two_sided_flag;
uniform bool fog_flag;
uniform bool occlusion_flag;

// copy from basic_fl_vs !
bool DirectionalLight(in vec3 normal,
                      in float shin,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular)
{
  float n_vp = max(0.0, dot(normal, normalize(vec3(gl_LightSource[0].position))));
  if(two_sided_flag && n_vp==0.0) return false;
  float n_hv = max(0.0, dot(normal, vec3(gl_LightSource[0].halfVector)));
  float pf = n_vp>0.0 ? pow(n_hv, shin) : 0.0;

  ambient  += gl_LightSource[0].ambient;
  diffuse  += gl_LightSource[0].diffuse*n_vp;
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
    
    if(DirectionalLight(normal, gl_FrontMaterial.shininess, amb, diff, spec)) {
    
        color  = gl_FrontLightModelProduct.sceneColor  +
                 (amb  * gl_FrontMaterial.ambient * gl_Color) +
                 (diff * gl_FrontMaterial.diffuse * gl_Color) +
                 (spec * gl_FrontMaterial.specular);
    } else {
      DirectionalLight(-normal, gl_BackMaterial.shininess, amb, diff, spec);

      color = gl_BackLightModelProduct.sceneColor  +
              (amb  * gl_BackMaterial.ambient * gl_Color) +
              (diff * gl_BackMaterial.diffuse * gl_Color) +
              (spec * gl_BackMaterial.specular);
    }
    
    gl_FragColor = color;

  } else {
    gl_FragColor = gl_Color;
  }

  if(occlusion_flag) {
    gl_FragColor.rgb = mix(gl_TexCoord[2].stp,gl_FragColor.rgb, gl_TexCoord[2].q);
  }

  if(fog_flag) {
    float fog = clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0);
    gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_FragColor.rgb, fog);
  }
  gl_FragColor.a = gl_Color.a;
}
