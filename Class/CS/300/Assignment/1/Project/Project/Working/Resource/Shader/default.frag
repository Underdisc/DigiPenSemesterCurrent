#version 330 core


in vec3 SNormal;
in vec3 SFragPos;

out vec4 OFragColor;

uniform vec3 ULightDirection = vec3(-1.0, -1.0, -1.0);

uniform vec3 UObjectColor = vec3(1.0, 1.0, 1.0);
uniform float UAmbientFactor = 0.5;
uniform vec3 UAmbientColor = vec3(1.0, 1.0, 1.0);
uniform float UDiffuseFactor = 0.4;
uniform vec3 UDiffuseColor = vec3(0.0, 0.0, 0.5);

void main()
{
  vec3 ambient_color = UAmbientFactor * UAmbientColor;

  vec3 normal = normalize(SNormal);
  vec3 light_dir = normalize(-ULightDirection);
  float diffuse_factor = max(dot(normal, light_dir), 0.0);
  vec3 diffuse_color = diffuse_factor * UDiffuseColor;

  vec3 result_color = (ambient_color + diffuse_color) * UObjectColor;
  OFragColor = vec4(result_color, 1.0f);
}
