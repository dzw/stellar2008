#pragma once
#ifndef TERRAIN_TERRAINNODEINSTANCE_H
#define TERRAIN_TERRAINNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    渲染一个地形块，在这里设置四层地形纹理，从TerrainChunkNode中取得空闲缓冲
	并填充顶点数据。

    (C)  cTuo
*/
#include "models/modelnodeinstance.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainNodeInstance : public Models::ModelNodeInstance
{
    DeclareClass(TerrainNodeInstance);
public:
    /// constructor
    TerrainNodeInstance();
    /// destructor
    virtual ~TerrainNodeInstance();

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

	Ptr<TerrainChunk> chunk;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
