///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//-----------------------------------------------------------------------------LineSegment
LineSegment::LineSegment()
{
  mStart = mEnd = Vector3::cZero;
}

LineSegment::LineSegment(Math::Vec3Param start, Math::Vec3Param end)
{
  mStart = start;
  mEnd = end;
}

DebugShape& LineSegment::DebugDraw() const
{
  return gDebugDrawer->DrawLine(*this);
}

//-----------------------------------------------------------------------------Ray
Ray::Ray()
{
  mStart = mDirection = Vector3::cZero;
}

Ray::Ray(Math::Vec3Param start, Math::Vec3Param dir)
{
  mStart = start;
  mDirection = dir;
}

Ray Ray::Transform(const Math::Matrix4& transform) const
{
  Ray transformedRay;
  transformedRay.mStart = Math::TransformPoint(transform, mStart);
  transformedRay.mDirection = Math::TransformDirection(transform, mDirection);
  return transformedRay;
}

Vector3 Ray::GetPoint(float t) const
{
  return mStart + mDirection * t;
}

DebugShape& Ray::DebugDraw(float t) const
{
  return gDebugDrawer->DrawRay(*this, t);
}

//-----------------------------------------------------------------------------PCA Helpers
Matrix3 ComputeCovarianceMatrix(const std::vector<Vector3>& points)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
  return Matrix3::cIdentity;
}

Matrix3 ComputeJacobiRotation(const Matrix3& matrix)
{
  /******Student:Assignment2******/
  // Compute the jacobi rotation matrix that will turn the largest (magnitude) off-diagonal element of the input
  // matrix into zero. Note: the input matrix should always be (near) symmetric.
  Warn("Assignment2: Required function un-implemented");
  return Matrix3::cIdentity;
}

void ComputeEigenValuesAndVectors(const Matrix3& covariance, Vector3& eigenValues, Matrix3& eigenVectors, int maxIterations)
{
  /******Student:Assignment2******/
  // Iteratively rotate off the largest off-diagonal elements until the resultant matrix is diagonal or maxIterations.
  Warn("Assignment2: Required function un-implemented");
}


//-----------------------------------------------------------------------------Sphere
Sphere::Sphere()
{
  mCenter = Vector3::cZero;
  mRadius = 0;
}

Sphere::Sphere(const Vector3& center, float radius)
{
  mCenter = center;
  mRadius = radius;
}

void Sphere::ComputeCentroid(const std::vector<Vector3>& points)
{
  // generate bounding aabb
  Aabb aabb;
  aabb.Compute(points);
  mCenter = aabb.GetCenter();
  // find the point furthest from aabb centroid
  float farthest_distance = 0.0f;
  for (const Vector3 point : points)
  {
    float point_dist = Math::DistanceSq(mCenter, point);
    farthest_distance = Math::Max(farthest_distance, point_dist);
  }
  // distance from centroid to furthest point is the radius 
  mRadius = Math::Sqrt(farthest_distance);
}

void Sphere::ComputeRitter(const std::vector<Vector3>& points)
{
  /******Student:Assignment2******/
  // The ritter method:
  // Find the largest spread on each axis.
  // Find which axis' pair of points are the furthest (euclidean distance) apart.
  // Choose the center of this line as the sphere center. Now incrementally expand the sphere.
  Warn("Assignment2: Required function un-implemented");
}

void Sphere::ComputePCA(const std::vector<Vector3>& points, int maxIterations)
{
  // The PCA method:
  // Compute the eigen values and vectors. Take the largest eigen vector as the axis of largest spread.
  // Compute the sphere center as the center of this axis then expand by all points.
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

bool Sphere::ContainsPoint(const Vector3& point)
{
  return PointSphere(point, mCenter, mRadius);
}

Vector3 Sphere::GetCenter() const
{
  return mCenter;
}

float Sphere::GetRadius() const
{
  return mRadius;
}

bool Sphere::Compare(const Sphere& rhs, float epsilon) const
{
  float posDiff = Math::Length(mCenter - rhs.mCenter);
  float radiusDiff = Math::Abs(mRadius - rhs.mRadius);

  return posDiff < epsilon && radiusDiff < epsilon;
}

DebugShape& Sphere::DebugDraw() const
{
  return gDebugDrawer->DrawSphere(*this);
}

//-----------------------------------------------------------------------------Aabb
Aabb::Aabb()
{
  //set the aabb to an initial bad value (where the min is smaller than the max)
  mMin.Splat(Math::PositiveMax());
  mMax.Splat(Math::NegativeMin());
}

Aabb::Aabb(const Vector3& min, const Vector3& max)
{
  mMin = min;
  mMax = max;
}

Aabb Aabb::BuildFromCenterAndHalfExtents(const Vector3& center, const Vector3& halfExtents)
{
  return Aabb(center - halfExtents, center + halfExtents);
}

Aabb Aabb::BuildFromMinMax(const Vector3& min, const Vector3& max)
{
  return Aabb(min, max);
}

float Aabb::GetVolume() const
{
  // volume = dx * dy * dz
  Vector3 delta = mMax - mMin;
  float volume = 1.0f;
  for (int i = 0; i < 3; ++i)
  {
    volume *= delta[i];
  }
  return volume;
}

float Aabb::GetSurfaceArea() const
{
  // surface area = 2 * (dx * dy + dy * dz + dz * dx)
  Vector3 delta = mMax - mMin;
  float surface_area = 0.0f;
  for (int i = 0; i < 3; ++i)
  {
    surface_area += delta[i] * delta[(i + 1) % 3];
  }
  surface_area *= 2.0f;
  return surface_area;
}

void Aabb::Compute(const std::vector<Vector3>& points)
{
  mMin.Splat(Math::PositiveMax());
  mMax.Splat(Math::NegativeMin());
  for(size_t i = 0; i < points.size(); ++i)
  {
    const Vector3& point = points[i];
    mMin = Math::Min(mMin, point);
    mMax = Math::Max(mMax, point);
  }
}

bool Aabb::Contains(const Aabb& other) const
{
  // test each axis
  for (int i = 0; i < 3; ++i)
  {
    // see if other aabb is outside of this aabb on this axis
    bool out = other.mMin[i] < mMin[i] || other.mMax[i] > mMax[i];
    if(out)
      return false;
  }
  // contained on all axes
  return true;
}

void Aabb::Expand(const Vector3& point)
{
  for(size_t i = 0; i < 3; ++i)
  {
    mMin[i] = Math::Min(mMin[i], point[i]);
    mMax[i] = Math::Max(mMax[i], point[i]);
  }
}

Aabb Aabb::Combine(const Aabb& lhs, const Aabb& rhs)
{
  Aabb result;
  for(size_t i = 0; i < 3; ++i)
  {
    result.mMin[i] = Math::Min(lhs.mMin[i], rhs.mMin[i]);
    result.mMax[i] = Math::Max(lhs.mMax[i], rhs.mMax[i]);
  }
  return result;
}

bool Aabb::Compare(const Aabb& rhs, float epsilon) const
{
  float pos1Diff = Math::Length(mMin - rhs.mMin);
  float pos2Diff = Math::Length(mMax - rhs.mMax);

  return pos1Diff < epsilon && pos2Diff < epsilon;
}

void Aabb::Transform(const Matrix4& transform)
{
  Vector3 half_delta = GetHalfSize();
  Vector3 center = mMin + half_delta;
  // compute new center
  for (int i = 0; i < 3; ++i)
  {
    center[i] += transform.array[i * 4 + 3];
  }

  /******Student:Assignment2******/
  // Compute aabb of the this aabb after it is transformed.
  // You should use the optimize method discussed in class (not transforming all 8 points).
  Warn("Assignment2: Required function un-implemented");

}

Vector3 Aabb::GetMin() const
{
  return mMin;
}

Vector3 Aabb::GetMax() const
{
  return mMax;
}

Vector3 Aabb::GetCenter() const
{
  return (mMin + mMax) * 0.5f;
}

Vector3 Aabb::GetHalfSize() const
{
  return (mMax - mMin) * 0.5f;
}

DebugShape& Aabb::DebugDraw() const
{
  return gDebugDrawer->DrawAabb(*this);
}

//-----------------------------------------------------------------------------Triangle
Triangle::Triangle()
{
  mPoints[0] = mPoints[1] = mPoints[2] = Vector3::cZero;
}

Triangle::Triangle(const Vector3& p0, const Vector3& p1, const Vector3& p2)
{
  mPoints[0] = p0;
  mPoints[1] = p1;
  mPoints[2] = p2;
}

DebugShape& Triangle::DebugDraw() const
{
  return gDebugDrawer->DrawTriangle(*this);
}

//-----------------------------------------------------------------------------Plane
Plane::Plane()
{
  mData = Vector4::cZero;
}

Plane::Plane(const Triangle& triangle)
{
  Set(triangle);
}

Plane::Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2)
{
  Set(p0, p1, p2);
}

Plane::Plane(const Vector3& normal, const Vector3& point)
{
  Set(normal, point);
}

void Plane::Set(const Triangle& triangle)
{
  // Set from the triangle's points
  Set(triangle.mPoints[0], triangle.mPoints[1], triangle.mPoints[2]);
}

void Plane::Set(const Vector3& p0, const Vector3& p1, const Vector3& p2)
{
  Set(Math::Cross(p1 - p0, p2 - p0), p0);
}

void Plane::Set(const Vector3& normal, const Vector3& point)
{
  Vector3 n = normal.Normalized();
  for (int i = 0; i < 3; ++i)
    mData[i] = n[i];
  mData[3] = Math::Dot(n, point);
}

Vector3 Plane::GetNormal() const
{
  return Vector3(mData.x, mData.y, mData.z);
}

float Plane::GetDistance() const
{
  return mData.w;
}

DebugShape& Plane::DebugDraw(float size) const
{
  return DebugDraw(size, size);
}

DebugShape& Plane::DebugDraw(float sizeX, float sizeY) const
{
  return gDebugDrawer->DrawPlane(*this, sizeX, sizeY);
}

//-----------------------------------------------------------------------------Frustum
void Frustum::Set(const Vector3& lbn, const Vector3& rbn, const Vector3& rtn, const Vector3& ltn,
                  const Vector3& lbf, const Vector3& rbf, const Vector3& rtf, const Vector3& ltf)
{
  mPoints[0] = lbn;
  mPoints[1] = rbn;
  mPoints[2] = rtn;
  mPoints[3] = ltn;
  mPoints[4] = lbf;
  mPoints[5] = rbf;
  mPoints[6] = rtf;
  mPoints[7] = ltf;

  //left
  mPlanes[0].Set(lbf, ltf, lbn);
  //right
  mPlanes[1].Set(rbn, rtf, rbf);
  //top
  mPlanes[2].Set(ltn, ltf, rtn);
  //bot
  mPlanes[3].Set(rbn, lbf, lbn);
  //near
  mPlanes[4].Set(lbn, ltn, rbn);
  //far
  mPlanes[5].Set(rbf, rtf, lbf);
}

Math::Vector4* Frustum::GetPlanes() const
{
  return (Vector4*)mPlanes;
}

DebugShape& Frustum::DebugDraw() const
{
  return gDebugDrawer->DrawFrustum(*this);
}
