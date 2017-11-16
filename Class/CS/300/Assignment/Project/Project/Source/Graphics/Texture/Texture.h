/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Texture.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 2017/10/11
\brief
  Contains the interface for the Texture class.
*/
/*****************************************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

/*****************************************************************************/
/*!
\class Texture
\brief
  Used for loading a texture and storing the texture in an OpenGL texture
  buffer.
*/
/*****************************************************************************/
class Texture
{
public:
  Texture(const std::string & file_name);
  ~Texture();
private:
  //! The images color data.
  unsigned char * _imageData;
  //! The channels that are used by the image.
  int _channels;
  //! The width of the texture.
  int _width;
  //! The height of the texture.
  int _height;
};

#endif // !TEXTURE_H