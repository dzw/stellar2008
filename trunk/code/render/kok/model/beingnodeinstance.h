#pragma once
#ifndef KOK_BEINGNODEINSTANCE_H
#define KOK_BEINGNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::BeingNodeInstance

	���¹���ƫ�ƾ�����Ⱦ��������

    (C) 2008 cTuo
*/
#include "kok/model/kokshapenodeinstance.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/shadervariable.h"
#include "kok/model/beingentity.h"

//------------------------------------------------------------------------------
namespace KOK
{
class BeingNodeInstance : public KokShapeNodeInstance
{
    DeclareClass(BeingNodeInstance);
public:
    /// constructor
    BeingNodeInstance();
    /// destructor
    virtual ~BeingNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	virtual void ApplyState();
	virtual void OnNotifyVisible(IndexT frameIndex);

	void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
protected:
	Ptr<CoreGraphics::ShaderVariableInstance> CreateShaderVariableInstance(const CoreGraphics::ShaderVariable::Semantic& sem);
	void RenderBatch(IndexT index, bool reflectRender);
	void CreateMaterial();

	Ptr<CoreGraphics::ShaderVariable> fakeReflectMap;		// �ٷ�����ͼ
	Ptr<CoreGraphics::ShaderVariable> fakeReflectLightMap;	// �ٷ��������ͼ
	Ptr<CoreGraphics::ShaderVariable> jointPaletteVar;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

