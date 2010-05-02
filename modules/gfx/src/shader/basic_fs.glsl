uniform bool fog_flag;

void main()
{
  float fog = fog_flag ? clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0) : 1.0;
  gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_Color.rgb, fog);
  gl_FragColor.a = gl_Color.a;
  // gl_FragBuffer[1].rgb=gl_TexCoord[0].stp*0.5+0.5;
}

