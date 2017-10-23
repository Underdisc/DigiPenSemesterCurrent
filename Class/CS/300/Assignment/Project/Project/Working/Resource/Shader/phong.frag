/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core


in vec3 SNormal;
in vec3 SFragPos;

out vec4 OFragColor;

uniform vec3 UObjectColor = vec3(1.0, 1.0, 1.0);

struct Light
{
  bool _active;
  vec3 _position;
  float _ambientFactor;
  float _diffuseFactor;
  float _specularFactor;
  float _specularExponent;

  vec3 _ambientColor;
  vec3 _diffuseColor;
  vec3 _specularColor;
};

uniform Light light = Light(true, vec3(0.0, 0.0, 0.0), 0.5, 0.4, 0.5, 20.0, vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(0.8, 0.6, 0.0));

void main()
{
  vec3 ambient_color = light._ambientFactor * light._ambientColor;

  vec3 normal = normalize(SNormal);
  vec3 light_dir = normalize(light._position - SFragPos);
  float diffuse_dot = max(dot(normal, light_dir), 0.0);
  vec3 diffuse_color = light._diffuseFactor * diffuse_dot * light._diffuseColor;

  vec3 camera_position = vec3(0.0, 0.0, 0.0);
  vec3 view_dir = normalize(camera_position - SFragPos);
  vec3 reflect_light_dir = 2.0 * dot(normal, light_dir) * normal - light_dir;
  float dot_result = dot(view_dir, reflect_light_dir);
  float specular_spread = pow(max(dot_result, 0.0), light._specularExponent);
  vec3 specular_color = light._specularFactor * light._specularColor * specular_spread;

  vec3 result_color = (ambient_color + diffuse_color + specular_color) * UObjectColor;
  OFragColor = vec4(result_color, 1.0);
}
