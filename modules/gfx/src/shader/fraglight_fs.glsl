uniform bool lighting_flag;
uniform bool two_sided_flag;
uniform bool fog_flag;
uniform bool occlusion_flag;
uniform vec2 ambient_weight;
varying vec4 ambient_color;
uniform sampler2D depth_map;
uniform int depth_mode;

// copy from basic_fl_vs !
bool DirectionalLight(in vec3 normal,
                      in float shin,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular,
		      inout bool lflag)
{
  float n_vp = max(0.0, dot(normal, normalize(gl_LightSource[0].position.xyz)));

  float pf = 0.0;
  lflag = n_vp>0.0;
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

    vec4 color = gl_Color;
    if(occlusion_flag) {
      /* 
        For ambient occlusion and local coloring, two effects are possible. 
        (1) Blending of the original fragment color and the accumulated
            color of the neighbouring fragments, by ambient_weight[0].
        (2) Attenuating the resulting color intensity by the ambient occlusion,
            modulated by ambient_weight[1]
        Only the rgb values are affected, fragment opacity is unchanged
      */

      color.rgb = mix(gl_Color.rgb,ambient_color.rgb,ambient_weight[0]);
      color.rgb = mix(color.rgb,ambient_color.aaa*color.rgb,ambient_weight[1]);
    }

    if(DirectionalLight(normal, gl_FrontMaterial.shininess, amb, diff, spec,lflag)) {

      color  = gl_FrontLightModelProduct.sceneColor  +
               (amb  * gl_FrontMaterial.ambient * color) +
               (diff * gl_FrontMaterial.diffuse * color) +
               (spec * gl_FrontMaterial.specular);
    } else {
      bool dummy;
      DirectionalLight(-normal, gl_BackMaterial.shininess, amb, diff, spec, dummy);

      color = gl_BackLightModelProduct.sceneColor  +
              (amb  * gl_BackMaterial.ambient * color) +
              (diff * gl_BackMaterial.diffuse * color) +
              (spec * gl_BackMaterial.specular);
    }
    
    gl_FragColor = color;

  } else {
    gl_FragColor = gl_Color;
  }

  float fog = fog_flag ? clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0) : 1.0;
  gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_FragColor.rgb, fog);
  gl_FragColor.a = gl_Color.a;
  //gl_FragBuffer[1].rgb=gl_TexCoord[0].stp*0.5+0.5;
}
