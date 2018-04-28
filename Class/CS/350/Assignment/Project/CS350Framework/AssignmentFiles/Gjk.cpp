///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//-----------------------------------------------------------------------------SupportShape
Vector3 SupportShape::GetCenter(const std::vector<Vector3>& localPoints, const Matrix4& transform) const
{
  Vector3 center = Vector3::cZero;
  /******Student:Assignment5******/
  Warn("Assignment5: Required function un-implemented");
  return center;
}

Vector3 SupportShape::Support(const Vector3& worldDirection, const std::vector<Vector3>& localPoints, const Matrix4& localToWorldTransform) const
{
  Vector3 result = Vector3::cZero;
  /******Student:Assignment5******/
  Warn("Assignment5: Required function un-implemented");
  return result;
}

void SupportShape::DebugDraw(const std::vector<Vector3>& localPoints, const Matrix4& localToWorldTransform, const Vector4& color) const
{
  /******Student:Assignment5******/
  Warn("Assignment5: Required function un-implemented");
}

//-----------------------------------------------------------------------------ModelSupportShape
Vector3 ModelSupportShape::GetCenter() const
{
  return SupportShape::GetCenter(mModel->mMesh->mVertices, mModel->mOwner->has(Transform)->GetTransform());
}

Vector3 ModelSupportShape::Support(const Vector3& worldDirection) const
{
  return SupportShape::Support(worldDirection, mModel->mMesh->mVertices, mModel->mOwner->has(Transform)->GetTransform());
}

void ModelSupportShape::DebugDraw(const Vector4& color) const
{
  SupportShape::DebugDraw(mModel->mMesh->mVertices, mModel->mOwner->has(Transform)->GetTransform());
}

//-----------------------------------------------------------------------------PointsSupportShape
PointsSupportShape::PointsSupportShape()
{
  mScale = Vector3(1);
  mRotation = Matrix3::cIdentity;
  mTranslation = Vector3::cZero;
}

Vector3 PointsSupportShape::GetCenter() const
{
  Matrix4 transform = Math::BuildTransform(mTranslation, mRotation, mScale);
  return SupportShape::GetCenter(mLocalSpacePoints, transform);
}

Vector3 PointsSupportShape::Support(const Vector3& worldDirection) const
{
  Matrix4 transform = Math::BuildTransform(mTranslation, mRotation, mScale);
  return SupportShape::Support(worldDirection, mLocalSpacePoints, transform);
}

void PointsSupportShape::DebugDraw(const Vector4& color) const
{
  Matrix4 transform = Math::BuildTransform(mTranslation, mRotation, mScale);
  SupportShape::DebugDraw(mLocalSpacePoints, transform, color);
}

//-----------------------------------------------------------------------------SphereSupportShape
Vector3 SphereSupportShape::GetCenter() const
{
  return mSphere.mCenter;
}

Vector3 SphereSupportShape::Support(const Vector3& worldDirection) const
{
  /******Student:Assignment5******/
  Warn("Assignment5: Required function un-implemented");
  return Vector3::cZero;
}

void SphereSupportShape::DebugDraw(const Vector4& color) const
{
  DebugShape& shape = gDebugDrawer->DrawSphere(mSphere);
  shape.Color(color);
}

//-----------------------------------------------------------------------------ObbSupportShape
Vector3 ObbSupportShape::GetCenter() const
{
  return mTranslation;
}

Vector3 ObbSupportShape::Support(const Vector3& worldDirection) const
{
  /******Student:Assignment5******/
  // Note: A unit obb spans from [-0.5, to 0.5]. Make sure to properly account for this.
  Warn("Assignment5: Required function un-implemented");
  return Vector3::cZero;
}

void ObbSupportShape::DebugDraw(const Vector4& color) const
{
  Matrix4 transform = Math::BuildTransform(mTranslation, mRotation, mScale);
  DebugShape& shape = gDebugDrawer->DrawAabb(Aabb(Vector3(-0.5f), Vector3(0.5f)));
  shape.Color(color);
  shape.SetTransform(transform);
}

//------------------------------------------------------------ Voronoi Region Tests
VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, 
  const Vector3& p0, size_t& newSize, int newIndices[4], 
  Vector3& closestPoint, Vector3& searchDirection)
{
  newSize = 1;
  newIndices[0] = 0;
  closestPoint = p0;
  searchDirection = q - p0;
  return VoronoiRegion::Point0;
}

VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, 
  const Vector3& p0, const Vector3& p1, size_t& newSize, int newIndices[4],
  Vector3& closestPoint, Vector3& searchDirection)
{
  float u, v;
  BarycentricCoordinates(q, p0, p1, u, v);
  VoronoiRegion::Type region;
  if (v <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 0;
    closestPoint = p0;
    region = VoronoiRegion::Point0;
  }
  else if(u <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 1;
    closestPoint = p1;
    region = VoronoiRegion::Point1;
  }
  else
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 1;
    closestPoint = u * p0 + v * p1;
    region = VoronoiRegion::Edge01;
  }
  searchDirection = q - closestPoint;
  return region;
}

VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, const Vector3& p0, const Vector3& p1, const Vector3& p2,
  size_t& newSize, int newIndices[4],
  Vector3& closestPoint, Vector3& searchDirection)
{
  // handle triangle point regions first
  // edge [0, 2]
  float u0, v0, u1, v1, u2, v2;
  BarycentricCoordinates(q, p0, p1, u0, v0);
  BarycentricCoordinates(q, p1, p2, u1, v1);
  BarycentricCoordinates(q, p2, p0, u2, v2);
  // point 0 region
  if (v0 <= 0.0f && u2 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 0;
    closestPoint = p0;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point0;
  }
  // point 1 region
  if (v1 <= 0.0f && u0 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 1;
    closestPoint = p1;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point1;
  }
  // point 2 region
  if (v2 <= 0.0f && u1 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 2;
    closestPoint = p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point2;
  }
  // edge and triangle regions
  float u, v, w;
  BarycentricCoordinates(q, p0, p1, p2, u, v, w);
  if (u <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 1;
    newIndices[1] = 2;
    closestPoint = u1 * p1 + v1 * p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge12;
  }
  if (v <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 2;
    closestPoint = u2 * p2 + v2 * p0;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge02;
  }
  if (w <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 1;
    closestPoint = u0 * p0 + v0 * p1;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge01;
  }
  newSize = 3;
  for (int i = 0; i < 3; ++i)
  {
    newIndices[i] = i;
  }
  closestPoint = q;
  searchDirection = q - closestPoint;
  return VoronoiRegion::Triangle012;
}

VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3,
  size_t& newSize, int newIndices[4],
  Vector3& closestPoint, Vector3& searchDirection)
{
  /******Student:Assignment5******/
  Warn("Assignment5: Required function un-implemented");
  return VoronoiRegion::Unknown;
}

Gjk::Gjk()
{
}

bool Gjk::Intersect(const SupportShape* shapeA, const SupportShape* shapeB, unsigned int maxIterations, CsoPoint& closestPoint, float epsilon, int debuggingIndex, bool debugDraw)
{
  Warn("Assignment5: Required function un-implemented");
  return false;
}

Gjk::CsoPoint Gjk::ComputeSupport(const SupportShape* shapeA, const SupportShape* shapeB, const Vector3& direction)
{
  /******Student:Assignment5******/
  CsoPoint result;
  Warn("Assignment5: Required function un-implemented");

  return result;
}
