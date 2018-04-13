///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"



//--------------------------------------------------------------------BspTreeNode
void BspTreeNode::ClipTo(BspTreeNode * clip_node, float epsilon)
{

  // need a new and old triangle list
  TriangleList front_triangles;
  TriangleList back_triangles;

  for (const Triangle & tri : mTriangles)
  {
    BspTree::SplitTriangle(clip_node->mPlane, tri, front_triangles, 
      back_triangles, front_triangles, back_triangles, epsilon);
  }

  mTriangles = front_triangles;
}

void BspTreeNode::ClipTriangles(const TriangleList & triangles,
  TriangleList * front_triangles, TriangleList * back_triangles, float epsilon)
{
  for (const Triangle & tri : triangles)
  {
    BspTree::SplitTriangle(mPlane, tri, *front_triangles, *back_triangles,
      *front_triangles, *back_triangles, epsilon);
  }
}


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

//--------------------------------------------------------------------Helpers

Vector3 RayPlaneIntersection(const Vector3 & p0, const Vector3 & p1, const Plane & plane)
{
  // find and return point where ray intersects plane
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
  // ignore degenerate triangles
  if(normal.Length() < epsilon)
    return Math::PositiveMax();
  // find how many tris are inside, outside and overlapping plane
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
  // calculate score
  float score = k * (float)overlap;
  score += (1.0f - k) * (float)Math::Abs(front - back);
  return score;
}

size_t BspTree::PickSplitPlane(const TriangleList& triangles, float k, float epsilon)
{
  // finding and returning the index of the tri with the lowest score
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

bool BspTree::RayCast(const Ray& ray, float& t, float planeThicknessEpsilon, 
  float triExpansionEpsilon, int debuggingIndex)
{
  return RayCastRecursive(ray, &t, 0.0f, Math::PositiveMax(), mRoot,
    planeThicknessEpsilon, triExpansionEpsilon);
}

void BspTree::AllTriangles(TriangleList& triangles) const
{
  AllTrianglesRecursive(mRoot, &triangles);
}

void BspTree::Invert()
{
  InvertRecursive(mRoot);
}

void BspTree::ClipTo(BspTree* tree, float epsilon)
{
  ClipToRecurseThisTree(mRoot, tree, epsilon);
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
  DebugDrawRecursive(mRoot, level, color, bitMask);
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

bool BspTree::RayCastRecursive(const Ray & ray, float * t, float t_min,
  float t_max, const BspTreeNode * node, float planeThicknessEpsilon, 
  float triExpansionEpsilon)
{
  if(!node)
    return false;
  bool intersection = false;
  // find where the ray start is in relation to the plane
  Vector3 segment_start = ray.GetPoint(t_min);
  IntersectionType::Type min_intersection = PointPlane(ray.mStart, 
    node->mPlane.mData, planeThicknessEpsilon);
  // check contained tris and recurse down front and back
  if (min_intersection == IntersectionType::Coplanar)
  {
    intersection |= RayCastContainedTriangles(ray, node, t,
      triExpansionEpsilon);
    intersection |= RayCastRecursive(ray, t, t_min, t_max,
      node->mFrontChild, planeThicknessEpsilon, triExpansionEpsilon);
    intersection |= RayCastRecursive(ray, t, t_min, t_max,
      node->mBackChild,
      planeThicknessEpsilon, triExpansionEpsilon);
    return intersection;
  }
  // define near and far nodes
  BspTreeNode * near_node;
  BspTreeNode * far_node;
  if (min_intersection == IntersectionType::Inside)
  {
    near_node = node->mFrontChild;
    far_node = node->mBackChild;
  }
  else
  {
    near_node = node->mBackChild;
    far_node = node->mFrontChild;
  }
  // only recurse down near node if there is no plane intersection
  float t_plane;
  bool plane_intersection = RayPlane(ray.mStart, ray.mDirection,
    node->mPlane.mData, t_plane);
  if (!plane_intersection)
  {
    intersection |= RayCastRecursive(ray, t, t_min, t_max, near_node,
      planeThicknessEpsilon, triExpansionEpsilon);
    return intersection;
  }
  // find t values for thick plane start and end
  Vector3 normal = node->mPlane.GetNormal();
  float normal_dot_raydir = Math::Dot(normal, ray.mDirection);
  float t_plane_epsilon = Math::Abs(planeThicknessEpsilon / normal_dot_raydir);
  float t_plane_min = t_plane - t_plane_epsilon;
  float t_plane_max = t_plane + t_plane_epsilon;
  // only recurse down one side if segment does not hit thick plane
  if (t_plane_min > t_max)
  {
    intersection |= RayCastRecursive(ray, t, t_min, t_max,
      near_node, planeThicknessEpsilon, triExpansionEpsilon);
    return intersection;
  }
  if (t_plane_max < t_min)
  {
    intersection |= RayCastRecursive(ray, t, t_min, t_max,
      far_node, planeThicknessEpsilon, triExpansionEpsilon);
    return intersection;
  }
  // find t_min and t_max for near and far sides
  float near_t_min = t_min;
  float near_t_max = t_plane_max;
  float far_t_min = t_plane_min; 
  float far_t_max = t_max;
  // check coplanar tris and recurse down both near and far nodes
  intersection |=  RayCastContainedTriangles(ray, node, t, 
    triExpansionEpsilon);
  intersection |= RayCastRecursive(ray, t, near_t_min, near_t_max, 
    near_node, planeThicknessEpsilon, triExpansionEpsilon);
  intersection |= RayCastRecursive(ray, t, far_t_min, far_t_max, 
    far_node, planeThicknessEpsilon, triExpansionEpsilon);
  return intersection;
}

void BspTree::AllTrianglesRecursive(const BspTreeNode * node,
  TriangleList * triangles) const
{
  if(!node)
    return;
  // add triangles in pre-order traversal
  for (const Triangle & tri : node->mTriangles)
  {
    triangles->push_back(tri);
  }
  AllTrianglesRecursive(node->mFrontChild, triangles);
  AllTrianglesRecursive(node->mBackChild, triangles);
}

void BspTree::InvertRecursive(BspTreeNode * node)
{
  if(!node)
    return;
  // flip plane
  node->mPlane.mData *= -1.0f;
  // flip all triangles
  for (Triangle & tri : node->mTriangles)
  {
    Vector3 temp_point = tri.mPoints[0];
    tri.mPoints[0] = tri.mPoints[1];
    tri.mPoints[1] = temp_point;
  }
  // flip front and back nodes
  BspTreeNode * temp_node = node->mFrontChild;
  node->mFrontChild = node->mBackChild;
  node->mBackChild = temp_node;
  // recurse into front and back nodes
  InvertRecursive(node->mFrontChild);
  InvertRecursive(node->mBackChild);
}

void BspTree::ClipToRecurseThisTree(BspTreeNode * current_node, BspTree * tree,
  float epsilon)
{
  if(!current_node)
    return;
  ClipToRecurseOtherTree(current_node, tree->mRoot, epsilon);
  
  ClipToRecurseThisTree(current_node->mFrontChild, tree, epsilon);
  ClipToRecurseThisTree(current_node->mBackChild, tree, epsilon);
  
}

void BspTree::ClipToRecurseOtherTree(BspTreeNode * this_node,  
  BspTreeNode * other_current_node, float epsilon)
{
  if(!other_current_node)
    return;
  this_node->ClipTo(other_current_node, epsilon);

  ClipToRecurseOtherTree(this_node, other_current_node->mFrontChild, epsilon);
  ClipToRecurseOtherTree(this_node, other_current_node->mBackChild, epsilon);
}

void BspTree::DebugDrawRecursive(const BspTreeNode * node, int level,
  const Vector4 & color, int bitMask)
{
  if(!node)
    return;
  // draw shape if at appropriate level
  if (level < 1)
  {
    // draw plane
    const Plane & plane = node->mPlane;
    DebugShape & plane_draw = gDebugDrawer->DrawPlane(plane, 5.0f, 5.0f);
    plane_draw.SetTransform(Matrix4::cIdentity);
    plane_draw.Color(color);
    plane_draw.SetMaskBit(bitMask);
    // draw all tirangles in node
    for (const Triangle & tri : node->mTriangles)
    {
      DebugShape & tri_draw = gDebugDrawer->DrawTriangle(tri);
      tri_draw.SetTransform(Matrix4::cIdentity);
      tri_draw.Color(color);
      tri_draw.SetMaskBit(bitMask);
    }
  }
  // find next level and move to next nodes
  // or return at final level
  int next_level;
  if (level == -1)
    next_level = -1;
  else if(level == 0)
    return;
  else
    next_level = level - 1;
  DebugDrawRecursive(node->mFrontChild, next_level, color, bitMask);
  DebugDrawRecursive(node->mBackChild, next_level, color, bitMask);
}

// Helpers

bool BspTree::RayCastContainedTriangles(const Ray & ray, 
  const BspTreeNode * node, float * t, float triExpansionEpsilon)
{
  bool intersection = false;
  for (const Triangle & tri : node->mTriangles)
  {
    float new_t ;
    bool tri_intersection;
    tri_intersection = RayTriangle(ray.mStart, ray.mDirection, tri.mPoints[0], 
      tri.mPoints[1], tri.mPoints[2], new_t, triExpansionEpsilon);
    intersection |= tri_intersection;
    if (tri_intersection && new_t < *t)
      *t = new_t;
  }
  return intersection;
}
