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
  return mParent;
}

DynamicAabbTreeNode* DynamicAabbTreeNode::GetLeftChild() const
{
  return mLeftChild;
}

DynamicAabbTreeNode* DynamicAabbTreeNode::GetRightChild() const
{
  return mRightChild;
}

Aabb DynamicAabbTreeNode::GetAabb() const
{
  return mAabb;
}

void* DynamicAabbTreeNode::GetClientData() const
{
  return mClientData;
}

int DynamicAabbTreeNode::GetHeight() const
{
  return mHeight;
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
  // create new node
  DynamicAabbTreeNode * new_node = new DynamicAabbTreeNode;
  new_node->mAabb = data.mAabb;
  new_node->mAabb.Fatten(mFatteningFactor);
  new_node->mClientData = data.mClientData;

  // add node to structure
  if(!mRoot)
  {
    // first node
    new_node->mHeight = 0;
    new_node->mLeftChild = nullptr;
    new_node->mRightChild = nullptr;
    new_node->mParent = nullptr;
    mRoot = new_node;
  }
  else
  {
    InsertIntoTree(mRoot, new_node);
  }
  key.mVoidKey = (void *)new_node;
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


size_t DynamicAabbTree::InsertIntoTree(DynamicAabbTreeNode * node, DynamicAabbTreeNode * new_node)
{
  // leaf node
  if (node->mHeight == 0)
  {
    // create and set up empty node
    DynamicAabbTreeNode * empty_node = new DynamicAabbTreeNode;
    empty_node->mAabb = Aabb::Combine(node->mAabb, new_node->mAabb);
    empty_node->mClientData = nullptr;
    empty_node->mHeight = 1;
    empty_node->mParent = nullptr;
    empty_node->mLeftChild = node;
    empty_node->mRightChild = new_node;

    // quick fix
    if(node == mRoot)
      mRoot = empty_node;

    // set up left node
    node->mParent = empty_node;
    node->mLeftChild = nullptr;
    node->mRightChild = nullptr;
    node->mHeight = 0;
    // set up right node
    new_node->mParent = empty_node;
    new_node->mLeftChild = nullptr;
    new_node->mRightChild = nullptr;
    new_node->mHeight = 0;
    // return height of 1
    return 1;
  }
  // not a leaf
  // compute new aabbs
  const Aabb & left_aabb = node->mLeftChild->mAabb;
  const Aabb & right_aabb = node->mRightChild->mAabb;
  Aabb left_new_combine = Aabb::Combine(left_aabb, new_node->mAabb);
  Aabb right_new_combine = Aabb::Combine(right_aabb, new_node->mAabb);
  // compute change in surface area
  float l_sa = left_aabb.GetSurfaceArea();
  float r_sa = right_aabb.GetSurfaceArea();
  float delta_l_sa = left_new_combine.GetSurfaceArea() - l_sa;
  float delta_r_sa = right_new_combine.GetSurfaceArea() - r_sa;
  // go down correct node
  size_t child_height;
  if (delta_l_sa < delta_r_sa)
  {
    child_height = InsertIntoTree(node->mLeftChild, new_node);
  }
  else
  {
    child_height = InsertIntoTree(node->mRightChild, new_node);
  }
  // return node's new height
  size_t node_height = child_height + 1;
  if (node_height > node->mHeight)
  {
    node->mHeight = node_height;
  }
  return node->mHeight;

  // all you need to do now is rotate the tree if the balance is fucked
}



