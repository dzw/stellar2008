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
class TerrainEntity : public GraphicsEntity
{
    DeclareClass(TerrainEntity);
public:
    /// constructor
    TerrainEntity();
    /// destructor
    virtual ~TerrainEntity();

	/// ������Ⱦ��
	void CreateTerrainCacha(SizeT num);
	/// ������Ҫ��Ⱦ�Ŀ�
	void AddChunkToRender(const Ptr<TerrainChunk>& chunk);

protected:
	Ptr<Model> terrain;
	Ptr<ModelInstance> terrainInstance;

	SizeT curChunkNum;
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    