#pragma once
#ifndef KOK_TERRAIN_H
#define KOK_TERRAIN_H
//------------------------------------------------------------------------------
/**
    @class KOK::Terrain

    (C) 2008 cTuo
*/
#include "models/model.h"
#include "kok/terrain/terraindef.h"
#include "kok/terrain/terraininfo.h"
#include "kok/terrain/terrainmeshgrid.h"
#include "kok/terrain/clifftable.h"
#include "kok/terrain/districtnode.h"
#include "kok/terrain/terrainrender.h"
#include "graphics/cell.h"
#include "kok/model/thingentity.h"

//------------------------------------------------------------------------------
namespace KOK
{
	// ��Ⱦ��������
struct DrawTile 
{
	Ptr<DistrictNode> dist;
	int tex;
	int pass;

	friend bool operator <(const DrawTile& l, const DrawTile& r);
	friend bool operator >(const DrawTile& l, const DrawTile& r);
	friend bool operator <=(const DrawTile& l, const DrawTile& r);
};

inline bool 
	operator <(const DrawTile& l, const DrawTile& r)
{
	if (l.tex <= r.tex)
		return l.dist->GetSquareDistance() <= r.dist->GetSquareDistance();

	return false;//l.dist->GetZ() <= r.dist->GetZ();
}

inline bool 
	operator >(const DrawTile& l, const DrawTile& r)
{
	if (l.tex > r.tex)
		return true;

	if (l.tex == r.tex)
		return l.dist->GetSquareDistance() >= r.dist->GetSquareDistance();

	return false;//l.dist->GetZ() <= r.dist->GetZ();
}

inline bool 
	operator <=(const DrawTile& l, const DrawTile& r)
{
	if (r.tex < l.tex)
		return false;
	else if (r.tex > l.tex)
		return true;
	else if (r.tex == l.tex)
		return l.dist->GetSquareDistance() <= r.dist->GetSquareDistance();	// ����

	return false;
}

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

	/// �����µ�district
	Ptr<DistrictNode> CreateNewDistrict(int x, int z);
	/// �����Ĳ����߶�
	void CreateQuadTree(const Ptr<Graphics::Cell>& root);

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
	
	void AppendTexture(const Util::String& resId);
	const Util::Array<Ptr<Resources::ManagedTexture>>& GetTextures()const;
	int GetTextureCount()const;
	TerrainMeshData** GetMeshData()const;
	const TerrainInfo& GetTerrainInfo()const;
	const Ptr<TerrainMeshGrid>& GetTerrainMeshGrid()const;
	
	void UpdateVertexPool();
	virtual void Render(const Models::ModelNodeType::Code& nodeFilter, const Frame::LightingMode::Code& lightingMode, CoreGraphics::ShaderFeature::Mask& shaderFeatures);

	void AddVisibleDistrict(IndexT id);
	void UpdateRenderList(IndexT frameIndex);

	void SetStage(const Ptr<Graphics::Stage>& stage);
	void UpdateThing();
protected:
	friend class TerrainReader;

	void SetMapSize(SizeT size);
	void ComputeTileMesh();
	void UpdateTileTex(int iX, int iY);


	/// ������Ϣ
    TerrainInfo					terrInfo;
	/// ������Ϣ
	Ptr<TerrainMeshGrid>		terrMeshGrid;
	/// ���±�
	Ptr<CliffTable>				cliffTable;
	/// �ر������ͼID
	ThingTex**					thingTex;
	/// ������
	Util::Array<Ptr<Resources::ManagedTexture>> textures;
	/// ���θ�������
	TerrainMeshData**			meshData;
	/// �Ŵ���
	int							tileMeshScale;
	/// ����tileλ��offset
	float						tilePosOffset;
	

	Ptr<TerrainRender>			terrRender;
	Util::Array<Ptr<DistrictNode>> renderList;
	/// ��Ⱦ�������
	Util::Array<DrawTile>		renderLayer[4];
	DWORD						frameIndex;

	/// �ݴ�δ���ص�thing����б�
	Util::Array<Ptr<ThingEntity>> thingEntitys;
	Ptr<Graphics::Stage> stage;
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

    