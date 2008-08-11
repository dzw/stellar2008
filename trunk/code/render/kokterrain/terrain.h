#pragma once
#ifndef KOK_TERRAIN_H
#define KOK_TERRAIN_H
//------------------------------------------------------------------------------
/**
    @class KOK::Terrain

    (C) 2008 cTuo
*/
#include "models/model.h"
#include "kokterrain/terraindef.h"
#include "kokterrain/terraininfo.h"
#include "kokterrain/terrainmeshgrid.h"
#include "kokterrain/clifftable.h"
#include "kokterrain/districtnode.h"

//------------------------------------------------------------------------------
namespace KOK
{

class Terrain : public Models::Model
{
    DeclareClass(Terrain);
public:
    /// constructor
    Terrain();
    /// destructor
    virtual ~Terrain();

	virtual Ptr<Models::ModelInstance> CreateInstance();
    /// unload the resource, or cancel the pending load
    virtual void Unload();

	void ComputeTileMesh();

	/// district坐标变换到格子坐标
	SizeT DistrictPos2TilePos(SizeT x, SizeT z);
	/// 格子偏移
	float GetTilePosOffset()const;
	/// 返回格子的大小
	int GetTileSize()const;
	/// 返回地图宽度
	SizeT GetMapWide();
	/// 返回格子高度
	float GetHight(int x, int z);
	/// 返回一个district的大小
	int GetDistrictSize()const;
	

	void SetMapSize(SizeT size);

	///
	const Ptr<TerrainMeshGrid>& GetTerrainMeshGrid()const;
	///
	Ptr<DistrictNode> CreateNewDistrict(int x, int z);
protected:
	friend class TerrainReader;

	void UpdateTileTex(int iX, int iY);

	/// 基础信息
    TerrainInfo					terrInfo;
	Ptr<TerrainMeshGrid>		terrMeshGrid;
	Ptr<CliffTable>				cliffTable;
	Util::Array<Util::String>	textureLayer;

	/// 地形放大
	TerrainMeshData**			meshData;
	int							tileMeshScale;		// 放大倍数
	float						tilePosOffset;		// 计算tile位置offset

	ThingTex**					thingTex;			// 地表各层贴图ID

	
};

inline const Ptr<TerrainMeshGrid>& 
Terrain::GetTerrainMeshGrid()const
{
	return this->terrMeshGrid;
}

inline SizeT 
Terrain::DistrictPos2TilePos(SizeT x, SizeT z)
{
	return x*this->terrInfo.GetTileCountX()+z;
}

inline float 
Terrain::GetTilePosOffset()const
{
	return this->tilePosOffset;
}

inline int
Terrain::GetTileSize()const
{
	return this->tileMeshScale * COMP;
}

inline int
Terrain::GetDistrictSize()const
{
	return GetTileSize()*DISTRICT_VERTS;
}

inline SizeT 
Terrain::GetMapWide()
{
	return this->terrInfo.GetTileCountX();
}



} // namespace Models
//------------------------------------------------------------------------------
#endif

    