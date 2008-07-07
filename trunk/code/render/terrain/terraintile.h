#pragma once
#ifndef TERRAIN_TERRAINTILE_H
#define TERRAIN_TERRAINTILE_H
//------------------------------------------------------------------------------
/**
	一个adt文件资源，用来加载ADT文件。
	一个地形tile,管理tile上的所有数据
*/
#include "resources/resource.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainTile : public Resources::Resource
{
    DeclareClass(TerrainTile);
public:
	TerrainTile();
	~TerrainTile();

	virtual void Unload();

	/// 16*16个chunk
	Util::FixedArray<Ptr<TerrainChunkEntity>> terrainChunk;
	/// buliding
	//Util::Array<Ptr<InteriorEntity> interiors;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    