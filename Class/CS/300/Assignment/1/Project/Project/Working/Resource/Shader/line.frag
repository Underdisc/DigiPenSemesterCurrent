#version 330 core

out vec4 FragColor;

uniform vec3 ULineColor = vec3(1.0f, 1.0f, 0.0f);

void main()
{
  FragColor = vec4(ULineColor, 1.0);
}
