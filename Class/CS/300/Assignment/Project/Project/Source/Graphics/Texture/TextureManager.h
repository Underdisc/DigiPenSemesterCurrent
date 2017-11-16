
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <GL/glew.h>
#include "Texture.h"

#define MAXBOUNDTEXTURES

class TextureManager;

class TextureObject {
private:
  TextureObject();
  GLuint _glID;
  int _boundLocation;
  friend TextureManager;
};

class TextureManager
{
public:
  TextureObject * Upload(const Texture & texture);
  void Unload(TextureObject * texture_object);
  void Bind(TextureObject * texture_object, int location);
  void Unbind(TextureObject * texture_object);


  TextureObject * _boundTextures[MAXBOUNDTEXTURES]
};

#endif // !TEXTUREMANAGER_H