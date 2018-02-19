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
  // calculate mean of points
  Vector3 mean(0.0f, 0.0f, 0.0f);
  for (const Vector3 & point : points)
  {
    mean += point;
  }
  float num_points = static_cast<float>(points.size());
  mean = mean / num_points; 
  // compute covariance
  Matrix3 covariance;
  covariance.ZeroOut();
  for (const Vector3 & point : points)
  {
    Vector3 variance = point - mean;
    for (int i = 0; i < 3; ++i) 
    {
      for (int j = 0; j < 3; ++j)
      {
        covariance.array[i * 3 + j] += (variance[i]) * (variance[j]);
      }
    }
  }
  covariance /= num_points;
  return covariance;
}

Matrix3 ComputeJacobiRotation(const Matrix3& matrix)
{
  // find largest off diagonal
  float off_diagonals[3];
  off_diagonals[0] = Math::Abs(matrix.m01);
  off_diagonals[1] = Math::Abs(matrix.m02);
  off_diagonals[2] = Math::Abs(matrix.m12);
  unsigned largest = 0;
  for (int i = 1; i < 3; ++i)
  {
    if (off_diagonals[i] > off_diagonals[largest])
    {
      largest = i;
    }
  }
  // finding the indicies of app, apq, aqp, and aqq
  // I feel like there is a better way to do this, but I do not know what
  // that better way is
  unsigned app_i, apq_i, aqp_i, aqq_i; 
  switch (largest)
  {
  case 0: app_i = 0; apq_i = 1; aqp_i = 3; aqq_i = 4; break;
  case 1: app_i = 0; apq_i = 2; aqp_i = 6; aqq_i = 8; break;
  case 2: app_i = 4; apq_i = 5; aqp_i = 7; aqq_i = 8; break;
  }
  // find values of app, apq, and aqq
  // apq = aqp
  float app, apq, aqq;
  app = matrix.array[app_i];
  apq = matrix.array[apq_i];
  aqq = matrix.array[aqq_i];
  // find beta and use beta to find tangent
  float beta = (aqq - app) / (2.0f * apq);
  float tangent;
  (beta > 0.0f) ? tangent = 1.0f : tangent = -1.0f;
  tangent = tangent / (Math::Abs(beta) + Math::Sqrt(beta * beta + 1.0f));
  // find theta and use to find sin and cos terms of jacobi
  float theta = Math::ArcTan(tangent);
  float cos = Math::Sqrt(1.0f / (tangent * tangent + 1.0f));
  float sin = tangent * cos;
  // Construct jacobi
  Matrix3 jacobi;
  jacobi.SetIdentity();
  jacobi.array[app_i] = cos;
  jacobi.array[apq_i] = sin;
  jacobi.array[aqp_i] = -sin;
  jacobi.array[aqq_i] = cos;
  return jacobi;
}

void ComputeEigenValuesAndVectors(const Matrix3& covariance, Vector3& eigenValues, Matrix3& eigenVectors, int maxIterations)
{
  Matrix3 diagonal = covariance;
  Matrix3 jacobi_final;
  jacobi_final.SetIdentity();
  // compute jacobi rotations
  const float epsilon = 1.0e-7f;
  for (int i = 0; i < maxIterations; ++i)
  {
    // stop computations if off diagonals are close to 0
    float off_diagonal_sum = Math::Abs(diagonal.m01) + 
      Math::Abs(diagonal.m02) + Math::Abs(diagonal.m12);
    if(off_diagonal_sum < epsilon)
      break;
    Matrix3 jacobi = ComputeJacobiRotation(diagonal);
    diagonal = jacobi.Transposed() * diagonal * jacobi;
    jacobi_final = jacobi_final * jacobi;
  }
  // pull out eigenvalues from diagonal
  for (int i = 0; i < 3; ++i)
  {
    eigenValues[i] = diagonal[i][i];
  }
  // set eigen vectors using jacobi
  eigenVectors = jacobi_final;
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

void Sphere::ExpandStartingSpread(const Vector3 & a, const Vector3 & b,
  const std::vector<Vector3> & points)
{
  Vector3 half_spread = 0.5f * (a - b);
  mCenter = b + half_spread;
  float sqr_radius = half_spread.LengthSq();
  // expand sphere for each point not contained
  for (const Vector3 & point : points)
  {
    Vector3 center_to_point = point - mCenter;
    float ctp_dist = center_to_point.LengthSq();
    // minimally expand sphere if point is outside sphere
    if (ctp_dist > sqr_radius)
    {
      center_to_point.Normalize();
      float current_radius = Math::Sqrt(sqr_radius);
      Vector3 b = mCenter - center_to_point * current_radius;
      mCenter = (b + point) * 0.5f;
      sqr_radius = (point - mCenter).LengthSq();
    }
  }
  // all points accounted for
  // finding final radius
  mRadius = Math::Sqrt(sqr_radius);
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
  // min_max stores pointers to the min and max vectors for each component
  // order: x min, x max, y min, y max, z min, z max
  const Vector3 * min_max[6];
  // fill min max array with first point
  for(unsigned i = 0; i < 6; ++i)
    min_max[i] = &(points[0]);
  
  for (unsigned p = 1; p < points.size(); ++p)
  {
    for (unsigned i = 0; i < 3; ++i)
    {
      float component_val = points[p][i];
      // test (i component < current component min)
      if (component_val < (*min_max[i * 2])[i])
        min_max[i * 2] = &points[p];
      // testing max
      else if (component_val > (*min_max[i * 2 + 1])[i])
        min_max[i * 2 + 1] = &points[p];
    }
  }
  // find which set of points has the largest spread
  int spread_i = -1;
  float largest_sqr_dist = 0.0f;
  for (unsigned i = 0; i < 3; ++i)
  {
    Vector3 spread = *min_max[i * 2 + 1] - *min_max[i * 2];
    float sqr_dist = spread.LengthSq();
    if(sqr_dist > largest_sqr_dist)
    {
      spread_i = i * 2;
      largest_sqr_dist = sqr_dist;
    }
  }
  // expand starting spread to contain all points
  ExpandStartingSpread(*min_max[spread_i + 1], *min_max[spread_i], points);
}

void Sphere::ComputePCA(const std::vector<Vector3>& points, int maxIterations)
{
  Matrix3 covariance = ComputeCovarianceMatrix(points);
  Vector3 eigen_values;
  Matrix3 eigen_vectors;
  ComputeEigenValuesAndVectors(covariance, eigen_values, eigen_vectors, 
    maxIterations);
  // find largest eigen value
  int largest_i = 0;
  for (int i = 1; i < 3; ++i)
  {
    if(eigen_values[i] > eigen_values[largest_i])
      largest_i = i;
  }
  // found axis of largest spread
  Vector3 axis = eigen_vectors.Basis(largest_i);
  // find points 2 points with largest spread on axis
  // 0 : most positive value
  // 1 : most negative value
  const Vector3 * largest_spread[2];
  float dist[2] = {Math::NegativeMin(), Math::PositiveMax()};
  for (const Vector3 & point : points)
  {
    float new_dist = Math::Dot(axis, point);
    if(new_dist > dist[0])
    {
      largest_spread[0] = &point;
      dist[0] = new_dist;
    }
    else if (new_dist < dist[1])
    {
      largest_spread[1] = &point;
      dist[1] = new_dist;
    }
  }
  // expand starting spread to contain all points
  ExpandStartingSpread(*largest_spread[0], *largest_spread[1], points);
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
  Vector3 new_half_delta(0.0f, 0.0f, 0.0f);
  // compute new half delta
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      new_half_delta[i] += Math::Abs(transform.array[i * 4 + j]) * half_delta[j];
    }
  }
  // compute new center
  Vector3 center = mMin + half_delta;
  center = Math::TransformPoint(transform, center);
  // compute new min and max
  mMin = center - new_half_delta;
  mMax = center + new_half_delta;
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
