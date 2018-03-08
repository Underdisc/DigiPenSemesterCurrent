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
  mRoot = nullptr;
}

DynamicAabbTree::~DynamicAabbTree()
{
}

void DynamicAabbTree::InsertData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  /******Student:Assignment3******/
  // we are going to need to traverse down the correct side using the surface area
  // heuristic.
  // start at le root sir
  // roger

  // create new node
  DynamicAabbTreeNode * new_node = new DynamicAabbTreeNode;
  new_node->mAabb = data.mAabb;
  new_node->mClientData = data.mClientData;
  // add node to structure
  if(!mRoot)
  {
    mRoot = new_node;
  }
  else
  {
    InsertIntoTree(new_node);
  }
  key.mVoidKey = (void *)new_node;

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
  return mRoot;
}


void DynamicAabbTree::InsertIntoTree(DynamicAabbTreeNode * node, DynamicAabbTreeNode * new_node)
{
  //cases
  // we have two children // choose
  // we have one child // actually tho?
  // we have no children


  if(node->)
}



