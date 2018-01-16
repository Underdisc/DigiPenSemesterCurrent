///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//--------------------------------------------------------------------DynamicAabbTreeNode
DynamicAabbTreeNode* DynamicAabbTreeNode::GetParent() const
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return nullptr;
}

DynamicAabbTreeNode* DynamicAabbTreeNode::GetLeftChild() const
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return nullptr;
}

DynamicAabbTreeNode* DynamicAabbTreeNode::GetRightChild() const
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return nullptr;
}

Aabb DynamicAabbTreeNode::GetAabb() const
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return Aabb();
}

void* DynamicAabbTreeNode::GetClientData() const
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return nullptr;
}

int DynamicAabbTreeNode::GetHeight() const
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return -1;
}


//--------------------------------------------------------------------DynamicAabbTree
const float DynamicAabbTree::mFatteningFactor = 1.1f;

DynamicAabbTree::DynamicAabbTree()
{
  mType = SpatialPartitionTypes::AabbTree;
}

DynamicAabbTree::~DynamicAabbTree()
{
}

void DynamicAabbTree::InsertData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

void DynamicAabbTree::UpdateData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

void DynamicAabbTree::RemoveData(SpatialPartitionKey& key)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

void DynamicAabbTree::DebugDraw(int level, const Math::Matrix4& transform, const Vector4& color, int bitMask)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

void DynamicAabbTree::CastRay(const Ray& ray, CastResults& results)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

void DynamicAabbTree::CastFrustum(const Frustum& frustum, CastResults& results)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

void DynamicAabbTree::SelfQuery(QueryResults& results)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

DynamicAabbTreeNode* DynamicAabbTree::GetRoot() const
{
  /******Student:Assignment3******/
  // Return the root of your tree so that unit tests can print out the contents
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return nullptr;
}

