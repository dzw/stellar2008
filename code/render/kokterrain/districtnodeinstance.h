#pragma once
#ifndef KOK_DIRSTRICTNODEINSTANCE_H
#define KOK_DIRSTRICTNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::DistrictNodeInstance

    (C) 2008 cTuo
*/
#include "models/nodes/statenodeinstance.h"
#include "coregraphics/Dynamicmeshpool.h"
#include "kokterrain/districtnode.h"

//------------------------------------------------------------------------------
namespace KOK
{
class DistrictNodeInstance : public Models::StateNodeInstance
{
    DeclareClass(DistrictNodeInstance);
public:
    /// constructor
    DistrictNodeInstance();
    /// destructor
    virtual ~DistrictNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	/// apply per-instance state prior to rendering
	virtual void ApplyState();
	/// 设置每次渲染的group,返回flash：当没有面需要渲染时
	bool SetRenderGroup(int pass, int texId);
	int GetTextureId(int pass);
	/// 判断传入的pass是否需要渲染
	bool IsRender(int pass);

	/// 切换DISTRICT时清除已有数据
	void Clear();
	/// 设置渲染数据
	void Reset(const Ptr<DistrictNode>& district = 0);
	/// 渲染前分配缓冲
	void AllocMeshPool();
	void FreeMeshPool(IndexT frame);
	float GetSquareDistance()const;
	void CalcSquareDistance();

	int GetX()const;
	int	GetZ()const;

	/// notify that we are visible
    virtual void OnNotifyVisible(IndexT frameIndex);
protected:
	/// 顶点缓冲块位置
	DWORD vertexStart;
	IndexT frameIndex;
	Ptr<CoreGraphics::DynamicMeshPool> meshPool;
	CoreGraphics::PrimitiveGroup group;
	SizeT vbStart;
	SizeT ibStart;
	Ptr<DistrictNode> dist;
	/// 包围盒到摄像机的距离
	float squareDistance;
};

inline DWORD
DistrictNodeInstance::GetVertexStart()
{
	return this->vertexStart;
}

inline float 
DistrictNodeInstance::GetSquareDistance()const
{
	return this->squareDistance;
}

inline bool 
DistrictNodeInstance::IsRender(int pass)
{
	return dist->drawTable[pass].Texture && dist->drawTable[pass].FaceCount > 0;
}

inline int 
DistrictNodeInstance::GetTextureId(int pass)
{
	return dist->drawTable[pass].Texture-1;
}

inline int  
DistrictNodeInstance::GetX()const
{
	return this->modelNode.downcast<DistrictNode>()->GetX();
}

inline int	 
DistrictNodeInstance::GetZ()const
{
	return this->modelNode.downcast<DistrictNode>()->GetZ();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

