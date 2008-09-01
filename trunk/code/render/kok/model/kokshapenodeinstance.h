#pragma once
#ifndef KOK_KOKSHAPENODEINSTANCE_H
#define KOK_KOKSHAPENODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::KokShapeNodeInstance

    (C) 2008 cTuo
*/
#include "models/nodes/transformnodeinstance.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shadervariable.h"

//------------------------------------------------------------------------------
namespace KOK
{
class KokShapeNodeInstance : public Models::TransformNodeInstance
{
    DeclareClass(KokShapeNodeInstance);
public:
    /// constructor
    KokShapeNodeInstance();
    /// destructor
    virtual ~KokShapeNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	virtual void ApplyState();
	virtual void OnNotifyVisible(IndexT frameIndex);
	
protected:
	void CreateMaterial();
	void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
	
	Ptr<CoreGraphics::ShaderInstance> shaderInstance;
	Ptr<CoreGraphics::ShaderVariable> diffuseColor;
	Ptr<CoreGraphics::ShaderVariable> ambientColor;
	Ptr<CoreGraphics::ShaderVariable> specularColor;
	Ptr<CoreGraphics::ShaderVariable> emissiveColor;
	Ptr<CoreGraphics::ShaderVariable> diffMap;				// ∆’Õ®Œ∆¿Ì
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

