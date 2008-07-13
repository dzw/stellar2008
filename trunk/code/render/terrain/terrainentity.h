#pragma once
#ifndef TERRAIN_TERRAINENTITY_H
#define TERRAIN_TERRAINENTITY_H
//------------------------------------------------------------------------------
/**
*/
#include "graphics/graphicsentity.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainEntity : public Graphics::GraphicsEntity
{
    DeclareClass(TerrainEntity);
public:
    /// constructor
    TerrainEntity();
    /// destructor
    virtual ~TerrainEntity();

	/// 创建渲染块
	void CreateTerrainCacha(SizeT num);
	/// 加入需要渲染的块
	//void AddChunkToRender(const Ptr<TerrainChunk>& chunk);

protected:
	Ptr<Models::Model> terrain;
	Ptr<Models::ModelInstance> terrainInstance;

	SizeT curChunkNum;
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    