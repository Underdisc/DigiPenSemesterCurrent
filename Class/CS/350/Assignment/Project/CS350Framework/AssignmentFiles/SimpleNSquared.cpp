///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"

//-----------------------------------------------------------------------------NSquaredSpatialPartition
NSquaredSpatialPartition::NSquaredSpatialPartition()
{
  mType = SpatialPartitionTypes::NSquared;
  mCurrentId = 0;
}

void NSquaredSpatialPartition::InsertData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  // Doing this lazily (and bad, but it's n-squared...).
  // Just store an ever incrementing id as a key along with the client data so we can look it up later.
  key.mUIntKey = mCurrentId;
  Item item;
  item.mClientData = data.mClientData;
  item.mKey = key.mUIntKey;

  mData.push_back(item);

  ++mCurrentId;
}

void NSquaredSpatialPartition::UpdateData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  // Nothing to do here, there's no spatial partition data to update
}

void NSquaredSpatialPartition::RemoveData(SpatialPartitionKey& key)
{
  // Find the key data and remove it
  for(size_t i = 0; i < mData.size(); ++i)
  {
    if(mData[i].mKey == key.mUIntKey)
    {
      mData[i] = mData.back();
      mData.pop_back();
      break;
    }
  }
}

void NSquaredSpatialPartition::DebugDraw(int level, const Math::Matrix4& transform, const Vector4& color, int bitMask)
{
  // Nothing to debug draw
}

void NSquaredSpatialPartition::CastRay(const Ray& ray, CastResults& results)
{
  // Add everything
  for(size_t i = 0; i < mData.size(); ++i)
  {
    CastResult result;
    result.mClientData = mData[i].mClientData;
    results.AddResult(result);
  }
}

void NSquaredSpatialPartition::CastFrustum(const Frustum& frustum, CastResults& results)
{
  // Add everything
  for(size_t i = 0; i < mData.size(); ++i)
  {
    CastResult result;
    result.mClientData = mData[i].mClientData;
    results.AddResult(result);
  }
}

void NSquaredSpatialPartition::SelfQuery(QueryResults& results)
{
  // Add everything
  for(size_t i = 0; i < mData.size(); ++i)
  {
    for(size_t j = i + 1; j < mData.size(); ++j)
    {
      results.AddResult(QueryResult(mData[i].mClientData, mData[j].mClientData));
    }
  }
}

void NSquaredSpatialPartition::GetDataFromKey(const SpatialPartitionKey& key, SpatialPartitionData& data) const
{
  // Find the key data and remove it
  for(size_t i = 0; i < mData.size(); ++i)
  {
    if(mData[i].mKey == key.mUIntKey)
    {
      data.mClientData = mData[i].mClientData;
    }
  }
  
}

void NSquaredSpatialPartition::FilloutData(std::vector<SpatialPartitionQueryData>& results) const
{
  for(size_t i = 0; i < mData.size(); ++i)
  {
    SpatialPartitionQueryData data;
    data.mClientData = mData[i].mClientData;
    results.push_back(data);
  }
}

//-----------------------------------------------------------------------------BoundingSphereSpatialPartition
BoundingSphereSpatialPartition::BoundingSphereSpatialPartition()
{
  mType = SpatialPartitionTypes::NSquaredSphere;
}

void BoundingSphereSpatialPartition::InsertData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

void BoundingSphereSpatialPartition::UpdateData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

void BoundingSphereSpatialPartition::RemoveData(SpatialPartitionKey& key)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

void BoundingSphereSpatialPartition::DebugDraw(int level, const Math::Matrix4& transform, const Vector4& color, int bitMask)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

void BoundingSphereSpatialPartition::CastRay(const Ray& ray, CastResults& results)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

void BoundingSphereSpatialPartition::CastFrustum(const Frustum& frustum, CastResults& results)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

void BoundingSphereSpatialPartition::SelfQuery(QueryResults& results)
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}

void BoundingSphereSpatialPartition::FilloutData(std::vector<SpatialPartitionQueryData>& results) const
{
  /******Student:Assignment2******/
  Warn("Assignment2: Required function un-implemented");
}
