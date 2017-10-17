/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

out vec4 FragColor;

uniform vec3 ULineColor = vec3(1.0, 1.0, 1.0);
uniform float UTime = 0.0f;

void main()
{
  // For Rainbow lines

  float frag_pos_x = gl_FragCoord.x / 100.0;
  float sin_time = (sin(UTime + frag_pos_x) + 1.0) / 2.0;
  float sin_time_shift = (sin(UTime + frag_pos_x + 3.14) + 1.0) / 2.0;
  float cos_time = (cos(UTime + frag_pos_x) + 1.0) / 2.0;
  vec3 result_color = vec3(sin_time * ULineColor.x,
                           cos_time * ULineColor.y,
                           sin_time_shift * ULineColor.z);

  FragColor = vec4(result_color, 1.0);
}
