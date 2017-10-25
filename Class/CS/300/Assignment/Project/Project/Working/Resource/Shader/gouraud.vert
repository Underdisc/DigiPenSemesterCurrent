/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

in vec3 APosition;
in vec3 ANormal;

out vec3 SFragColor;

uniform mat4 UProjection = mat4(1,0,0,0,
  0,1,0,0,
  0,0,1,0,
  0,0,0,1);

uniform mat4 UView = mat4(1,0,0,0,
  0,1,0,0,
  0,0,1,0,
  0,0,0,1);

uniform mat4 UModel = mat4(1,0,0,0,
  0,1,0,0,
  0,0,1,0,
  0,0,0,1);

const uint MaxLights = 10;

uniform vec3 UObjectColor = vec3(1.0, 1.0, 1.0);

struct Light
{
  vec3 UPosition;
  float UAmbientFactor;
  float UDiffuseFactor;
  float USpecularFactor;
  float USpecularExponent;

  vec3 UAmbientColor;
  vec3 UDiffuseColor;
  vec3 USpecularColor;
};

uniform int UActiveLights = 1;
uniform Light ULights[MaxLights];

vec3 ComputeLight(int light, vec3 position, vec3 normal, vec3 view_dir)
{
  // ambient term
  vec3 ambient_color = ULights[light].UAmbientFactor * ULights[light].UAmbientColor;
  // diffuse term
  vec3 light_dir = normalize(ULights[light].UPosition - position);
  float ndotl = max(dot(normal, light_dir), 0.0);
  vec3 diffuse_color = ULights[light].UDiffuseFactor * ndotl * ULights[light].UDiffuseColor;
  // specular term
  vec3 reflect_dir = 2.0 * dot(normal, light_dir) * normal - light_dir;
  reflect_dir = normalize(reflect_dir);
  float vdotr = max(dot(view_dir, reflect_dir), 0.0);
  float specular_spread = pow(vdotr, ULights[light].USpecularExponent);
  vec3 specular_color = ULights[light].USpecularFactor * ULights[light].USpecularColor * specular_spread;
  // final color
  vec3 light_color = (ambient_color + diffuse_color + specular_color);
  return light_color;
  //return specular_color;
}

void main()
{
  gl_Position = UProjection * UView * UModel * vec4(APosition.xyz, 1.0);
  vec3 normal = normalize(mat3(transpose(inverse(UModel))) * ANormal);
  vec3 position = vec3(UModel * vec4(APosition, 1.0));

  vec3 camera_position = vec3(0.0, 0.0, 0.0);
  vec3 view_dir = normalize(camera_position - position);
  // summing all light results
  vec3 final_color = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < UActiveLights; ++i)
    final_color += ComputeLight(i, position, normal, view_dir);

  SFragColor = final_color;
}
