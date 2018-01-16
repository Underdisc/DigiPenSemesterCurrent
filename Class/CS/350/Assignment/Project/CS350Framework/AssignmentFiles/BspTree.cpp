///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//--------------------------------------------------------------------BspTreeNode
BspTreeNode* BspTreeNode::GetFrontChild() const
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
  return nullptr;
}

BspTreeNode* BspTreeNode::GetBackChild() const
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
  return nullptr;
}

Plane BspTreeNode::GetSplitPlane() const
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
  return Plane();
}

void BspTreeNode::GetTriangles(TriangleList& triangles) const
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}


//--------------------------------------------------------------------BspTree
BspTree::BspTree()
{
}

BspTree::~BspTree()
{
}

void BspTree::SplitTriangle(const Plane& plane, const Triangle& tri, TriangleList& coplanarFront, TriangleList& coplanarBack, TriangleList& front, TriangleList& back, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

float BspTree::CalculateScore(const TriangleList& triangles, size_t testIndex, float k, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
  return Math::PositiveMax();
}

size_t BspTree::PickSplitPlane(const TriangleList& triangles, float k, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
  return 0;
}

void BspTree::Construct(const TriangleList& triangles, float k, float epsilon)
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
}

BspTreeNode* BspTree::GetRoot() const
{
  /******Student:Assignment4******/
  Warn("Assignment4: Required function un-implemented");
  return nullptr;
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

