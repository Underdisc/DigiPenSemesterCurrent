///////////////////////////////////////////////////////////////////////////////
///
/// Authors: Joshua Davis
/// Copyright 2015, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.hpp"
#include "Geometry.hpp"

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

typedef std::pair<unsigned int, SpatialPartitionData> MapElement;
typedef std::unordered_map<unsigned int, SpatialPartitionData>::iterator MapPos;

//-----------------------------------------------------------------------------BoundingSphereSpatialPartition
BoundingSphereSpatialPartition::BoundingSphereSpatialPartition()
{
  mType = SpatialPartitionTypes::NSquaredSphere;
  mDataAdded = 0;
}

void BoundingSphereSpatialPartition::InsertData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  key.mUIntKey = mDataAdded;
  MapElement new_element(key.mUIntKey, data);
  mDataMap.insert(new_element);
  ++mDataAdded;
}

void BoundingSphereSpatialPartition::UpdateData(SpatialPartitionKey& key, const SpatialPartitionData& data)
{
  MapPos it = mDataMap.find(key.mUIntKey);
  if(it == mDataMap.end())
    return;
  // update found data
  it->second = data;
}

void BoundingSphereSpatialPartition::RemoveData(SpatialPartitionKey& key)
{
  MapPos it = mDataMap.find(key.mUIntKey);
  if (it == mDataMap.end())
    return;
  // erase found data
  mDataMap.erase(it);
}

void BoundingSphereSpatialPartition::DebugDraw(int level, const Math::Matrix4& transform, const Vector4& color, int bitMask)
{
  // draw all bounding spheres
  for(const MapElement & element : mDataMap)
  {
    const Sphere & sphere = element.second.mBoundingSphere;
    DebugShape & draw_shape = gDebugDrawer->DrawSphere(sphere);
    // apply transform, color, and mask
    draw_shape.SetTransform(transform);
    draw_shape.Color(color);
    draw_shape.SetMaskBit(bitMask);
  }
}

void BoundingSphereSpatialPartition::CastRay(const Ray& ray, CastResults& results)
{
  // check for collision with every sphere
  for (const MapElement & element : mDataMap)
  {
    float t;
    bool collision;
    const Sphere & sphere = element.second.mBoundingSphere;
    collision = RaySphere(ray.mStart, ray.mDirection, 
      sphere.mCenter, sphere.mRadius, t);
    // modify results when collision occurs
    if(collision)
      results.AddResult(CastResult(element.second.mClientData, t));
  }
}

void BoundingSphereSpatialPartition::CastFrustum(const Frustum& frustum, CastResults& results)
{
  // not currently used
  size_t last_axis;
  // check for collision with every sphere
  for (const MapElement & element : mDataMap)
  {
    IntersectionType::Type intersection_type;
    const Sphere & sphere = element.second.mBoundingSphere;
    intersection_type = FrustumSphere(frustum.GetPlanes(), sphere.mCenter,
      sphere.mRadius, last_axis);
    // modify results if sphere is inside or overlapping
    if(intersection_type >= IntersectionType::Inside)
      results.AddResult(CastResult(element.second.mClientData, 0.0f));
    
  }
}

void BoundingSphereSpatialPartition::SelfQuery(QueryResults& results)
{
  // go through all sphere pairs
  MapPos data_a = mDataMap.begin();
  while (data_a != mDataMap.end())
  {
    MapPos data_b = data_a;
    ++data_b;
    while (data_b != mDataMap.end())
    { 
      // check for collision between sphere pairs
      bool collision;
      const Sphere & s_a = data_a->second.mBoundingSphere;
      const Sphere & s_b = data_b->second.mBoundingSphere;
      collision = SphereSphere(s_a.mCenter, s_a.mRadius, 
        s_b.mCenter, s_b.mRadius);
      // modify results if collision occurs
      if (collision)
      {
        QueryResult result(data_a->second.mClientData, 
          data_b->second.mClientData);
        results.AddResult(result);
      }
      ++data_b;
    }
    ++data_a;
  }
}

void BoundingSphereSpatialPartition::FilloutData(std::vector<SpatialPartitionQueryData>& results) const
{
  for(const MapElement & element : mDataMap)
    results.push_back(SpatialPartitionQueryData(element.second));
}
