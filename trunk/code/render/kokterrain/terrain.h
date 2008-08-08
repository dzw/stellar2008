#pragma once
#ifndef KOK_TERRAIN_H
#define KOK_TERRAIN_H
//------------------------------------------------------------------------------
/**
    @class KOK::Terrain

    (C) 2008 cTuo
*/
#include "models/model.h"
#include "coregraphics/meshpool.h"
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

    /// unload the resource, or cancel the pending load
    virtual void Unload();

	void ComputeTileMesh();

	/// district����任����������
	SizeT DistrictPos2TilePos(SizeT x, SizeT z);
	/// ����ƫ��
	float GetTilePosOffset()const;
	/// ���ظ��ӵĴ�С
	int GetTileSize()const;
	/// ���ص�ͼ����
	SizeT GetMapWide();
	/// ���ظ��Ӹ߶�
	float GetHight(int x, int z);
	/// ����һ��district�Ĵ�С
	int GetDistrictSize()const;

	/// ���㻺��
	const Ptr<CoreGraphics::MeshPool>& GetMeshPool()const;
	/// ���»�����
	void UpdateMeshPool();

	void SetMapSize(SizeT size);

	bool CheckValidDistrict(int x, int z);
	void CheckDistrict(const Math::vector& pos);
	void EnterDistrict(int x, int z);
	Ptr<DistrictNode> LoadDistrict(int x, int z);
protected:
	friend class TerrainReader;

	void UpdateTileTex(int iX, int iY);

	Ptr<CoreGraphics::MeshPool> distMeshPool;

	/// ������Ϣ
    TerrainInfo					terrInfo;
	Ptr<TerrainMeshGrid>		terrMeshGrid;
	Ptr<CliffTable>				cliffTable;
	Util::Array<Util::String>	textureLayer;

	/// ���ηŴ�
	TerrainMeshData**			meshData;
	int							tileMeshScale;		// �Ŵ���
	float						tilePosOffset;		// ����tileλ��offset

	ThingTex**					thingTex;			// �ر�������ͼID

	Ptr<DistrictNode>			centerDist;			// ����λ�õ�District
	int							curX;				// ��ǰλ��
	int							curZ;
};

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

inline const Ptr<CoreGraphics::MeshPool>& 
Terrain::GetMeshPool()const
{
	return this->distMeshPool;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    