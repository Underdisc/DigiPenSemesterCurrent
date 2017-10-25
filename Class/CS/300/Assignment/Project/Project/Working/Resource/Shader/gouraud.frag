/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

in vec3 SFragColor;

out vec4 OFragColor;

void main()
{
  OFragColor = vec4(SFragColor, 1.0);
}
