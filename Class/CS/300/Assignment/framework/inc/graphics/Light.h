#ifndef H_LIGHT
#define H_LIGHT

#include "graphics/Color.h"
#include "math/Vector4.h"

namespace Graphics
{
  // This structure represents a light very similarly to the struct defined in
  // the vertex shader (shader.vert). The structure is carefully setup to be
  // directly copyable to the GPU. However, you will likely add a lot of code
  // to this structure in future assignments as you need to implement a more
  // sophisticated approach to lighting.
  struct Light
  {
    // positional light
    Light(Math::Vector4 const &direction, Color const &ambient = Color::White,
      Color const &diffuse = Color::White);

    // setup so it can be copied right over to the GPU
    Math::Vector4 direction;
    Color ambient;
    Color diffuse;
  };
}

#endif
