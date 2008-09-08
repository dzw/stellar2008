#pragma once
#ifndef KOK_DISTRICT_H
#define KOK_DISTRICT_H
//------------------------------------------------------------------------------
/**
    @class KOK::DistrictNode

    (C) 2008 cTuo
*/    
#include "models/modelnode.h"
#include "models/modelnodeinstance.h"
#include "kok/terrain/terraindef.h"
#include "kok/terrain/terrainmeshgrid.h"
#include "coregraphics/primitivegroup.h"
#include "kok/model/thingentity.h"

//------------------------------------------------------------------------------
namespace KOK
{
class DistrictNode : public Models::ModelNode
{
    DeclareClass(DistrictNode);
public:
    /// constructor
    DistrictNode();
    /// destructor
    virtual ~DistrictNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();

	/// called when resources should be loaded
    virtual void LoadResources();

	void SetPosition(int x, int z);

	void CreateMeshData();
	void UpdateDrawTable(TerrainMeshData** pMeshDatas);
	void CalcSquareDistance();
	float GetSquareDistance()const;
	TileMesh* GetVertexData()const;

	void NotifyVisible(IndexT frameIndex);
	DWORD GetFrameIndex()const;
	void Reset();

	/// 判断传入的pass是否需要渲染
	bool IsRender(int pass);
	void SetVertexStart(DWORD vb);
	DWORD GetVertexStart()const;
	bool SetRenderGroup(int pass, int texId);
	void Render();

	void AddThing(const Ptr<ThingEntity>& entity);
	const Util::Array<Ptr<ThingEntity>>& GetThings()const;

protected:
	//friend class DistrictNodeInstance;

    /// create a model node instance
	//virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;
    
    /// called when resources should be unloaded
    virtual void UnloadResources();

	/// district坐标
	int disX;
	int disZ;

	int tileStartX;
	int tileStartY;

	/// render
	TileMesh*		vertices;
	int				vertexSize;
	WORD*			indices;
	int				indexSize;
	TileDrawTable*	drawTable;

	int				shadowIndexSize;

	/// 包围盒到摄像机的距离
	float squareDistance;
	DWORD frameIndex;

	/// 顶点缓冲块位置
	SizeT vertexStart;
	SizeT indexStart;
	CoreGraphics::PrimitiveGroup group;
	/// 每帧更新一次索引缓冲
	bool updateIndexBuffer;

	/// 区域中所包含的地物
	Util::Array<Ptr<ThingEntity>> things;
};

inline DWORD
DistrictNode::GetFrameIndex()const
{
	return this->frameIndex;
}

inline DWORD
DistrictNode::GetVertexStart()const
{
	return this->vertexStart;
}

inline void
DistrictNode::SetVertexStart(DWORD vb)
{
	this->vertexStart = vb;
}

inline TileMesh*
DistrictNode::GetVertexData()const
{
	n_assert(this->vertices != NULL);
	return this->vertices;
}

inline float 
DistrictNode::GetSquareDistance()const
{
	return this->squareDistance;
}

inline bool 
DistrictNode::IsRender(int pass)
{
	return this->drawTable[pass].Texture && this->drawTable[pass].FaceCount > 0;
}

inline void 
DistrictNode::SetPosition(int x, int z)
{
	this->disX = x;
	this->disZ = z;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
