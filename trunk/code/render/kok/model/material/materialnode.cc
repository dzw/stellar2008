//------------------------------------------------------------------------------
//  MaterialNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/material/materialnode.h"
#include "kok/model/material/materialnodeinstance.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"
#include "resources/resourcemanager.h"
#include "models/attributes.h"

namespace KOK
{
ImplementClass(KOK::MaterialNode, 'MTNE', Models::TransformNode);

using namespace Attr;
using namespace Util;
using namespace CoreGraphics;
using namespace Resources;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
MaterialNode::MaterialNode():
m_pMaterial(0),
m_pMaterialAnimator(0),
shaderInstance(0)
{
    // empty    
}

//------------------------------------------------------------------------------
/**
*/
MaterialNode::~MaterialNode()
{
    n_assert(!this->shaderInstance.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
MaterialNode::CreateNodeInstance() const
{
    Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) MaterialNodeInstance::Create();
    return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNode::LoadResources()
{
    n_assert(!this->shaderInstance.isvalid());
    n_assert(this->HasAttr(Attr::Shader));

    // create a new shader instance from the Shader attribute
    const ResourceId& resId = this->GetString(Attr::Shader);
    this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(resId);

    ModelNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNode::UnloadResources()
{
    n_assert(this->shaderInstance.isvalid());

    // discard shader instance
    this->shaderInstance->Discard();
    this->shaderInstance = 0;
    ModelNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
MaterialNode::ApplySharedState()
{
    // up to parent class
    TransformNode::ApplySharedState();

    // set our shader instance as current
    ShaderServer::Instance()->SetActiveShaderInstance(this->shaderInstance);
	return true;
}

} // namespace Models
