//------------------------------------------------------------------------------
//  ParticleNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/particle/particlenodeinstance.h"
#include "coregraphics/transformdevice.h"
#include "models/modelnode.h"

namespace KOK
{
ImplementClass(KOK::ParticleNodeInstance, 'PLIN', KOK::MaterialNodeInstance);

using namespace CoreGraphics;
using namespace Models;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
ParticleNodeInstance::ParticleNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ParticleNodeInstance::~ParticleNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeInstance::Render()
{
	
}

void
ParticleNodeInstance::ApplyState()
{
	MaterialNodeInstance::ApplyState();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
ParticleNodeInstance::Update()
{
	MaterialNodeInstance::Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(node->IsA(ParticleNode::RTTI));
	MaterialNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	CreateMaterial();
}

void
ParticleNodeInstance::CreateMaterial()
{
	this->diffMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));
}

}
