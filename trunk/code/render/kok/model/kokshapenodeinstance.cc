//------------------------------------------------------------------------------
//  KokShapeNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/kokshapenodeinstance.h"
#include "kok/model/kokshapenode.h"
#include "coregraphics/transformdevice.h"

namespace KOK
{
ImplementClass(KOK::KokShapeNodeInstance, 'KSNI', Models::TransformNodeInstance);

using namespace Models;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
KokShapeNodeInstance::KokShapeNodeInstance():
	shaderInstance(0),
	diffuseColor(0),
	ambientColor(0),
	specularColor(0),
	emissiveColor(0),
	diffMap(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
KokShapeNodeInstance::~KokShapeNodeInstance()
{
	this->shaderInstance = 0;
	this->diffuseColor   = 0;
	this->ambientColor   = 0;
	this->specularColor  = 0;
	this->emissiveColor  = 0;
	this->diffMap	     = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNodeInstance::Render()
{
	//this->modelNode.downcast<KokShapeNode>()->Render();
}    

void
KokShapeNodeInstance::ApplyState()
{
	TransformNodeInstance::ApplyState();

	TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
KokShapeNodeInstance::Update()
{
	TransformNodeInstance::Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
KokShapeNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(node->IsA(KokShapeNode::RTTI));
	TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	CreateMaterial();
}

void
KokShapeNodeInstance::CreateMaterial()
{
	this->shaderInstance = this->modelNode.downcast<KokShapeNode>()->GetShaderInstance();

	this->diffuseColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffuseColor"));
	this->ambientColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("AmbientColor"));
	this->specularColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("SpecularColor"));
	this->emissiveColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("EmissiveColor"));
	this->diffMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));
}

}
