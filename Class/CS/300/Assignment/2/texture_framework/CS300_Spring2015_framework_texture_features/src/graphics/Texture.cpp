#include "Precompiled.h"
#include "framework/Debug.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

static u8 const UnbuiltTexture = 0;
static u8 const UnboundTexture = -1;

// Converts the relative path to a useful relative path (based on the local
// directory of the executable). This assumes ASSET_PATH is correct.
static std::string GetFilePath(std::string const &relativePath)
{
  std::stringstream strstr;
  strstr << ASSET_PATH << "textures/" << relativePath;
  return strstr.str();
}

namespace Graphics
{
  Texture::Texture(u8 *pixels, u32 width, u32 height)
    : pixels_(pixels), width_(width), height_(height), 
    textureHandle_(UnbuiltTexture), boundSlot_(UnboundTexture)
  {
  }

  Texture::~Texture()
  {
    Destroy();
    delete[] pixels_;
  }

  void Texture::Build()
  {
    Assert(textureHandle_ == UnbuiltTexture,
      "Cannot build already built texture.");

    // create a new texture
    glGenTextures(1, &textureHandle_);

    // bind the generated texture and upload its image contents to OpenGL
    glBindTexture(GL_TEXTURE_2D, textureHandle_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB,
      GL_UNSIGNED_BYTE, pixels_);

    // unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Texture::Bind(u8 slot)
  {
    // Enable the texture unit given the specified slot, then bind this texture
    // as the 2D texture for that slot.

    WarnIf(textureHandle_ == UnbuiltTexture,
      "Warning: Cannot bind unbuilt texture.");
    WarnIf(boundSlot_ != UnboundTexture,
      "Warning: Cannot rebind texture until it is unbound.");
    glActiveTexture(GL_TEXTURE0 + slot); // bind texture to slot
    glBindTexture(GL_TEXTURE_2D, textureHandle_);
    boundSlot_ = slot;
  }

  bool Texture::IsBound() const
  {
    return boundSlot_ != UnboundTexture;
  }

  void Texture::AttachTexture(std::shared_ptr<ShaderProgram> const &shader,
    std::string const &uniformName)
  {
    // bind the texture slot to the uniform since this is the activated texture
    // in that slot (allows the shader to sample the sampler given the input
    // name, wherein it will directly be pulling pixel data from this texture
    // instance).
    shader->SetUniform(uniformName, u32(static_cast<u32>(boundSlot_)));
  }

  void Texture::Unbind()
  {
    WarnIf(boundSlot_ == UnboundTexture,
      "Warning: Cannot unbind unbound texture.");
    glActiveTexture(GL_TEXTURE0 + boundSlot_); // unbind texture from slot
    glBindTexture(GL_TEXTURE_2D, 0);
    boundSlot_ = UnboundTexture; // unbound
  }

  void Texture::Destroy()
  {
    if (textureHandle_ == UnbuiltTexture)
      return; // nothing to destroy
    if (boundSlot_ != UnboundTexture)
      Unbind();
    glDeleteTextures(1, &textureHandle_); // wipe out the texture
    textureHandle_ = UnbuiltTexture;
  }

  u32 Texture::GetWidth() const
  {
    return width_;
  }

  u32 Texture::GetHeight() const
  {
    return height_;
  }

  std::shared_ptr<Texture> Texture::LoadTGA(std::string const &path)
  {
    // STB image handles the type based on the format itself
    return LoadPNG(path);
  }

  std::shared_ptr<Texture> Texture::LoadPNG(std::string const &relative)
  {
    // convert relative path (to textures) to a fully qualified relative path
    // (relative to the executable itself)
    std::string path = GetFilePath(relative);

    // attempt to load a PNG/TGA file using STB Image
    int width = 0, height = 0, bpp = 0;
    void *data = stbi_load(path.c_str(), &width, &height, &bpp, STBI_rgb);

    Assert(data, "Error: Unable to read file: textures/%s, reason: %s",
      relative.c_str(), stbi_failure_reason());
    if (!data)
      return NULL; // failed to load

    Assert(bpp == 3, "Error: Can only handle RGB images in the CS300 framework."
      " No alpha channels supported. Read file with bpp=%d", bpp);

    Texture *texture = NULL;
    if (bpp == 3) // successfully read an image with 3 channels of data
    {
      // copy data from loaded STB image buffer to local pixel buffer
      u8 *pixelData = new u8[width * height * bpp];
      std::memcpy(pixelData, data, width * height * bpp);
      texture = new Texture(pixelData, u32(width), u32(height));
    }

    stbi_image_free(data);
    return std::shared_ptr<Texture>(texture);
  }
}
