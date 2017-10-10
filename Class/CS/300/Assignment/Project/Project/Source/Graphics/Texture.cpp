#define STB_IMAGE_IMPLEMENTATION
#include <STB\stb_image.h>

#include "../Utility/OpenGLError.h"

#include "Texture.h"

#define RGB  3
#define RGBA 4

Texture::Texture(const std::string & filename)
{
  // loading image
  stbi_set_flip_vertically_on_load(true);
  int channels;
  unsigned char * image_data = stbi_load(filename.c_str(), &_width, 
    &_height, &channels, 0);
  if (!image_data) {
    Error error("Texture.cpp", "Texture Constructor");
    error.Add("Image file failed to load.");
    error.Add("> Image File");
    error.Add(filename.c_str());
    throw(error);
  }
  // generating opengl texture object
  glGenTextures(1, &_textureObject);
  glBindTexture(GL_TEXTURE_2D, _textureObject);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // for rgb or rgba
  switch (channels)
  {
  case RGB:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0,
      GL_RGB, GL_UNSIGNED_BYTE, image_data);
    break;
  case RGBA:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, 
      GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    break;
  default:
    Error error("Texture.cpp", "Texture Constructor");
    error.Add("Image file format not supported");
    error.Add("> Image file");
    error.Add(filename.c_str());
    throw(error);
  }
  glGenerateMipmap(GL_TEXTURE_2D);
  // freeing image data
  stbi_image_free(image_data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

inline void Texture::Bind()
{
  glBindTexture(GL_TEXTURE_2D, _textureObject);
}

inline void Texture::Unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}