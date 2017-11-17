/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Texture.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 2017/10/11
\brief
  Contains the implementation of the Texture class.
*/
/*****************************************************************************/

// using stb's image functions
#define STB_IMAGE_IMPLEMENTATION
#include <STB\stb_image.h>
#include "../../Utility/Error.h"
#include "Texture.h"

/*****************************************************************************/
/*!
\brief
  Constructor for a Texture.

\param filename
  The name of the file being loaded.
*/
/*****************************************************************************/
Texture::Texture(const std::string & filename) :
  _imageFile(filename)
{
  // loading image
  stbi_set_flip_vertically_on_load(true);
  _imageData = stbi_load(filename.c_str(), &_width, 
    &_height, &_channels, 0);
  if (!_imageData) {
    Error error("Texture.cpp", "Texture Constructor");
    error.Add("Image file failed to load.");
    error.Add("> Image File");
    error.Add(filename.c_str());
    throw(error);
  }
}

Texture::~Texture()
{
  stbi_image_free(_imageData);
}