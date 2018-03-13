///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//----------------------------------------------------------DynamicAabbTreeNode
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

void DynamicAabbTreeNode::UpdateAabb()
{
  mAabb = Aabb::Combine(mLeftChild->mAabb, mRightChild->mAabb);
}

void DynamicAabbTreeNode::UpdateHeight()
{
  size_t child_height = Math::Max(mLeftChild->mHeight, mRightChild->mHeight);
  mHeight = child_height + 1;
}

//--------------------------------------------------------------DynamicAabbTree
const float DynamicAabbTree::mFatteningFactor = 1.1f;

DynamicAabbTree::DynamicAabbTree()
{
  mType = SpatialPartitionTypes::AabbTree;
  mRoot = nullptr;
}

DynamicAabbTree::~DynamicAabbTree()
{
}

void DynamicAabbTree::InsertData(SpatialPartitionKey& key, 
  const SpatialPartitionData& data)
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

void DynamicAabbTree::UpdateData(SpatialPartitionKey& key, 
  const SpatialPartitionData& data)
{
  DynamicAabbTreeNode * update_node = (DynamicAabbTreeNode *)key.mVoidKey;
  const Aabb & aabb = update_node->mAabb;
  // reinsert if new aabb is not fully contained within old aabb
  if (!aabb.Contains(data.mAabb))
  {
    RemoveData(key);
    InsertData(key, data);
  }
}

void DynamicAabbTree::RemoveData(SpatialPartitionKey& key)
{
  DynamicAabbTreeNode * remove_node = (DynamicAabbTreeNode *)key.mVoidKey;
  // node is root
  if (remove_node == mRoot)
  {
    delete mRoot;
    mRoot = nullptr;
    return;
  }
  // node to replace empty node above remove_node
  DynamicAabbTreeNode * replace_node;
  DynamicAabbTreeNode * empty_node = remove_node->mParent;
  // find replace node
  if (remove_node == empty_node->mLeftChild)
    replace_node = empty_node->mRightChild;
  else
    replace_node = empty_node->mLeftChild;
  // connect replace node
  replace_node->mParent = empty_node->mParent;
  if (empty_node == mRoot)
  {
    // replace node becomes root
    mRoot = replace_node;
  }
  else
  {
    // replace node takes empty nodes place
    if(empty_node->mParent->mLeftChild == empty_node)
      empty_node->mParent->mLeftChild = replace_node;
    else
      empty_node->mParent->mRightChild = replace_node;
  }
  delete empty_node;
  delete remove_node;
  BalanceAndUpdateFromBottom(replace_node->mParent);
}

void DynamicAabbTree::DebugDraw(int level, const Math::Matrix4& transform, 
  const Vector4& color, int bitMask)
{
  /******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
}

void DynamicAabbTree::CastRay(const Ray& ray, CastResults& results)
{
  if(mRoot)
    CastRay(mRoot, ray, results);
}

void DynamicAabbTree::CastFrustum(const Frustum& frustum, CastResults& results)
{
  if(mRoot)
    CastFrustum(mRoot, frustum, results);
}

void DynamicAabbTree::SelfQuery(QueryResults& results)
{
  // the first thing you are going to want to do is see if
  // the two children intersect
}

DynamicAabbTreeNode* DynamicAabbTree::GetRoot() const
{/******Student:Assignment3******/
  Warn("Assignment3: Required function un-implemented");
  return mRoot;
}


void DynamicAabbTree::InsertIntoTree(DynamicAabbTreeNode * node, 
  DynamicAabbTreeNode * new_node)
{
  // leaf node
  if (node->mHeight == 0)
  {
    // create and set up empty node
    DynamicAabbTreeNode * empty_node = new DynamicAabbTreeNode;
    empty_node->mAabb = Aabb::Combine(node->mAabb, new_node->mAabb);
    empty_node->mClientData = nullptr;
    empty_node->mHeight = 1;
    empty_node->mParent = node->mParent;
    empty_node->mLeftChild = node;
    empty_node->mRightChild = new_node;
    // set root 
    if(node == mRoot)
    {
      mRoot = empty_node;
    }
    else
    {
      // attach parent to new empty node
      if(node->mParent->mLeftChild == node)
        node->mParent->mLeftChild = empty_node;
      else
        node->mParent->mRightChild = empty_node;
    }
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
    return;
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
  if (delta_l_sa < delta_r_sa)
  {
    InsertIntoTree(node->mLeftChild, new_node);
    node->mAabb = left_new_combine;
  }
  else
  {
    InsertIntoTree(node->mRightChild, new_node);
    node->mAabb = right_new_combine;
  }
  // balance if necessary
  bool rotated = Balance(node);
  // update aabb and height of node
  // Balance will handle this if node is rotated
  if (!rotated)
  {
    node->UpdateAabb();
    node->UpdateHeight();
  }
}

bool DynamicAabbTree::Balance(DynamicAabbTreeNode * node)
{
  size_t left_height = node->mLeftChild->mHeight;
  size_t right_height = node->mRightChild->mHeight;
  // preform rotation if necessary
  if(left_height > (right_height + 1))
  {
    Rotate(node, node->mLeftChild);
    return true;
  }
  else if(right_height > (left_height + 1))
  {
    Rotate(node, node->mRightChild);
    return true;
  }
  return false;
}

void DynamicAabbTree::Rotate(DynamicAabbTreeNode * old_parent, 
  DynamicAabbTreeNode * pivot)
{
  // find small child of pivot
  size_t left_height = pivot->mLeftChild->mHeight;
  size_t right_height = pivot->mRightChild->mHeight;
  DynamicAabbTreeNode * small_child;
  if(left_height < right_height)
    small_child = pivot->mLeftChild;
  else
    small_child = pivot->mRightChild;
  // connect pivot to old parent's parent
  if (old_parent == mRoot)
  {
    mRoot = pivot;
    pivot->mParent = nullptr;
  }
  else
  {
    pivot->mParent = old_parent->mParent;
    if(old_parent->mParent->mLeftChild == old_parent)
      old_parent->mParent->mLeftChild = pivot;
    else
      old_parent->mParent->mRightChild = pivot;
  }
  // connect old parent to pivot
  old_parent->mParent = pivot;
  if(pivot->mLeftChild == small_child)
    pivot->mLeftChild = old_parent;
  else
    pivot->mRightChild = old_parent;
  // connect small child to old parent
  small_child->mParent = old_parent;
  if(old_parent->mLeftChild == pivot)
    old_parent->mLeftChild = small_child;
  else
    old_parent->mRightChild = small_child;
  // update height of old parent then pivot
  // pivot depends on old parent
  old_parent->UpdateAabb();
  old_parent->UpdateHeight();
  pivot->UpdateAabb();
  pivot->UpdateHeight();
}

void DynamicAabbTree::BalanceAndUpdateFromBottom(DynamicAabbTreeNode * node)
{
  // top of tree reached
  if(node == nullptr)
    return;
  // perform balance if necessary
  bool rotated = Balance(node);
  // update aabb and height if rotation was not performed
  if (!rotated)
  {
    node->UpdateAabb();
    node->UpdateHeight();
  }
  BalanceAndUpdateFromBottom(node->mParent);
}

void DynamicAabbTree::CastRay(DynamicAabbTreeNode * node, const Ray & ray, 
  CastResults & results)
{
  // check for intersection
  float t;
  bool intersect = RayAabb(ray.mStart, ray.mDirection, 
    node->mAabb.mMin, node->mAabb.mMax, t);
  if(!intersect)
    return;
  if (node->mHeight == 0)
  {
    // add client data at a leaf node
    CastResult result(node->mClientData, t);
    results.AddResult(result);
  }
  else
  {
    // cast ray down children at empty node
    CastRay(node->mLeftChild, ray, results);
    CastRay(node->mRightChild, ray, results);
  }
}

void DynamicAabbTree::CastFrustum(DynamicAabbTreeNode * node,
  const Frustum & frustum, CastResults & results)
{
  // find intersection type
  IntersectionType::Type intersection_type;
  intersection_type = FrustumAabb(frustum.GetPlanes(), 
    node->mAabb.mMin, node->mAabb.mMax, node->mLastAxis);
  // exit on no intersection
  if(intersection_type < IntersectionType::Inside)
    return;
  // add all leaf nodes if aabb is completely contained
  if(intersection_type ==  IntersectionType::Inside)
  {
    AddLeafNodes(node, results);
    return;
  }
  if (node->mHeight == 0)
  {
    // at a leaf node
    CastResult result(node->mClientData);
    results.AddResult(result);
  }
  else
  {
    // recurse down children
    CastFrustum(node->mLeftChild, frustum, results);
    CastFrustum(node->mRightChild, frustum, results);
  }
}

void DynamicAabbTree::AddLeafNodes(DynamicAabbTreeNode * node, 
  CastResults & results)
{
  if (node->mHeight == 0)
  {
    // add result
    CastResult result(node->mClientData);
    results.AddResult(result);
  }
  else
  {
    // recurse down children
    AddLeafNodes(node->mLeftChild, results);
    AddLeafNodes(node->mRightChild, results);
  }
}

void DynamicAabbTree::Query(DynamicAabbTreeNode * a, DynamicAabbTreeNode * b, 
  QueryResults & results)
{
  bool intersection;
  intersection = AabbAabb(a->mAabb.mMin, a->mAabb.mMax,
    b->mAabb.mMin, b->mAabb.mMax);
  if (a->mHeight == 0)
  {
    if (b->mHeight == 0)
    {
      if (!intersection)
        return;
      QueryResult result(a->mClientData, b->mClientData);
      results.AddResult(result);
      return;
    }
    Query(a, b->mLeftChild, results);
    Query(a, b->mRightChild, results);
    return;
  }
  if (b->mHeight == 0)
  {
    Query(b, a->mLeftChild, results);
    Query(b, a->mRightChild, results);
    return;
  }
  Query(a, b->mLeftChild, results);
  Query(a, b->mRightChild, results);
  Query(b, a->mLeftChild, results);
  Query(b, a->mRightChild, results);
}



