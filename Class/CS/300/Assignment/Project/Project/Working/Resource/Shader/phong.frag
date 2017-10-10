/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core


in vec3 SNormal;
in vec3 SFragPos;

out vec4 OFragColor;

uniform vec3 UObjectColor = vec3(1.0, 1.0, 1.0);

uniform vec3 ULightPosition = vec3(0.0, 0.0, 0.0);
uniform float UAmbientFactor = 0.5;
uniform float UDiffuseFactor = 0.4;
uniform float USpecularFactor = 0.5;
uniform float USpecularExponent = 20.0;

uniform vec3 UAmbientColor = vec3(1.0, 1.0, 1.0);
uniform vec3 UDiffuseColor = vec3(1.0, 1.0, 1.0);
uniform vec3 USpecularColor = vec3(0.8, 0.6, 0.0);

void main()
{
  vec3 ambient_color = UAmbientFactor * UAmbientColor;

  vec3 normal = normalize(SNormal);
  vec3 light_dir = normalize(ULightPosition - SFragPos);
  float diffuse_factor = max(dot(normal, light_dir), 0.0);
  vec3 diffuse_color = UDiffuseFactor * diffuse_factor * UDiffuseColor;

  vec3 camera_position = vec3(0.0, 0.0, 0.0);
  vec3 view_dir = normalize(camera_position - SFragPos);
  vec3 reflect_light_dir = 2.0 * dot(normal, light_dir) * normal - light_dir;
  float dot_result = dot(view_dir, reflect_light_dir);
  float specular_spread = pow(max(dot_result, 0.0), USpecularExponent);
  vec3 specular_color = USpecularFactor * USpecularColor * specular_spread;

  vec3 result_color = (ambient_color + diffuse_color + specular_color) * UObjectColor;
  OFragColor = vec4(result_color, 1.0);
}
