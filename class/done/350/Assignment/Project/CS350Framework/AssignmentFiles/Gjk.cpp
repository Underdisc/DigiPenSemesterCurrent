///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//-----------------------------------------------------------------SupportShape
Vector3 SupportShape::GetCenter(const std::vector<Vector3>& localPoints, 
  const Matrix4& transform) const
{
  // use aabb to find center
  Aabb aabb;
  aabb.Compute(localPoints);
  Vector3 aabb_center = aabb.GetCenter();
  Vector4 center = Vector4(aabb_center.x, aabb_center.y, aabb_center.z, 1.0f);
  // transform center
  Vector4 t_center = Math::Transform(transform, center);
  return Vector3(t_center.x, t_center.y, t_center.z);
}

Vector3 SupportShape::Support(const Vector3& worldDirection, 
  const std::vector<Vector3>& localPoints, 
  const Matrix4& localToWorldTransform) const
{
  // compute direction in local space
  const Matrix4& t = localToWorldTransform;
  Matrix3 rotation;
  rotation.SetBasis(0, t.Basis3X());
  rotation.SetBasis(1, t.Basis3Y());
  rotation.SetBasis(2, t.Basis3Z());
  (t.Basis3X(), t.Basis3Y(), t.Basis3Z());
  Vector3 d = Math::Transform(rotation.Inverted(), worldDirection);
  d = d.Normalized();
  // find furthest point in direction
  const Vector3 * furthest = &(localPoints[0]);
  float furthest_dist = Math::Dot(d, localPoints[0]);
  unsigned num_points = localPoints.size();
  for (unsigned i = 1; i < num_points; ++i)
  {
    float dist = Math::Dot(d, localPoints[i]);
    if (dist > furthest_dist)
    {
      furthest = &localPoints[i];
      furthest_dist = dist;
    }
  }
  // transform furthest point
  Vector4 furthest_final = Vector4(furthest->x, furthest->y, furthest->z, 
    1.0f);
  furthest_final = Math::Transform(localToWorldTransform, furthest_final);
  return Vector3(furthest_final.x, furthest_final.y, furthest_final.z);
}

void SupportShape::DebugDraw(const std::vector<Vector3>& localPoints, 
  const Matrix4& localToWorldTransform, const Vector4& color) const
{
  unsigned num_points = localPoints.size();
  for (unsigned i = 0; i < num_points; ++i)
  {
    DebugShape & draw_shape = gDebugDrawer->DrawPoint(localPoints[i]);
    draw_shape.SetTransform(localToWorldTransform);
    draw_shape.Color(color);
  }
}

//------------------------------------------------------------ModelSupportShape
Vector3 ModelSupportShape::GetCenter() const
{
  return SupportShape::GetCenter(mModel->mMesh->mVertices, 
    mModel->mOwner->has(Transform)->GetTransform());
}

Vector3 ModelSupportShape::Support(const Vector3& worldDirection) const
{
  return SupportShape::Support(worldDirection, mModel->mMesh->mVertices, 
    mModel->mOwner->has(Transform)->GetTransform());
}

void ModelSupportShape::DebugDraw(const Vector4& color) const
{
  SupportShape::DebugDraw(mModel->mMesh->mVertices, 
    mModel->mOwner->has(Transform)->GetTransform());
}

//-----------------------------------------------------------PointsSupportShape
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

//-----------------------------------------------------------SphereSupportShape
Vector3 SphereSupportShape::GetCenter() const
{
  return mSphere.mCenter;
}

Vector3 SphereSupportShape::Support(const Vector3& worldDirection) const
{
  Vector3 furthest;
  Vector3 direction_normalized = worldDirection.Normalized();
  furthest = mSphere.mCenter + direction_normalized * mSphere.mRadius;
  return furthest;
}

void SphereSupportShape::DebugDraw(const Vector4& color) const
{
  DebugShape& shape = gDebugDrawer->DrawSphere(mSphere);
  shape.Color(color);
}

//--------------------------------------------------------------ObbSupportShape
Vector3 ObbSupportShape::GetCenter() const
{
  return mTranslation;
}

Vector3 ObbSupportShape::Support(const Vector3& worldDirection) const
{
  // find local direction
  Vector3 local_dir = Math::Transform(mRotation.Inverted(), worldDirection);
  local_dir = local_dir.Normalized();
  // compute half extents
  Vector3 half_extent = mScale / 2.0f;
  // find furthest point
  Vector3 furthest = Vector3::cZero;
  for (unsigned i = 0; i < 3; ++i)
  {
    furthest += Math::GetSign(local_dir[i]) * half_extent[i] * 
      mRotation.Basis(i);
  }
  return furthest + mTranslation;

}

void ObbSupportShape::DebugDraw(const Vector4& color) const
{
  Matrix4 transform = Math::BuildTransform(mTranslation, mRotation, mScale);
  DebugShape & shape = gDebugDrawer->DrawAabb(Aabb(Vector3(-0.5f), 
    Vector3(0.5f)));
  shape.Color(color);
  shape.SetTransform(transform);
}

//-------------------------------------------------------- Voronoi Region Tests


VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, 
  const Vector3& p0, 
  size_t& newSize, int newIndices[4], 
  Vector3& closestPoint, Vector3& searchDirection)
{
  newSize = 1;
  newIndices[0] = 0;
  closestPoint = p0;
  searchDirection = q - p0;
  return VoronoiRegion::Point0;
}

VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, 
  const Vector3& p0, const Vector3& p1, 
  size_t& newSize, int newIndices[4],
  Vector3& closestPoint, Vector3& searchDirection)
{
  float u, v;
  BarycentricCoordinates(q, p0, p1, u, v);
  VoronoiRegion::Type region;
  // check point regions
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

VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, 
  const Vector3& p0, const Vector3& p1, const Vector3& p2,
  size_t& newSize, int newIndices[4],
  Vector3& closestPoint, Vector3& searchDirection)
{
  // edge [0, 2]
  // point regions
  float u0, v0, u1, v1, u2, v2;
  BarycentricCoordinates(q, p0, p1, u0, v0);
  BarycentricCoordinates(q, p1, p2, u1, v1);
  BarycentricCoordinates(q, p2, p0, u2, v2);
  if (v0 <= 0.0f && u2 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 0;
    closestPoint = p0;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point0;
  }
  if (v1 <= 0.0f && u0 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 1;
    closestPoint = p1;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point1;
  }
  if (v2 <= 0.0f && u1 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 2;
    closestPoint = p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point2;
  }
  // check edge regions
  float u, v, w;
  BarycentricCoordinates(q, p0, p1, p2, u, v, w);
  if (u <= 0.0f && u1 > 0.0f && v1 > 0.0f)
  {
    newSize = 2;
    newIndices[0] = 1;
    newIndices[1] = 2;
    closestPoint = u1 * p1 + v1 * p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge12;
  }
  if (v <= 0.0f && u2 > 0.0f && v2 > 0.0f)
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 2;
    closestPoint = u2 * p2 + v2 * p0;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge02;
  }
  if (w <= 0.0f && u0 > 0.0f && v0 > 0.0f)
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 1;
    closestPoint = u0 * p0 + v0 * p1;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge01;
  }
  // in triangle region
  newSize = 3;
  for (int i = 0; i < 3; ++i)
  {
    newIndices[i] = i;
  }
  closestPoint = u * p0 + v * p1 + w * p2;
  searchDirection = q - closestPoint;
  return VoronoiRegion::Triangle012;
}

// finds if a point is on the positive or negative side of a triangle
bool OnPositiveSide(const Vector3 & q, const Vector3 & a, const Vector3 & b,
  const Vector3 & c, const Vector3 & d)
{
  Vector3 normal = Math::Cross(b - a, c - a);
  if(Math::Dot(normal, d - a) > 0.0f)
    normal *= -1.0f;
  float dot_result = Math::Dot(q - a, normal);
  if(dot_result >= 0.0f)
    return true;
  return false;
}

VoronoiRegion::Type Gjk::IdentifyVoronoiRegion(const Vector3& q, 
  const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3,
  size_t& newSize, int newIndices[4],
  Vector3& closestPoint, Vector3& searchDirection)
{
  // check all point regions
  float u01, v01, u02, v02, u03, v03;
  float u12, v12, u13, v13;
  float u23, v23;
  BarycentricCoordinates(q, p0, p1, u01, v01);
  BarycentricCoordinates(q, p0, p2, u02, v02);
  BarycentricCoordinates(q, p0, p3, u03, v03);
  BarycentricCoordinates(q, p1, p2, u12, v12);
  BarycentricCoordinates(q, p1, p3, u13, v13);
  BarycentricCoordinates(q, p2, p3, u23, v23);
  if (v01 <= 0.0f && v02 <= 0.0f && v03 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 0;
    closestPoint = p0;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point0;
  }
  if (u01 <= 0.0f && v12 <= 0.0f && v13 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 1;
    closestPoint = p1;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point1;
  }
  if (u02 <= 0.0f && u12 <= 0.0f && v23 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 2;
    closestPoint = p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point2;
  }
  if (u03 <= 0.0f && u13 <= 0.0f && u23 <= 0.0f)
  {
    newSize = 1;
    newIndices[0] = 3;
    closestPoint = p3;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Point3;
  }
  // check all edge regions
  float u012, v012, w012;
  float u031, v031, w031;
  float u023, v023, w023;
  float u132, v132, w132;
  BarycentricCoordinates(q, p0, p1, p2, u012, v012, w012);
  BarycentricCoordinates(q, p0, p3, p1, u031, v031, w031);
  BarycentricCoordinates(q, p0, p2, p3, u023, v023, w023);
  BarycentricCoordinates(q, p1, p3, p2, u132, v132, w132);
  if (u01 > 0.0f && v01 > 0.0f && v031 <= 0.0f && w012 <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 1;
    closestPoint = u01 * p0 + v01 * p1;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge01;
  }
  if (u02 > 0.0f && v02 > 0.0f && v012 <= 0.0f && w023 <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 2;
    closestPoint = u02 * p0 + v02 * p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge02;
  }
  if (u03 > 0.0f && v03 > 0.0f && w031 <= 0.0f && v023 <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 0;
    newIndices[1] = 3;
    closestPoint = u03 * p0 + v03 * p3;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge03;
  }
  if (u12 > 0.0f && v12 > 0.0f && u012 <= 0.0f && v132 <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 1;
    newIndices[1] = 2;
    closestPoint = u12 * p1 + v12 * p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge12;
  }
  if (u13 > 0.0f && v13 > 0.0f && u031 <= 0.0f && w132 <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 1;
    newIndices[1] = 3;
    closestPoint = u13 * p1 + v13 * p3;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge13;
  }
  if (u23 > 0.0f && v23 > 0.0f && u023 <= 0.0f && u132 <= 0.0f)
  {
    newSize = 2;
    newIndices[0] = 2;
    newIndices[1] = 3;
    closestPoint = u23 * p2 + v23 * p3;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Edge23;
  }
  // check all triangle regions
  bool positive;
  positive = OnPositiveSide(q, p0, p1, p2, p3);
  if (positive && u012 > 0.0f && v012 > 0.0f && w012 > 0.0f)
  {
    newSize = 3;
    newIndices[0] = 0;
    newIndices[1] = 1;
    newIndices[2] = 2;
    closestPoint = u012 * p0 + v012 * p1 + w012 * p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Triangle012;
  }
  positive = OnPositiveSide(q, p0, p3, p1, p2);
  if (positive && u031 > 0.0f  && v031 > 0.0f && w031 > 0.0f)
  {
    newSize = 3;
    newIndices[0] = 0;
    newIndices[1] = 1;
    newIndices[2] = 3;
    closestPoint = u031 * p0 + v031 * p3 + w031 * p1;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Triangle013;
  }
  positive = OnPositiveSide(q, p0, p2, p3, p1);
  if (positive && u023 > 0.0f && v023 > 0.0f && w023 > 0.0f)
  {
    newSize = 3;
    newIndices[0] = 0;
    newIndices[1] = 2;
    newIndices[2] = 3;
    closestPoint = u023 * p0 + v023 * p2 + w023 * p3;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Triangle023;
  }
  positive = OnPositiveSide(q, p1, p3, p2, p0);
  if (positive && u132 > 0.0f && v132 > 0.0f && w132 > 0.0f)
  {
    newSize = 3;
    newIndices[0] = 1;
    newIndices[1] = 2;
    newIndices[2] = 3;
    closestPoint = u132 * p1 + v132 * p3 + w132 * p2;
    searchDirection = q - closestPoint;
    return VoronoiRegion::Triangle123;
  }
  // inside of the tetrahedron
  newSize = 4;
  for (unsigned i = 0; i < 4; ++i)
  {
    newIndices[i] = i;
  }
  closestPoint = q;
  searchDirection = q - closestPoint;
  return VoronoiRegion::Tetrahedra0123;
}

Gjk::Gjk()
{
}

bool Gjk::Intersect(const SupportShape* shapeA, const SupportShape* shapeB, 
  unsigned int maxIterations, CsoPoint& closestPoint, float epsilon, 
  int debuggingIndex, bool debugDraw)
{
  // find initial search direction
  Vector3 q  = Vector3::cZero;
  Vector3 center_a = shapeA->GetCenter();
  Vector3 center_b = shapeB->GetCenter();
  Vector3 search_direction = center_a - center_b;
  if (search_direction == Vector3::cZero)
  {
    search_direction = Vector3(-1.0f, 0.0f, 0.0f);
  }
  // begin iterative gjk computation
  unsigned simplex_size = 0;
  int indices[4];
  CsoPoint p[4];
  Vector3 closest_point = Vector3(Math::PositiveMax());
  CsoPoint new_simplex_point = ComputeSupport(shapeA, shapeB, 
    search_direction);
  for (unsigned i = 0; i < maxIterations; ++i)
  {
    // add new simplex point
    p[simplex_size] = new_simplex_point;
    ++simplex_size;
    // uncomment for debug drawing
    /*if (debuggingIndex == i && debugDraw)
    {
      for (unsigned j = 0; j < simplex_size; ++j)
      {
        DebugShape & red_point = gDebugDrawer->DrawPoint(p[j].mPointA);
        red_point.Color(Vector4(1.0f, 0.0f, 0.0f, 0.0f));
        DebugShape & green_point = gDebugDrawer->DrawPoint(p[j].mPointB);
        green_point.Color(Vector4(0.0f, 1.0f, 0.0f, 0.0f));
        for (unsigned k = j + 1; k < simplex_size; ++k)
        {
          LineSegment red_segment;
          red_segment.mStart = p[j].mPointA;
          red_segment.mEnd = p[k].mPointA;
          DebugShape & red_line = gDebugDrawer->DrawLine(red_segment);
          red_line.Color(Vector4(1.0f, 0.0f, 0.0f, 0.0f));
          LineSegment green_segment;
          green_segment.mStart = p[j].mPointB;
          green_segment.mEnd = p[k].mPointB;
          DebugShape & green_line = gDebugDrawer->DrawLine(green_segment);
          green_line.Color(Vector4(0.0f, 1.0f, 0.0f, 0.0f));
        }
      }
    }*/
    // find voronoi region
    Vector3 previous_closest_point =  closest_point;
    switch (simplex_size)
    {
    case 1:
      IdentifyVoronoiRegion(q, p[0].mCsoPoint, simplex_size,
        indices, closest_point, search_direction);
      break;
    case 2:
      IdentifyVoronoiRegion(q, p[0].mCsoPoint, p[1].mCsoPoint,
        simplex_size, indices, closest_point, search_direction);
      break;
    case 3:
      IdentifyVoronoiRegion(q, p[0].mCsoPoint, p[1].mCsoPoint,
        p[2].mCsoPoint, simplex_size, indices, closest_point,
        search_direction);
      break;
    case 4:
      IdentifyVoronoiRegion(q, p[0].mCsoPoint, p[1].mCsoPoint,
        p[2].mCsoPoint, p[3].mCsoPoint, simplex_size, indices, closest_point,
        search_direction);
      break;
    }
    // check for collision
    // NOTE: Please look at the bottom of this file to find out why I am
    // using the the condition below and why I believe it works.
    if (q == closest_point || previous_closest_point == closest_point)
    {
      return true;
    }
    // update cso points
    for (unsigned j = 0; j < simplex_size; ++j)
    {
      p[j] = p[indices[j]];
    }
    // get new simplex point
    new_simplex_point = ComputeSupport(shapeA, shapeB, search_direction);
    // check for little progress
    Vector3 progress_vec = new_simplex_point.mCsoPoint - closest_point;
    search_direction = search_direction.Normalized();
    float progress_made = Math::Dot(progress_vec, search_direction);
    if (progress_made <= epsilon)
    {
      break;
    }
  }
  // no collision
  // compute closest cso points
  float u, v, w;
  closestPoint.mCsoPoint = closest_point;
  switch (simplex_size)
  {
  case 1:
    closestPoint.mPointA = p[0].mPointA;
    closestPoint.mPointB = p[0].mPointB;
    break;
  case 2:
    BarycentricCoordinates(closest_point, p[0].mCsoPoint, p[1].mCsoPoint, 
      u, v);
    closestPoint.mPointA = u * p[0].mPointA + v * p[1].mPointA;
    closestPoint.mPointB = u * p[0].mPointB + v * p[1].mPointB;
    break;
  case 3:
    BarycentricCoordinates(closest_point, p[0].mCsoPoint, p[1].mCsoPoint,
      p[2].mCsoPoint, u, v, w);
    closestPoint.mPointA = u * p[0].mPointA + v * p[1].mPointA + 
      w * p[2].mPointA;
    closestPoint.mPointB = u * p[0].mPointB + v * p[1].mPointB + 
      w * p[2].mPointB;
    break;
  }
  return false;
}

Gjk::CsoPoint Gjk::ComputeSupport(const SupportShape* shapeA, 
  const SupportShape* shapeB, const Vector3& direction)
{
  // compute support and cso point
  CsoPoint cso_point;
  cso_point.mPointA = shapeA->Support(direction);
  cso_point.mPointB = shapeB->Support(-direction);
  cso_point.mCsoPoint = cso_point.mPointA - cso_point.mPointB;
  return cso_point;
}

/*////////////////////////////////////////////////////////////////////////////
There was an issue with GjkFuzzTest78 when only the following condition 
was used.

if (q == closest_point)
{
  return true;
}

q is a zero vector. This was failing due to the fact that closest_point
was never actually reaching zero, but it got very close to reaching zero.
With a value very close to zero, the search direction for the next simplex
point had very little meaning. In fact, the search direction used caused
the lower condition that checks for no progress to fail repeatedly. Each time
a new simplex point was added, it seemed like progress was made, but in
reality, the same simplex point was just getting repeatedly added and
removed. Even though it looked like there was progress, there was actually none
at all.

However, the addition of the following works.

if (q == closest_point || previous_closest_point == closest_point)
{
  return true;
}

Essentially, if our closest point hasn't changed, there must be a collision.
This works because the addition of the new simplex point and check for no 
progress happens later in the loop. If a simplex point is added that provides 
no progress, then there is no collision. If this check was not there, the above
condition would be asserted during the next iteration of the loop.
/*////////////////////////////////////////////////////////////////////////////