#version 330 core

in vec3 SFragPos;

uniform sampler2D UUp;
uniform sampler2D UDown;
uniform sampler2D ULeft;
uniform sampler2D URight;
uniform sampler2D UFront;
uniform sampler2D UBack;

void main()
{
  vec3 position = abs(SFragPos);
  vec3 final_color = vec3(0.0, 0.0, 0.0);
  vec2 uv = vec2(0.0, 0.0);
  if(position.x > position.y && position.x > position.z){
    if(position.x > 0){
      uv.x = (position.z + 0.5);
      uv.y = (position.y + 0.5);
      final_color = vec3(texture(URight, uv));
    }
    else{
      uv.x = (position.z + 0.5);
      uv.y = (position.y + 0.5);
      final_color = vec3(texture(ULeft, uv));
    }
  }
  else if(position.y > position.x && position.y > position.z){
    if(position.y > 0){
      uv.x = (position.x + 0.5);
      uv.y = (position.z + 0.5);
      final_color = vec3(texture(UUp, uv));
    }
    else{
      uv.x = (position.z + 0.5);
      uv.y = (position.y + 0.5);
      final_color = vec3(texture(UDown, uv));
    }
  }
  else {
    if(position.z > 0){
      uv.x = (position.x + 0.5);
      uv.y = (position.y + 0.5);
      final_color = vec3(texture(UBack, uv));
    }
    else{
      uv.x = (position.x + 0.5);
      uv.y = (position.y + 0.5);
      final_color = vec3(texture(UFront, uv));
    }
  }

  gl_FragColor = vec4(final_color, 1.0);

  // Use the frag pos to determine the texture

}
