/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

in vec3 SNormal;
in vec3 SFragPos;

out vec4 OFragColor;

// The cameras world position
uniform vec3 UCameraPosition;

// Material values
struct Material
{
  vec3 UColor;
  // Material factors
  float UAmbientFactor;
  float UDiffuseFactor;
  float USpecularFactor;
  float USpecularExponent;
};

// Light values
struct Light
{
  // The type of the type: 0 - Point, 1 - Directional, 2 - Spot
  int UType;
  // The position of the light
  vec3 UPosition;
  // The direction of the light
  vec3 UDirection;
  // The light colors
  vec3 UAmbientColor;
  vec3 UDiffuseColor;
  vec3 USpecularColor;
};

const int MaxLights = 10;
uniform int UActiveLights = 1;
uniform Material UMaterial;
uniform Light ULights[MaxLights];

uniform vec3 UFogColor;
uniform float UNearPlane;
uniform float UFarPlane;

vec3 ComputeLight(int light, vec3 normal, vec3 view_dir)
{
  // ambient term
  vec3 ambient_color = UMaterial.UAmbientFactor * ULights[light].UAmbientColor;
  // finding light direction
  vec3 light_dir;
  if(ULights[light].UType == DIRECTIONAL)
    light_dir = -normalize(ULights[light].UDirection);
  else
    light_dir = normalize(ULights[light].UPosition - SFragPos);
  // diffuse term
  float ndotl = max(dot(normal, light_dir), 0.0);
  vec3 diffuse_color = UMaterial.UDiffuseFactor * ndotl * ULights[light].UDiffuseColor;
  // specular term
  vec3 reflect_dir = 2.0 * dot(normal, light_dir) * normal - light_dir;
  reflect_dir = normalize(reflect_dir);
  float vdotr = max(dot(view_dir, reflect_dir), 0.0);
  float specular_spread = pow(vdotr, UMaterial.USpecularExponent);
  vec3 specular_color = UMaterial.USpecularFactor * ULights[light].USpecularColor * specular_spread;
  // final color
  vec3 light_color = (ambient_color + diffuse_color + specular_color);
  return light_color;
}

void main()
{
  // precomputations
  vec3 normal = normalize(SNormal);
  vec3 view_vec = UCameraPosition - SFragPos;
  vec3 view_dir = normalize(view_vec);
  // summing all light results
  vec3 final_color = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < UActiveLights; ++i)
    final_color += ComputeLight(i, normal, view_dir);
  // accounting for object color
  final_color *= UMaterial.UColor;

  // accounting for fog
  float dist = length(view_vec);
  float fog_factor = (dist - UNearPlane) / (UFarPlane - UNearPlane);
  fog_factor = min(1.0, max(0.0, fog_factor));
  final_color = mix(final_color, UFogColor, fog_factor);
  // final color
  OFragColor = vec4(final_color, 1.0);
}
