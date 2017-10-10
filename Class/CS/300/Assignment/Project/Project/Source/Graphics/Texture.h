#include <string>

#include <GL\glew.h>


class Texture
{
public:
  Texture(const std::string & file_name);
  void Bind();
  static void Unbind();
private:
  GLuint _textureObject;
  int _width;
  int _height;
};