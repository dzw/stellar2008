#pragma once
#ifndef TERRAIN_TERRAINTILE_H
#define TERRAIN_TERRAINTILE_H
//------------------------------------------------------------------------------
/**
	一个adt文件资源，用来加载ADT文件。
	一个地形tile,管理tile上的所有数据
*/
#include "resources/resource.h"
#include "terrain/terrainchunk.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainTile : public Models::Model
{
    DeclareClass(TerrainTile);
public:
	TerrainTile();
	~TerrainTile();

	virtual void Unload();

protected:
	/// 16*16个chunk
	//Ptr<TerrainChunk> chunk[TILECHUNKSIZE*TILECHUNKSIZE];
	/// buliding
	//Util::Array<Ptr<InteriorEntity> interiors;
    //Util::Array<Ptr<ModelEntity> models;
    //Util::Array<String> textureNames;
    //Util::Array<String> interiorNames;
    //Util::Array<String> modelNames;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    