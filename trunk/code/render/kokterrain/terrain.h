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
#include "kokterrain/terrainrender.h"

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

	/// district����任����������
	SizeT DistrictPos2TilePos(SizeT x, SizeT z);
	/// ����ƫ��
	float GetTilePosOffset()const;
	/// ���ظ��ӵĴ�С
	int GetTileSize()const;
	/// ���ص�ͼ���
	SizeT GetMapWide();
	/// ���ظ��Ӹ߶�
	float GetHight(int x, int z);
	/// ����һ��district�Ĵ�С
	int GetDistrictSize()const;
	
	const Util::Array<Ptr<Resources::ManagedTexture>>& GetTextures()const;
	void AppendTexture(const Util::String& resId);
	int GetTextureCount()const;

	TerrainMeshData** GetMeshData()const;
	const TerrainInfo& GetTerrainInfo()const;

	void SetMapSize(SizeT size);

	///
	const Ptr<TerrainMeshGrid>& GetTerrainMeshGrid()const;
	/// �����µ�district
	Ptr<DistrictNode> CreateNewDistrict(int x, int z);

	virtual void Render(const Models::ModelNodeType::Code& nodeFilter, const Frame::LightingMode::Code& lightingMode, CoreGraphics::ShaderFeature::Mask& shaderFeatures);
protected:
	friend class TerrainReader;

	void UpdateTileTex(int iX, int iY);

	/// ������Ϣ
    TerrainInfo					terrInfo;
	Ptr<TerrainMeshGrid>		terrMeshGrid;
	Ptr<CliffTable>				cliffTable;
	//Util::Array<Util::String>	textureLayer;
	Util::Array<Ptr<Resources::ManagedTexture>> textures;

	/// ���ηŴ�
	TerrainMeshData**			meshData;
	/// �Ŵ���
	int							tileMeshScale;
	/// ����tileλ��offset
	float						tilePosOffset;
	
	/// �ر������ͼID
	ThingTex**					thingTex;
	Ptr<TerrainRender>			terrRender;
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

inline const Util::Array<Ptr<Resources::ManagedTexture>>& 
Terrain::GetTextures()const
{
	return this->textures;
}

inline int 
Terrain::GetTextureCount()const
{
	return this->textures.Size();
}

inline TerrainMeshData** 
Terrain::GetMeshData()const
{
	return this->meshData;
}

inline const TerrainInfo&
Terrain::GetTerrainInfo()const
{
	return this->terrInfo;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    