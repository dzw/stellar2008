#pragma once
#ifndef TERRAIN_TERRAINTILE_H
#define TERRAIN_TERRAINTILE_H
//------------------------------------------------------------------------------
/**
	一个adt文件资源，用来加载ADT文件。
	一个地形tile,管理tile上的所有数据

	在可见性判断时可以在这里增加一个保存上次可见chunk的列表，当摄像机没有改变时
	可以直接渲染上次保存的chunk列表。
*/
#include "models/model.h"
#include "Terrain/terraindef.h"
#include "Terrain/terrainnode.h"

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
	/// 
	const Ptr<TerrainNode>& GetChunk(int x, int z);
	/// Add a chunk to render
	void AddRenderChunk(int x, int z);
	/// 测试用的，渲染整个tile
	void AddAllChunk();
	int GetX()const;
	int GetZ()const;
	void SetXZ(int x, int z);
	Math::float2 GetPos()const;
protected:
	/// 16*16个chunk
	//Ptr<TerrainChunk> chunk[TILECHUNKSIZE*TILECHUNKSIZE];
	/// buliding
	//Util::Array<Ptr<InteriorEntity> interiors;
    //Util::Array<Ptr<ModelEntity> models;
    //Util::Array<String> textureNames;
    //Util::Array<String> interiorNames;
    //Util::Array<String> modelNames;

	int x, z;
};

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainTile::GetX()const
{
	return this->x;
}

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainTile::GetZ()const
{
	return this->z;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TerrainTile::SetXZ(int x, int z)
{
	this->x = x;
	this->z = z;
}

inline Math::float2 
TerrainTile::GetPos()const
{
	return Math::float2(this->x * TILESIZE, this->z * TILESIZE);
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    