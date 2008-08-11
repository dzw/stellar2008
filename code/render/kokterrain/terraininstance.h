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
	const Ptr<CoreGraphics::MeshPool>& GetMeshPool()const;
	/// ���»�����
	void UpdateMeshPool();

	bool CheckValidDistrict(int x, int z);
	void CheckDistrict(const Math::vector& pos);
	void EnterDistrict(int x, int z);
	Ptr<DistrictNodeInstance> LoadDistrict(int x, int z);

protected:
	virtual void OnAttachToModel(const Ptr<Models::Model>& model);

	Ptr<Terrain>				terrain;
	Ptr<CoreGraphics::MeshPool> distMeshPool;

	Ptr<DistrictNodeInstance>	centerDist;			// ����λ�õ�District
	int							curX;				// ��ǰλ��
	int							curZ;

	Math::vector				savePos;
};

inline const Ptr<CoreGraphics::MeshPool>& 
TerrainInstance::GetMeshPool()const
{
	return this->distMeshPool;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
