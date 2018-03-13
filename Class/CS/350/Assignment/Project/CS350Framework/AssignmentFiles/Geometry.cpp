///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"


bool InRange(float min, float max, float value)
{
  return value >= min && value <= max;
}

bool OutRange(float min, float max, float value)
{
  return value < min || value > max;
}

bool BarycentricCoordinates(const Vector3& point, const Vector3& a, const Vector3& b,
                            float& u, float& v, float expansionEpsilon)
{
  // b -> p and b -> a vector calcuations
  Vector3 b_p = point - b;
  Vector3 b_a = a - b;
  // calculation for u
  float denom = Math::Dot(b_a, b_a);
  if (denom == 0.0f) {
    u = 0.0f;
    v = 0.0f;
    return false;
  }
  float numer = Math::Dot(b_p, b_a);
  u = numer / denom;
  v = 1.0f - u;
  // see if the point is on the line segment
  float min = 0.0f - expansionEpsilon;
  float max = 1.0f + expansionEpsilon;
  return InRange(min, max, u);
}

bool BarycentricCoordinates(const Vector3& point, const Vector3& a, const Vector3& b, const Vector3& c,
                            float& u, float& v, float& w, float expansionEpsilon)
{
  // calculate cross products for u and v
  Vector3 Npbc = Math::Cross(b - point, c - point);
  Vector3 Npca = Math::Cross(c - point, a - point);
  Vector3 Nabc = Math::Cross(b - a, c - a);
  // calculate u, v and w
  float denom = Math::Dot(Nabc, Nabc);
  if (denom == 0.0f) {
    // degenerate triangle
    u = 0.0f;
    v = 0.0f;
    w = 0.0f;
    return false;
  }
  float u_numer = Math::Dot(Npbc, Nabc);
  float v_numer = Math::Dot(Npca, Nabc);
  u = u_numer / denom;
  v = v_numer / denom;
  w = 1.0f - u - v;
  // see if the point is in the triangle
  float min = 0.0f - expansionEpsilon;
  float max = 1.0f + expansionEpsilon;
  bool u_in_range = InRange(min, max, u);
  bool v_in_range = InRange(min, max, v);
  bool w_in_range = InRange(min, max, w);
  return u_in_range && v_in_range && w_in_range;

}

IntersectionType::Type PointPlane(const Vector3& point, const Vector4& plane, float epsilon)
{
  // dot point and plane normal
  float dot_result = 0.0f;
  for(int i = 0; i < 3; ++i)
    dot_result += point[i] * plane[i];
  // subtract w d component
  dot_result -= plane[3];
  // find intersection type
  if(dot_result < -epsilon)
    return IntersectionType::Outside;
  else if(dot_result > epsilon)
    return IntersectionType::Inside;
  return IntersectionType::Coplanar;
}

bool PointSphere(const Vector3& point, const Vector3& sphereCenter, float sphereRadius)
{
  // center to point distance squared
  float ctp_dist= point.DistanceSq(sphereCenter);
  if(ctp_dist <= sphereRadius * sphereRadius)
    return true;
  return false;
}

bool PointAabb(const Vector3& point, const Vector3& aabbMin, const Vector3& aabbMax)
{
  // return false if one dimension is out of range
  for (int i = 0; i < 3; ++i)
  {
    if(OutRange(aabbMin[i], aabbMax[i], point[i]))
      return false;
  }
  return true;
}

bool RayPlane(const Vector3& rayStart, const Vector3& rayDir,
              const Vector4& plane, float& t, float parallelCheckEpsilon)
{
  ++Application::mStatistics.mRayPlaneTests;
  float denom = 0.0f;
  // dot(plane normal, ray dir)
  for(int i = 0; i < 3; ++i)
    denom += plane[i] * rayDir[i];
  // make sure ray is not parallel
  if(InRange(-parallelCheckEpsilon, parallelCheckEpsilon, denom))
    return false;
  // d - dot(plane normal, ray start
  float numer = plane[3];
  for(int i = 0; i < 3; ++i)
    numer -= plane[i] * rayStart[i];
  t = numer / denom;
  // intersection if t is non-negative
  return t >= 0.0f;
}

bool RayTriangle(const Vector3& rayStart, const Vector3& rayDir,
                 const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
                 float& t, float triExpansionEpsilon, float parallelCheckEpsilon)
{
  ++Application::mStatistics.mRayTriangleTests;
  // finding intersection with triangle plane
  Plane plane;
  plane.Set(triP0, triP1, triP2);
  bool intersect = RayPlane(rayStart, rayDir, 
    plane.mData, t, parallelCheckEpsilon);
  // check for plane intersection
  if(!intersect)
    return false;
  // find point on plane
  Vector3 point = rayStart + rayDir * t;
  // see if point exists within triangle
  float u, v, w;
  bool in_tri = BarycentricCoordinates(point, triP0, triP1, triP2, 
    u, v, w, triExpansionEpsilon);
  return in_tri;
}

bool RaySphere(const Vector3& rayStart, const Vector3& rayDir,
               const Vector3& sphereCenter, float sphereRadius,
               float& t)
{
  ++Application::mStatistics.mRaySphereTests;
  // finding leading terms in quadratic
  Vector3 ray_center = sphereCenter - rayStart;
  float a = Math::Dot(rayDir, rayDir);
  float b = -2.0f * Math::Dot(ray_center, rayDir);
  float c = Math::Dot(ray_center, ray_center) - sphereRadius * sphereRadius;
  // compute discriminant and find solution
  float discriminant = b * b - 4.0f * a * c;
  if(discriminant < 0.0f)
    return false;
  float a2 = 2.0f * a;
  // one solution
  if (discriminant == 0.0f)
  {
    t = -b / (a2);
    return t >= 0.0f;
  }
  // two solutions
  float sqrt_discriminant = Math::Sqrt(discriminant);
  float t0 = (-b - sqrt_discriminant) / a2;
  float t1 = (-b + sqrt_discriminant) / a2;
  // find first intersection
  if(t1 < 0.0f)
    return false;
  else if (t0 < 0.0f)
  {
    t = 0.0f;
    return true;
  }
  t = t0;
  return true;
}

bool RayAabb(const Vector3& rayStart, const Vector3& rayDir,
             const Vector3& aabbMin, const Vector3& aabbMax, float& t, float parallelCheckEpsilon)
{
  // for each dimension
  ++Application::mStatistics.mRayAabbTests;
  // min of all max
  float t_min_of_max = FLT_MAX;
  // max of all min
  float t_max_of_min = -FLT_MAX;
  for (int i = 0; i < 3; ++i)
  {
    // check for parallel axis
    if (InRange(-parallelCheckEpsilon, parallelCheckEpsilon, rayDir[i])) 
    {
      if (InRange(aabbMin[i], aabbMax[i], rayStart[i]))
        continue;
      // not within the aabb's plane implies no collision
      return false;
    }
    // calculate t min and t max
    float min = (aabbMin[i] - rayStart[i]) / rayDir[i];
    float max = (aabbMax[i] - rayStart[i]) / rayDir[i];
    // max and min is flipped if direction is negative
    if (rayDir[i] < 0.0f)
    {
      t_max_of_min = Math::Max(t_max_of_min, max);
      t_min_of_max = Math::Min(t_min_of_max, min);
    }
    else
    {
      t_max_of_min = Math::Max(t_max_of_min, min);
      t_min_of_max = Math::Min(t_min_of_max, max);
    }
  }
  // test for intersection
  if (t_max_of_min < t_min_of_max)
  {
    if (t_min_of_max < 0.0f)
      return false;
    else if (t_max_of_min < 0.0f)
      t = 0.0f;
    else
      t = t_max_of_min;
  }
  else
    return false;
  return true;
    
}

IntersectionType::Type PlaneTriangle(const Vector4& plane, 
                                     const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
                                     float epsilon)
{
  ++Application::mStatistics.mPlaneTriangleTests;
  // finding where each point is in relation to the plane
  char result = PointPlane(triP0, plane, epsilon);
  result |= PointPlane(triP1, plane, epsilon);
  result |= PointPlane(triP2, plane, epsilon);
  return static_cast<IntersectionType::Type>(result);
}

IntersectionType::Type PlaneSphere(const Vector4& plane,
                                   const Vector3& sphereCenter, float sphereRadius)
{
  ++Application::mStatistics.mPlaneSphereTests;
  // using sphere radius as the epsilon for point plane
  IntersectionType::Type type = PointPlane(sphereCenter, plane, sphereRadius);
  // coplanar is equivalent to overlap
  if(type == IntersectionType::Coplanar)
    return IntersectionType::Overlaps;
  return type;
}

IntersectionType::Type PlaneAabb(const Vector4& plane,
                                 const Vector3& aabbMin, const Vector3& aabbMax)
{
  ++Application::mStatistics.mPlaneAabbTests;
  Vector3 center;
  // find center of aabb and radius relative to plane
  float aabb_radius =0.0f;
  for (int i = 0; i < 3; ++i)
  {
    float half_delta = (aabbMax[i] - aabbMin[i]) / 2.0f;
    center[i] = aabbMin[i] + half_delta;
    aabb_radius += half_delta * Math::Abs(plane[i]);

  }
  // find intersection type
  return PlaneSphere(plane, center, aabb_radius);
}

IntersectionType::Type FrustumTriangle(const Vector4 planes[6],
                                       const Vector3& triP0, const Vector3& triP1, const Vector3& triP2,
                                       float epsilon)
{
  ++Application::mStatistics.mFrustumTriangleTests;
  bool overlap = false;
  for (int i = 0; i < 6; ++i)
  {
    // find intersection type with single plane
    IntersectionType::Type result;
    result = PlaneTriangle(planes[i], triP0, triP1, triP2, epsilon);
    // no more checks needed if outside of plane
    if(result == IntersectionType::Outside)
    {
      return IntersectionType::Outside;
    }
    // coplanar and overlap are equivalent
    else if(result == IntersectionType::Coplanar || 
      result == IntersectionType::Overlaps)
    {
      overlap = true;
    }
  }
  // if one plane overlaps, the triangle is overlapping
  if(overlap)
    return IntersectionType::Overlaps;
  return IntersectionType::Inside;
}

IntersectionType::Type FrustumSphere(const Vector4 planes[6],
                                     const Vector3& sphereCenter, float sphereRadius, size_t& lastAxis)
{
  ++Application::mStatistics.mFrustumSphereTests;
  bool overlap = false;
  for (int i = 0; i < 6; ++i)
  {
    // begin with the last axis
    int plane_index = (i + lastAxis) % 6;
    // find intersection type with plane i
    IntersectionType::Type result;
    result = PlaneSphere(planes[plane_index], sphereCenter, sphereRadius);
    if(result == IntersectionType::Outside)
    {
      lastAxis = plane_index;
      return IntersectionType::Outside;
    }
    else if(result == IntersectionType::Overlaps)
      overlap = true;
  }
  // check for overlap on one or more planes
  if(overlap)
    return IntersectionType::Overlaps;
  return IntersectionType::Inside;
}

IntersectionType::Type FrustumAabb(const Vector4 planes[6],
                                   const Vector3& aabbMin, const Vector3& aabbMax, size_t& lastAxis)
{
  ++Application::mStatistics.mFrustumAabbTests;
  // same as FrustumAabb, but deals with PlaneAabb instead
  bool overlap = false;
  for (int i = 0; i < 6; ++i)
  {
    // begin with the last axis
    int plane_index = (i + lastAxis) % 6;
    // find intersection with plane i
    IntersectionType::Type result;
    result = PlaneAabb(planes[plane_index], aabbMin, aabbMax);
    if (result == IntersectionType::Outside)
    {
      lastAxis = plane_index;
      return IntersectionType::Outside;
    }
    else if (result == IntersectionType::Overlaps)
      overlap = true;
  }
  if (overlap)
    return IntersectionType::Overlaps;
  return IntersectionType::Inside;
}

bool SphereSphere(const Vector3& sphereCenter0, float sphereRadius0,
                  const Vector3& sphereCenter1, float sphereRadius1)
{
  ++Application::mStatistics.mSphereSphereTests;
  // test with one large sphere and point
  float large_radius = sphereRadius0 + sphereRadius1;
  return PointSphere(sphereCenter0, sphereCenter1, large_radius);
}

bool AabbAabb(const Vector3& aabbMin0, const Vector3& aabbMax0,
              const Vector3& aabbMin1, const Vector3& aabbMax1)
{
  ++Application::mStatistics.mAabbAabbTests;
  // test each axis for non intersection
  for (int i = 0; i < 3; ++i)
  {
    if(aabbMax0[i] < aabbMin1[i] || aabbMax1[i] < aabbMin0[i])
      return false;
  }
  // no non intersections found
  return true;
}
