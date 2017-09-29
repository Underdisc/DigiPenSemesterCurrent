#include "Precompiled.h"
#include "framework/Debug.h"
#include "framework/Utilities.h"
#include "graphics/MeshLoader.h"
#include "graphics/TriangleMesh.h"

namespace Graphics
{
  std::shared_ptr<TriangleMesh> MeshLoader::LoadMesh(std::string const &objFile)
  {
    std::stringstream strstr;
    strstr << ASSET_PATH << "models/" << objFile;
    std::ifstream input = std::ifstream(strstr.str());
    Assert(input.good(), "Cannot load mesh: assets/models/%s", objFile.c_str());
    if (!input.good())
      return NULL;

    // TODO(student): implement a Wavefront OBJ loader in this file, assemble
    // an instance of TriangleMesh using the data read in, and return it

    // NOTE: the 'input' stream above is ready to being reading the text lines
    // of OBJ files; it already is setup to correctly find the object file
    // under the assets/models directory.

    return NULL;
  }
}
