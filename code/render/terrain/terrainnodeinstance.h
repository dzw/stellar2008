#pragma once
#ifndef TERRAIN_TERRAINNODEINSTANCE_H
#define TERRAIN_TERRAINNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    渲染一个地形块，在这里设置四层地形纹理，从TerrainChunkNode中取得空闲缓冲
	并填充顶点数据。

    (C)  cTuo
*/
#include "models/nodes/statenodeinstance.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainNodeInstance : public Models::StateNodeInstance
{
    DeclareClass(TerrainNodeInstance);
public:
    /// constructor
    TerrainNodeInstance();
    /// destructor
    virtual ~TerrainNodeInstance();

	/// perform rendering
	virtual void Render();
protected:
	/// notify that we are visible
	virtual void OnNotifyVisible(IndexT frameIndex);
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
