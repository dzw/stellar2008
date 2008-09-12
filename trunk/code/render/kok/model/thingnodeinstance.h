#pragma once
#ifndef KOK_THINGNODEINSTANCE_H
#define KOK_THINGNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::ThingNodeInstance

	

    (C) 2008 cTuo
*/
#include "kok/model/kokshapenodeinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ThingNodeInstance : public KokShapeNodeInstance
{
    DeclareClass(ThingNodeInstance);
public:
    /// constructor
    ThingNodeInstance();
    /// destructor
    virtual ~ThingNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	virtual void ApplyState();
	virtual void OnNotifyVisible(IndexT frameIndex);

	/// ´´½¨mesh
	void CreateMesh();
protected:
	void RenderBatch(IndexT index);
	virtual void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, 
		const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);

	Ptr<CoreGraphics::ShaderVariable> cullMode;
	Ptr<CoreGraphics::ShaderVariable> srcBlend;
	Ptr<CoreGraphics::ShaderVariable> destBlend;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

