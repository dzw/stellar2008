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
	// 渲染对像排序
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
		return l.dist->GetSquareDistance() <= r.dist->GetSquareDistance();	// 距离

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

	/// 创建新的district
	Ptr<DistrictNode> CreateNewDistrict(int x, int z);
	/// 调整四叉树高度
	void CreateQuadTree(const Ptr<Graphics::Cell>& root);

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


	/// 基础信息
    TerrainInfo					terrInfo;
	/// 格子信息
	Ptr<TerrainMeshGrid>		terrMeshGrid;
	/// 悬崖表
	Ptr<CliffTable>				cliffTable;
	/// 地表各层贴图ID
	ThingTex**					thingTex;
	/// 总纹理
	Util::Array<Ptr<Resources::ManagedTexture>> textures;
	/// 地形格子数据
	TerrainMeshData**			meshData;
	/// 放大倍数
	int							tileMeshScale;
	/// 计算tile位置offset
	float						tilePosOffset;
	

	Ptr<TerrainRender>			terrRender;
	Util::Array<Ptr<DistrictNode>> renderList;
	/// 渲染排序队列
	Util::Array<DrawTile>		renderLayer[4];
	DWORD						frameIndex;

	/// 暂存未加载的thing物件列表
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

    