#pragma once
#ifndef KOK_BEINGNODEINSTANCE_H
#define KOK_BEINGNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::BeingNodeInstance

	¸üÐÂ¹Ç÷ÀÆ«ÒÆ¾ØÕó£¬äÖÈ¾¹Ç÷À¶¯»­

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

	Ptr<CoreGraphics::ShaderVariableInstance> jointPaletteVar;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

