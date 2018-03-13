///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "SpatialPartition.hpp"
#include "Shapes.hpp"

class DynamicAabbTree;

//--------------------------------------------------------------------DynamicAabbTreeNode
class DynamicAabbTreeNode
{
public:
  DynamicAabbTreeNode* GetParent() const;
  DynamicAabbTreeNode* GetLeftChild() const;
  DynamicAabbTreeNode* GetRightChild() const;
  Aabb GetAabb() const;
  void* GetClientData() const;
  int GetHeight() const;
private:
  void UpdateAabb();
  void UpdateHeight();

  DynamicAabbTreeNode * mParent;
  DynamicAabbTreeNode * mLeftChild;
  DynamicAabbTreeNode * mRightChild;
  Aabb mAabb;
  void * mClientData;
  size_t mHeight;
  size_t mLastAxis;
  friend DynamicAabbTree;
};

//--------------------------------------------------------------------DynamicAabbTree
/******Student:Assignment3******/
/// You must implement a dynamic aabb tree as we discussed in class.
class DynamicAabbTree : public SpatialPartition
{
public:

  DynamicAabbTree();
  ~DynamicAabbTree();

  // Spatial Partition Interface
  void InsertData(SpatialPartitionKey& key, const SpatialPartitionData& data) override;
  void UpdateData(SpatialPartitionKey& key, const SpatialPartitionData& data) override;
  void RemoveData(SpatialPartitionKey& key) override;

  void DebugDraw(int level, const Math::Matrix4& transform, const Vector4& color = Vector4(1), int bitMask = 0) override;

  void CastRay(const Ray& ray, CastResults& results) override;
  void CastFrustum(const Frustum& frustum, CastResults& results) override;

  void SelfQuery(QueryResults& results) override;

  DynamicAabbTreeNode* GetRoot() const;
  // A fattening factor to use for insertion to prevent jitter from causing updates
  static const float mFatteningFactor;
private:
  
  // Inserts new_node into the tree and returns the new height of node
  void InsertIntoTree(DynamicAabbTreeNode * node, DynamicAabbTreeNode * new_node);
  // Balances the tree if necessary.
  bool Balance(DynamicAabbTreeNode * node);
  void Rotate(DynamicAabbTreeNode * old_parent, DynamicAabbTreeNode * pivot);
  // Updates the aabb and height of each node from givent node to root
  void BalanceAndUpdateFromBottom(DynamicAabbTreeNode * node);
  

  void CastRay(DynamicAabbTreeNode * node, const Ray & ray, CastResults & results);
  void CastFrustum(DynamicAabbTreeNode * node, const Frustum & frustum, CastResults & results);
  // adds all leaf node client data below given node to results
  void AddLeafNodes(DynamicAabbTreeNode * node, CastResults & results);

  void Query(DynamicAabbTreeNode * a, DynamicAabbTreeNode * b, QueryResults & results);

  DynamicAabbTreeNode * mRoot;

};
