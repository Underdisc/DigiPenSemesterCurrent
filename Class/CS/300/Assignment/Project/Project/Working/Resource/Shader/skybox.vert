#version 330 core

in vec3 APosition;

out vec3 SFragPos;

uniform mat4 UProjection;
uniform mat4 UView;

void main()
{
  // remove translation part of view matrix
  mat4 view = UView;
  view[3][0] = 0.0;
  view[3][1] = 0.0;
  view[3][2] = 0.0;
  // Set position and frag world position
  gl_Position = UProjection * view * vec4(APosition, 1.0);
  SFragPos = APosition;
}
