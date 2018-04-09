///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//--------------------------------------------------------------------Helpers

Vector3 RayPlaneIntersection(const Vector3 & p0, const Vector3 & p1, const Plane & plane)
{
  Vector3 ray_start = p0;
  Vector3 ray_dir = p1 - p0;
  Ray edge_ray(ray_start, ray_dir);
  float t;
  RayPlane(edge_ray.mStart, edge_ray.mDirection, plane.mData, t);
  return edge_ray.GetPoint(t);
}

void AddPointsToTriangleList(const std::vector<const Vector3 *> & p, TriangleList & list)
{
  list.push_back(Triangle(*p[0], *p[1], *p[2]));
  if (p.size() == 4)
  {
    list.push_back(Triangle(*p[0], *p[2], *p[3]));
  }
}

//--------------------------------------------------------------------BspTreeNode
BspTreeNode* BspTreeNode::GetFrontChild() const
{
  return mFrontChild;
}

BspTreeNode* BspTreeNode::GetBackChild() const
{
  return mBackChild;
}

Plane BspTreeNode::GetSplitPlane() const
{
  return mPlane;
}

void BspTreeNode::GetTriangles(TriangleList& triangles) const
{
  triangles = mTriangles;
}


//--------------------------------------------------------------------BspTree
BspTree::BspTree() : mRoot(nullptr)
{
}

BspTree::~BspTree()
{
}

void BspTree::SplitTriangle(const Plane& plane, const Triangle& tri, TriangleList& coplanarFront, TriangleList& coplanarBack, TriangleList& front, TriangleList& back, float epsilon)
{
  // not calling PlaneTriangle because the intersection data from PointPlane
  // is needed
  IntersectionType::Type intersections[3];
  unsigned char tri_result = 0x00;
  for (int i = 0; i < 3; ++i)
  {
    intersections[i] = PointPlane(tri.mPoints[i], plane.mData, epsilon);
    tri_result |= intersections[i];
  }
  IntersectionType::Type tri_intersection = 
    static_cast<IntersectionType::Type>(tri_result);
  // handle cases where splitting triangle is not required
  switch (tri_intersection)
  {
  case IntersectionType::Inside:
    // triangle is in front
    front.push_back(tri);
    return;
  case IntersectionType::Outside:
    // triangle is in back
    back.push_back(tri);
    return;
  case IntersectionType::Coplanar:
    // triangle is coplanar
    // use triangle normal to determine side
    Vector3 tri_normal = tri.ScaledNormal();
    tri_normal.Normalized();
    Vector3 plane_normal = plane.GetNormal();
    float dot_result = Math::Dot(tri_normal, plane_normal);
    if (dot_result > 0)
      coplanarFront.push_back(tri);
    else
      coplanarBack.push_back(tri);
    return;
  }
  // triangle must be split
  // find the front and back points for the triangle split
  std::vector<Vector3> intersection_points;
  intersection_points.reserve(2);
  std::vector<const Vector3 *> front_points;
  std::vector<const Vector3 *> back_points;
  for (int i = 0; i < 3; ++i)
  {
    int prev_index = i;
    int next_index = (i + 1) % 3;
    IntersectionType::Type prev = intersections[prev_index];
    IntersectionType::Type next = intersections[next_index];
    switch (next)
    {
    case IntersectionType::Inside:
      // next point is inside
      if(prev == IntersectionType::Inside || 
        prev == IntersectionType::Coplanar)
      {
        front_points.push_back(&tri.mPoints[next_index]);
      }
      else
      {
        Vector3 new_point = RayPlaneIntersection(
          tri.mPoints[prev_index], tri.mPoints[next_index], plane);
        intersection_points.push_back(new_point);
        front_points.push_back(&intersection_points.back());
        front_points.push_back(&tri.mPoints[next_index]);
        back_points.push_back(&intersection_points.back());
      }
      break;
    case IntersectionType::Coplanar:
      // next point is coplanar
      if (prev == IntersectionType::Inside ||
        prev == IntersectionType::Coplanar)
      {
        front_points.push_back(&tri.mPoints[next_index]);
      }
      else
      {
        front_points.push_back(&tri.mPoints[next_index]);
        back_points.push_back(&tri.mPoints[next_index]);
      }
      break;
    case IntersectionType::Outside:
      // next point is outside
      if (prev == IntersectionType::Inside)
      {
        Vector3 new_point = RayPlaneIntersection(
          tri.mPoints[next_index], tri.mPoints[prev_index], plane);
        intersection_points.push_back(new_point);
        front_points.push_back(&intersection_points.back());
        back_points.push_back(&intersection_points.back());
        back_points.push_back(&tri.mPoints[next_index]);
      }
      else if (prev == IntersectionType::Coplanar)
      {
        back_points.push_back(&tri.mPoints[prev_index]);
        back_points.push_back(&tri.mPoints[next_index]);
      }
      else if (prev == IntersectionType::Outside)
      {
        back_points.push_back(&tri.mPoints[next_index]);
      }
      break;
    }
  }
  // add front and back points to appropriate triangle lists
  AddPointsToTriangleList(front_points, front);
  AddPointsToTriangleList(back_points, back);
}

float BspTree::CalculateScore(const TriangleList& triangles, size_t testIndex, float k, float epsilon)
{
  int front = 0;
  int back = 0;
  int overlap = 0;
  const Triangle & test_tri = triangles[testIndex];
  Vector3 normal = test_tri.ScaledNormal();
  if(normal.Length() < epsilon)
    return Math::PositiveMax();
  Plane test_plane(normal, test_tri.mPoints[0]);
  for (size_t i = 0; i < triangles.size(); ++i)
  {
    if(i == testIndex)
      continue;
    const Triangle & tri = triangles[i];
    IntersectionType::Type tri_intersect;
    tri_intersect = PlaneTriangle(test_plane.mData, tri.mPoints[0], 
      tri.mPoints[1], tri.mPoints[2], epsilon);
    switch (tri_intersect)
    {
    case IntersectionType::Inside: ++front; break;
    case IntersectionType::Outside: ++back; break;
    case IntersectionType::Overlaps: ++overlap; break;
    }
  }
  float score = k * (float)overlap;
  score += (1.0f - k) * (float)Math::Abs(front - back);
  return score;
}

size_t BspTree::PickSplitPlane(const TriangleList& triangles, float k, float epsilon)
{
  float lowest_score = Math::PositiveMax();
  size_t split_tri = 0;
  for (size_t i = 0; i < triangles.size(); ++i)
  {
    float score = CalculateScore(triangles, i, k, epsilon);
    if(score < lowest_score)
    {
      lowest_score = score;
      split_tri = i;
    }
  }
  return split_tri;
}

void BspTree::Construct(const TriangleList& triangles, float k, float epsilon)
{
  mRoot = ConstructRecursive(triangles, k, epsilon);
}

BspTreeNode* BspTree::GetRoot() const
{
  return mRoot;
}

bool BspTree::RayCast(const Ray& ray, float& t, float planeThicknessEpsilon, float triExpansionEpsilon, int debuggingIndex)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
  t = Math::PositiveMax();
  return false;
}

void BspTree::AllTriangles(TriangleList& triangles) const
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

void BspTree::Invert()
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

void BspTree::ClipTo(BspTree* tree, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

void BspTree::Union(BspTree* tree, float k, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

void BspTree::Intersection(BspTree* tree, float k, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

void BspTree::Subtract(BspTree* tree, float k, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

void BspTree::DebugDraw(int level, const Vector4& color, int bitMask)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}


BspTreeNode * BspTree::ConstructRecursive(const TriangleList & triangles, float k, float epsilon)
{
  if (triangles.size() == 0)
  {
    return nullptr;
  }
  BspTreeNode * node = new BspTreeNode;
  if(triangles.size() <= 1)
  {
    node->mFrontChild = nullptr;
    node->mBackChild = nullptr;
    node->mTriangles = triangles;
    node->mPlane = Plane(triangles[0]);
    return node;
  }
  size_t split_tri = PickSplitPlane(triangles, k, epsilon);
  // is it even worth to add the split plane at the start
  node->mPlane = Plane(triangles[split_tri]);
  TriangleList front_tris;
  TriangleList back_tris;
  for (size_t i = 0; i < triangles.size(); ++i)
  {
    const Triangle & tri = triangles[i];
    SplitTriangle(node->mPlane, tri, node->mTriangles, node->mTriangles,
      front_tris, back_tris, epsilon);
  }
  node->mFrontChild = ConstructRecursive(front_tris, k, epsilon);
  node->mBackChild = ConstructRecursive(back_tris, k, epsilon);
  
  return node;
}

