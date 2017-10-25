
#include "ShaderLibrary.h"

LineShader::LineShader() : 
  Shader("Resource/Shader/line.vert", "Resource/Shader/line.frag")
{
  APosition = GetAttribLocation("APosition");
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  ULineColor = GetUniformLocation("ULineColor");
}

PhongShader::PhongShader() :
  Shader("Resource/Shader/phong.vert", "Resource/Shader/phong.frag")
{
  // finding attributes
  APosition = GetAttribLocation("APosition");
  ANormal = GetAttribLocation("ANormal");
  // finding uniforms
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UCameraPosition = GetUniformLocation("UCameraPosition");
  // finding material uniforms
  UMaterial.UColor = GetUniformLocation("UMaterial.UColor");
  UMaterial.UAmbientFactor = GetUniformLocation("UMaterial.UAmbientFactor");
  UMaterial.UDiffuseFactor = GetUniformLocation("UMaterial.UDiffuseFactor");
  UMaterial.USpecularFactor = GetUniformLocation("UMaterial.USpecularFactor");
  UMaterial.USpecularExponent = GetUniformLocation("UMaterial.USpecularExponent");
  // finding light uniforms
  for (unsigned int i = 0; i < MAXLIGHTS; ++i) {
    std::string index(std::to_string(i));
    ULights[i].UType = GetUniformLocation(
      "ULights[" + index + "].UType");
    ULights[i].UPosition = GetUniformLocation(
      "ULights[" + index + "].UPosition");
    ULights[i].UDirection = GetUniformLocation(
      "ULights[" + index + "].UDirection");
    ULights[i].UAmbientColor = GetUniformLocation(
      "ULights[" + index + "].UAmbientColor");
    ULights[i].UDiffuseColor = GetUniformLocation(
      "ULights[" + index + "].UDiffuseColor");
    ULights[i].USpecularColor = GetUniformLocation(
      "ULights[" + index + "].USpecularColor");
  }
  UActiveLights = GetUniformLocation("UActiveLights");
  // finding fog uniforms
  UFogColor = GetUniformLocation("UFogColor");
  UNearPlane = GetUniformLocation("UNearPlane");
  UFarPlane = GetUniformLocation("UFarPlane");
}
void PhongShader::EnableAttributes()
{
  glVertexAttribPointer(APosition, 3, GL_FLOAT, GL_FALSE,
    6 * sizeof(GLfloat), nullptr);
  glVertexAttribPointer(ANormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
    (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(APosition);
  glEnableVertexAttribArray(ANormal);
}
void PhongShader::DisableAttributes()
{
  glDisableVertexAttribArray(APosition);
  glDisableVertexAttribArray(ANormal);
}

GouraudShader::GouraudShader() :
  Shader("Resource/Shader/gouraud.vert", "Resource/Shader/gouraud.frag")
{
  // finding attributes
  APosition = GetAttribLocation("APosition");
  ANormal = GetAttribLocation("ANormal");
  // finding uniforms
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UObjectColor = GetUniformLocation("UObjectColor");
  UActiveLights = GetUniformLocation("UActiveLights");
  // finding light uniforms
  for (unsigned int i = 0; i < MAXLIGHTS; ++i) {
    std::string index(std::to_string(i));
    ULights[i].UPosition = GetUniformLocation(
      "ULights[" + index + "].UPosition");
    ULights[i].UAmbientFactor = GetUniformLocation(
      "ULights[" + index + "].UAmbientFactor");
    ULights[i].UDiffuseFactor = GetUniformLocation(
      "ULights[" + index + "].UDiffuseFactor");
    ULights[i].USpecularFactor = GetUniformLocation(
      "ULights[" + index + "].USpecularFactor");
    ULights[i].USpecularExponent = GetUniformLocation(
      "ULights[" + index + "].USpecularExponent");
    ULights[i].UAmbientColor = GetUniformLocation(
      "ULights[" + index + "].UAmbientColor");
    ULights[i].UDiffuseColor = GetUniformLocation(
      "ULights[" + index + "].UDiffuseColor");
    ULights[i].USpecularColor = GetUniformLocation(
      "ULights[" + index + "].USpecularColor");
  }
}
void GouraudShader::EnableAttributes()
{
  glVertexAttribPointer(APosition, 3, GL_FLOAT, GL_FALSE,
    6 * sizeof(GLfloat), nullptr);
  glVertexAttribPointer(ANormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
    (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(APosition);
  glEnableVertexAttribArray(ANormal);
}
void GouraudShader::DisableAttributes()
{
  glDisableVertexAttribArray(APosition);
  glDisableVertexAttribArray(ANormal);
}

TextureShader::TextureShader() :
  Shader("Resource/Shader/texture.vert", "Resource/Shader/texture.frag")
{
  APosition = GetAttribLocation("APosition");
  ATexCoord = GetAttribLocation("ATexCoord");
  UProjection = GetUniformLocation("UProjection");
  UView = GetUniformLocation("UView");
  UModel = GetUniformLocation("UModel");
  UTexture = GetUniformLocation("UTexture");
}