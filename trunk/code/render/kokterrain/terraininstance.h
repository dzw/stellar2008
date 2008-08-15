#pragma once
#ifndef KOK_TERRAININSTANCE_H
#define KOK_TERRAININSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::TerrainInstance
    
    (C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "models/modelinstance.h"
#include "coregraphics/meshpool.h"
#include "kokterrain/districtnode.h"
#include "kokterrain/districtnodeinstance.h"
#include "kokterrain/terrain.h"

//------------------------------------------------------------------------------
namespace KOK
{
// ��Ⱦ��������
struct DrawTile 
{
	Ptr<DistrictNodeInstance> dist;
	int tex;
	int pass;

	friend bool operator <(const DrawTile& l, const DrawTile& r);
	friend bool operator >(const DrawTile& l, const DrawTile& r);
	friend bool operator <=(const DrawTile& l, const DrawTile& r);
	//{
	//	if (tex <= r.pass)
	//		return true;
	//	// �Ⱦ���
	//	return dist->GetZ() <= r.dist->GetZ();
	//}
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

class TerrainInstance : public Models::ModelInstance
{
    DeclareClass(TerrainInstance);
public:
    /// constructor
    TerrainInstance();
    /// destructor
    virtual ~TerrainInstance();

    /// perform per-frame update (after setting transform, visibility, time, etc)
    virtual void Update();

    /// render node specific debug shape
	//void RenderDebug();

	/// ���㻺��
	//const Ptr<CoreGraphics::MeshPool>& GetMeshPool()const;
	/// ���»�����
	//void UpdateMeshPool();

	bool CheckValidDistrict(int x, int z);
	void CheckDistrict(const Math::vector& pos);
	void EnterDistrict(int x, int z);
	Ptr<DistrictNodeInstance> LoadDistrict(int x, int z);

	void AppendRenderDist(const Ptr<DistrictNodeInstance>& d);
	const Util::Array<Ptr<DistrictNodeInstance>>& GetRenderList()const;
	void ClearRenderList();

	const Util::Array<DrawTile>& GetDrawList(IndexT layer)const;
	const Util::Array<bool>& GetTextureRenderCheck()const;
protected:
	virtual void OnAttachToModel(const Ptr<Models::Model>& model);
	virtual void NotifyVisible(IndexT frameIndex);

	Ptr<Terrain>				terrain;
	//Ptr<CoreGraphics::MeshPool> distMeshPool;

	Ptr<DistrictNodeInstance>	centerDist;			// ����λ�õ�District
	int							curX;				// ��ǰλ��
	int							curZ;

	Math::vector				savePos;

	/// �ɼ���Ⱦ����
	Util::Array<Ptr<DistrictNodeInstance>> renderList;
	Util::Array<Ptr<DistrictNodeInstance>> renderList1;
	/// ��Ⱦ�������
	Util::Array<DrawTile> renderLayer[4];
	/// �����Ƿ���Ҫʹ��
	Util::Array<bool> texRender;
};

//inline const Ptr<CoreGraphics::MeshPool>& 
//TerrainInstance::GetMeshPool()const
//{
//	return this->distMeshPool;
//}

inline const Util::Array<bool>& 
TerrainInstance::GetTextureRenderCheck()const
{
	return this->texRender;
}

inline const Util::Array<DrawTile>& 
TerrainInstance::GetDrawList(IndexT layer)const
{
	n_assert(layer>=0 && layer<4);
	return this->renderLayer[layer];
}

inline void 
TerrainInstance::ClearRenderList()
{
	renderList.Clear();
}

inline const Util::Array<Ptr<DistrictNodeInstance>>& 
TerrainInstance::GetRenderList()const
{
	return this->renderList1;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
