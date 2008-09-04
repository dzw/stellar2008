#pragma once
#ifndef KOK_EQUIPODEINSTANCE_H
#define KOK_EQUIPODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::EquipNodeInstance

	
    (C) 2008 cTuo
*/
#include "kok/model/kokshapenodeinstance.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/shadervariable.h"

//------------------------------------------------------------------------------
namespace KOK
{
class EquipNodeInstance : public KokShapeNodeInstance
{
    DeclareClass(EquipNodeInstance);
public:
    /// constructor
    EquipNodeInstance();
    /// destructor
    virtual ~EquipNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	virtual void ApplyState();
	virtual void OnNotifyVisible(IndexT frameIndex);

protected:
	virtual void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
	Ptr<CoreGraphics::ShaderVariableInstance> CreateShaderVariableInstance(const CoreGraphics::ShaderVariable::Semantic& sem);
	void RenderBatch(IndexT index, bool reflectRender);
	void CreateMaterial();
	
	Ptr<CoreGraphics::ShaderVariable> fakeReflectMap;		// �ٷ�����ͼ
	Ptr<CoreGraphics::ShaderVariable> fakeReflectLightMap;	// �ٷ��������ͼ
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

