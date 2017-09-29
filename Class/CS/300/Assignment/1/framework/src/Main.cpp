// TODO(student): copyright for Main.cpp, don't worry about any of the other
// framework files

#include "Precompiled.h"

#include "framework/Application.h"
#include "framework/Debug.h"
#include "graphics/ShaderManager.h"
#include "graphics/ShaderProgram.h"
#include "graphics/VertexArrayObject.h"
#include "graphics/TriangleMesh.h"
#include "graphics/MeshLoader.h"
#include "graphics/Light.h"
#include "graphics/Color.h"
#include "math/Math.h"

static void Initialize(Application *application, void *udata);
static void Update(Application *application, void *udata);
static void Cleanup(Application *application, void *udata);

int main(int argc, char *argv[])
{
  static int const WindowWidth = 800;
  static int const WindowHeight = 600;

  // open the application, initialize it, and start running it
  // TODO(student): rename the application title to something more meaningful
  Application application("CS300 Assignment 1 EXAMPLE",
    WindowWidth, WindowHeight);
  application.Initialize(argc, argv);
  application.Run(Initialize, Update, Cleanup);

  // application has closed, return from main
  return 0;
}

using namespace Graphics;
using namespace Math;

static std::unique_ptr<ShaderManager> shaderManager;
static std::shared_ptr<TriangleMesh> loadedModel;

// TODO(student): this is an example of a file path pointing to the initial
// cube.obj to load; this variable is important for ensuring the text field
// in ImGui that's used to load the model is properly updated; feel free to
// rename it or change the logic as you see fit, it's here to make you aware of
// the need for such a variable
static std::string modelFile = "cube.obj";

// static lights in the scene
static Light light0(Vector4(-1, -1, 0, 0.f), Color::Red, Color::Red);

// TODO(student): create a second light, perhaps something generic to help
// prepare for assignment 2 (2 lights supported for assignment 1,
// 8 for assignment 2)

struct
{
  Color ambient, diffuse;
} Material;

enum NormalDebugMode {
  NONE, VERTEX_NORMALS, FACE_NORMALS,
  DEBUG_MODE_COUNT
};

// helper for createSphere(); not documented well because it is not essential
// for CS300 assignment 1
static inline Vector3 CreateSpherePoint(f32 theta, f32 phi, Vector3 &norm)
{
  f32 sinPhi = sinf(phi);
  norm = Vector3(cosf(theta) * sinPhi, cosf(phi), sinf(theta) * sinPhi);
  // fix radius = 1 and center at (0, 0, 2)
  return norm + Vector3(0, 0, 2.f);
}

// Note: this code is not at all necessary for assignment 1. It is provided to
// demonstrate that the lighting works and to show a more sophisticated example
// of creating a triangle mesh. Feel free to re-use this code in future
// assignments. You can also just test lighting right off of loaded meshes, but
// it might be easier to try and get the entire application working before
// worrying about meshes from the disk.
static TriangleMesh *createSphere()
{
  // Generate base wrapping for a sphere; does not include polar caps
  TriangleMesh *mesh = new TriangleMesh;
  f32 deltaPhi = 0.22439947525641380274733167023425f; // hardcode 12 stacks
  f32 deltaTheta = 0.52359877559829887307710723054658f; // hardcode 12 slices
  f32 phi = deltaPhi;
  // remark: could easily optimize this by not duplicating points
  u32 vertexCount = 0;
  for (u8 y = 0; y < 12; ++y) // stacks represents # of actual strips
  {
    f32 theta = 0.f;
    f32 nextPhi = phi + deltaPhi;
    for (u8 x = 0; x < 12; ++x)
    {
      f32 nextTheta = theta + deltaTheta;
      Vector3 normA, normB, normC, normD;
      Vector3 a = CreateSpherePoint(theta, phi, normA);
      Vector3 b = CreateSpherePoint(nextTheta, phi, normB);
      Vector3 c = CreateSpherePoint(nextTheta, nextPhi, normC);
      Vector3 d = CreateSpherePoint(theta, nextPhi, normD);
      mesh->AddVertex(a.x, a.y, a.z);
      mesh->AddVertex(b.x, b.y, b.z);
      mesh->AddVertex(c.x, c.y, c.z);
      mesh->AddVertex(d.x, d.y, d.z);
      u32 idxA = vertexCount++, idxB = vertexCount++;
      u32 idxC = vertexCount++, idxD = vertexCount++;
      mesh->GetVertex(idxA).normal = normA.Normalized();
      mesh->GetVertex(idxB).normal = normB.Normalized();
      mesh->GetVertex(idxC).normal = normC.Normalized();
      mesh->GetVertex(idxD).normal = normD.Normalized();
      mesh->AddTriangle(idxA, idxB, idxC);
      mesh->AddTriangle(idxA, idxC, idxD);
      theta = nextTheta;
    }
    phi = nextPhi;
  }
  return mesh;
}

// This is the core example used by createSampleMesh to demonstrate how to
// build a basic triangle mesh.
static TriangleMesh *createTriangle()
{
  // TODO(student): look at this example code for hints on implementing mesh
  // loader

  TriangleMesh *mesh = new TriangleMesh;

  // construct vertices for a single triangle
  mesh->AddVertex(0.f, 0.5f, 1.f);
  mesh->AddVertex(-0.5f, -0.5f, 1.f);
  mesh->AddVertex(0.5f, -0.5f, 1.f);
  // stitch these vertices together in CCW order to create a triangle
  mesh->AddTriangle(0, 1, 2);

  // should also call mesh->Preprocess() here, but that will mess up our sample
  // until translation is implemented (will cause the object to be centered
  // about the origin, thereby causing nothing to draw); make sure to call this
  // as a last step in the mesh loading process
  return mesh;
}

static void createSampleMesh()
{
  // TODO(student): use the MeshLoader to load a file based on the filename
  // provided by the user; note that this should initially be 'cube.obj' if no
  // name was provided

  // instead, this function is going to show how to construct a mesh from
  // scratch using the TriangleMesh data structure; this logic is exactly what
  // you should use when loading an OBJ file from the disk

  TriangleMesh *mesh = createTriangle();
  // uncomment this one if you want to see the lighting in action
  //TriangleMesh *mesh = createSphere();

  // wrap the mesh instance in a shared_ptr
  loadedModel = std::shared_ptr<TriangleMesh>(mesh);

  // build the mesh using the example shader
  std::shared_ptr<ShaderProgram> const &program = shaderManager->GetShader(
    ShaderType::EXAMPLE);
  loadedModel->Build(program);
}

static void loadExampleShader()
{
  // load the vertex and fragment shader given by shader.vert and shader.frag;
  // this is the provided EXAMPLE shader included with the framework
  shaderManager->RegisterShader(ShaderType::EXAMPLE,
    "shader.vert", "shader.frag")->Build();
}

void Initialize(Application *application, void *udata)
{
  // initialize material colors to a light ambient (absorb little ambient) and
  // pure white for diffuse (absorb all diffuse from any lights in the scene)
  Material.ambient = Color(0.1f, 0.1f, 0.1f);
  Material.diffuse = Color::White;

  // initialize light colors (initialization done in case Color::Red and
  // Color::Blue is not initialized until after light0 and light1 are; this
  // goes around relying on static initialization order)
  light0.ambient = light0.diffuse = Color::Red;

  // TODO(student): initialize other lights here

  // create the class used to manage shader programs
  shaderManager = std::unique_ptr<ShaderManager>(new ShaderManager);

  glClearColor(0.5f, 0.5f, 0.5f, 1.f); // set background color to medium gray
  glEnable(GL_DEPTH_TEST); // enable the depth buffer and depth testing

  // load all shaders for the application
  loadExampleShader();

  // load the initial model (cube.obj)
  createSampleMesh();
}

static void setupTransformations(Application *application,
  Matrix4 &modelview, Matrix4 &mvp)
{
  ImGui::Separator();
  // model options
  {
    ImGui::PushID("ModelOptions");

    // TODO(student): implement Euler-angles rotation and translation
    float rotationX = 0.f, rotationY = 0.f, rotationZ = 0.f;
    Vector3 position(0.f, 0.f, 0.f);
    ImGui::InputFloat3("Model Position", position.ToFloats());
    ImGui::SliderAngle("Rotation X", &rotationX);
    ImGui::SliderAngle("Rotation Y", &rotationY);
    ImGui::SliderAngle("Rotation Z", &rotationZ);

    if (ImGui::CollapsingHeader("Material"))
    {
      // TODO: feel free to alter this code as you see fit; perhaps you wish to
      // change how materials are being handled; that's completely fine; this
      // is fully implemented just to demonstrate how to use ImGui, since none
      // of the other inputs are saved (minus the model file path input).
      ImGui::ColorEdit3("Ambient", Material.ambient.ToFloats());
      ImGui::ColorEdit3("Diffuse", Material.diffuse.ToFloats());
    }

    ImGui::PopID();
  }

  // TODO(student): create the logic for doing local to world transformations
  modelview.SetIdentity();

  // TODO(student): remember to implement the logic for perspective projection
  // inside Matrix4::CreateProjection; this location is not necessary for the
  // logic (ie, you can place it anywhere), but a work projection matrix will
  // always apply itself to the correct width/height of the application
  Matrix4 proj = Matrix4::CreateProjection(0, 0, 0, 0, 0);
  mvp = proj * modelview; // model-view-projection concatenated matrix
}

static void enableSampleLights(std::shared_ptr<ShaderProgram> program)
{
  // TODO(student): consider reimplementing this method to be more generic,
  // rather than hardcoding for 2 lights; it will help for assignment 2

  // light options
  ImGui::Separator();

  // PushID/PopID ensures that each of the inputs for lights do not conflict
  // with each other; 'Position' below stores temporary state based on that
  // name, so each element in this window by name 'Position' could potentially
  // conflict; this stack of IDs helps prevent that

  // Light 0
  ImGui::PushID("Light 0");
  if (ImGui::CollapsingHeader("Light 0"))
  {
    // TODO(student): implement handling inputs for lights
    float dummy1[3] = { 0.f }, dummy2[3] = { 0.f }, dummy3[3] = { 0.f };
    ImGui::InputFloat3("Direction", dummy1);
    ImGui::ColorEdit3("Ambient", dummy2);
    ImGui::ColorEdit3("Diffuse", dummy3);
  }
  ImGui::PopID();

  // set light 0's properties on shader
  program->SetUniform("Lights[0].ambient", light0.ambient);
  // TODO(student): set light 0's other properties, too

  // TODO(student): implement ImGui and uniform logic for light 1

  // '1U' used to differentiate from a float; activate 1 light in the vertex
  // shader
  program->SetUniform("LightCount", 1U); // TODO(student): support 2 lights
}

void Update(Application *application, void *udata)
{
  // clear the pixel and depth buffers for this frame
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // create a window entitled "Sample Window"
  // TODO(student): rename this to be something more appropriate
  ImGui::Begin("Sample Window");

  // import options
  {
    // this code is already setup to copy the model file name into a local C
    // buffer, allow ImGui to alter it, and then immediately update the file
    // name based on the changes; feel free to change this logic as you see fit
    char fileNameBuffer[140] = { '\0' };
    std::strcat(fileNameBuffer, modelFile.c_str());
    if (ImGui::InputText("Model", fileNameBuffer, sizeof(fileNameBuffer)))
    {
      // text was changed; copy back over to C++ string
      modelFile = fileNameBuffer;
    }

    if (ImGui::Button("Load Model"))
    {
      // TODO(student): implement loading models from a file; you can use the
      // string 'modelFile' which should store the updated file name from ImGui
    }

    if (ImGui::Button("Reload Shaders"))
    {
      // we could implement reloading shaders here, but not necessary for
      // assignment 1
    }

    // TODO(student): implement GUI logic for enabling debug drawing modes
    int debugMode = NONE;
    std::vector<char const *> debugModeStrings = {
      "None", "VertexNormals", "FaceNormals" };
    ImGui::Combo("Debug Mode", &debugMode, debugModeStrings.data(),
      DEBUG_MODE_COUNT);
  }

  Matrix4 modelview, mvp;
  setupTransformations(application, modelview, mvp);

  // Render
  if (shaderManager)
  {
    auto &program = shaderManager->GetShader(ShaderType::EXAMPLE);
    program->Bind();

    // transforms
    program->SetUniform("ModelViewMatrix", modelview);
    program->SetUniform("ModelViewProjectionMatrix", mvp);

    // material
    program->SetUniform("Material.ambient", Material.ambient);
    // TODO(ben): implement uploading other material properties here; ambient
    // should fully work (try dragging the red channel on the material ambient
    // field in ImGui; it's the only one that should have a visual differenc)

    // lights
    enableSampleLights(program);

    if (loadedModel)
    {
      loadedModel->Render();

      // disable lights for debug drawing, leading to black colors for lines
      program->SetUniform("LightCount", 0U);

      // TODO(student): implement drawing debug face normals
      // TODO(student): implement drawing debug vertex normals
    }

    program->Unbind();
  }

  // done creating ImGui components
  ImGui::End();
}

void Cleanup(Application *application, void *udata)
{
  // cleanup OpenGL resources and allocated memory
  shaderManager = NULL; // delete all programs
  // delete all meshes
  loadedModel = NULL;
}
