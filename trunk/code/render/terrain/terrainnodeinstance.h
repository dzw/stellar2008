#pragma once
#ifndef TERRAIN_TERRAINCHUNKNODEINSTANCE_H
#define TERRAIN_TERRAINCHUNKNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    渲染一个地形块，在这里设置四层地形纹理，从TerrainChunkNode中取得空闲缓冲
	并填充顶点数据。

    (C)  cTuo
*/
#include "models/nodes/transformnodeinstance.h"
#include "math/vector.h"
#include "math/point.h"
#include "math/quaternion.h"
#include "math/transform44.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainChunkNodeInstance : public TransformNodeInstance
{
    DeclareClass(TerrainChunkNodeInstance);
public:
    /// constructor
    TerrainChunkNodeInstance();
    /// destructor
    virtual ~TerrainChunkNodeInstance();

	///
	friend bool operator <= (const TerrainChunkNodeInstance& a, const TerrainChunkNodeInstance& b);
	/// perform rendering
	virtual void Render();
	/// apply per-instance state prior to rendering
	virtual void ApplyState();
protected:
	/// notify that we are visible
	virtual void OnNotifyVisible(IndexT frameIndex);
	/// called when attached to ModelInstance
	virtual void OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst);
	/// called when removed from ModelInstance
	virtual void OnRemoveFromModelInstance();

	bool isLoaded;	// chunk is loaded
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
operator <= (const TerrainChunkNodeInstance& a, const TerrainChunkNodeInstance& b)
{
	return a.GetPosition().z() > b.GetPosition().z();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
