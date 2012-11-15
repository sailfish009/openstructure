vec4 light(in vec3 normal,
           in vec4 color,
           in bool two_sided)
{
  vec4 ground_color=0.2*color;
  vec4 sky_color=1.0*color;
  
  vec3 l_dir  = normalize(gl_LightSource[0].position.xyz); // assume directional vector
  float a  = 0.5+0.5*dot(normal,l_dir);
  color.rgb = mix(ground_color, sky_color, a).rgb;
  bool backside = false;
  if(two_sided && backside) {
    float a  = 0.5+0.5*dot(-normal,l_dir);
    color.rgb = mix(ground_color, sky_color, a).rgb;
  }
  return color;
}

