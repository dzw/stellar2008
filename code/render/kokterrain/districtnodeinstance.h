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
	/// ����ÿ����Ⱦ��group,����flash����û������Ҫ��Ⱦʱ
	bool SetRenderGroup(int pass, int texId);
	int GetTextureId(int pass);
	/// �жϴ����pass�Ƿ���Ҫ��Ⱦ
	bool IsRender(int pass);

	/// �л�DISTRICTʱ�����������
	void Clear();
	/// ������Ⱦ����
	void Reset(const Ptr<DistrictNode>& district = 0);
	/// ��Ⱦǰ���仺��
	void AllocMeshPool();
	void FreeMeshPool(IndexT frame);
	float GetSquareDistance()const;
	void CalcSquareDistance();

	int GetX()const;
	int	GetZ()const;

	/// notify that we are visible
    virtual void OnNotifyVisible(IndexT frameIndex);
protected:
	/// ���㻺���λ��
	DWORD vertexStart;
	IndexT frameIndex;
	Ptr<CoreGraphics::DynamicMeshPool> meshPool;
	CoreGraphics::PrimitiveGroup group;
	SizeT vbStart;
	SizeT ibStart;
	Ptr<DistrictNode> dist;
	/// ��Χ�е�������ľ���
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

