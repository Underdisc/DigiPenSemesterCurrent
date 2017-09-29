#include "Precompiled.h"
#include "graphics/Light.h"

namespace Graphics
{
  // Initialize the light structure.
  Light::Light(Math::Vector4 const &_direction,
    Color const &_ambient/* = Color::White*/,
    Color const &_diffuse/* = Color::White*/)
    : direction(_direction.Normalized()), ambient(_ambient), diffuse(_diffuse)
  {
  }
}
