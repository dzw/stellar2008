//------------------------------------------------------------------------------
//  MaterialNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/nodes/MaterialNodeInstance.h"
#include "models/nodes/MaterialNode.h"
#include "coregraphics/shader.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/transformdevice.h"

namespace Models
{
ImplementClass(Models::MaterialNodeInstance, 'MNIE', Models::TransformNodeInstance);

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
MaterialNodeInstance::MaterialNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MaterialNodeInstance::~MaterialNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
    TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNodeInstance::OnRemoveFromModelInstance()
{
    this->shaderVariableInstances.Clear();
    TransformNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNodeInstance::ApplyState()
{
    TransformNodeInstance::ApplyState();
 
    // apply the state of all shader variables instances
    IndexT i;
    for (i = 0; i < this->shaderVariableInstances.Size(); i++)
    {
        this->shaderVariableInstances.ValueAtIndex(i)->Apply();
    }

    // apply any needed model transform state to shader
    TransformDevice* transformDevice = TransformDevice::Instance();
    transformDevice->ApplyModelTransforms();    
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ShaderVariableInstance>
MaterialNodeInstance::CreateShaderVariableInstance(const ShaderVariable::Semantic& sem)
{
    n_assert(!this->shaderVariableInstances.Contains(sem));

    // get the shader from my StateNode
    n_assert(this->modelNode->IsA(MaterialNode::RTTI));
    const Ptr<ShaderInstance>& shaderInstance = this->modelNode.downcast<MaterialNode>()->GetShaderInstance();
    n_assert(shaderInstance.isvalid());
    
    // create new shader variable instance
    n_assert(shaderInstance->HasVariableBySemantic(sem));
    const Ptr<ShaderVariable>& var = shaderInstance->GetVariableBySemantic(sem);
    Ptr<ShaderVariableInstance> varInst = var->CreateInstance();
    this->shaderVariableInstances.Add(sem, varInst);
    return varInst;
}

//------------------------------------------------------------------------------
/**
*/
bool
MaterialNodeInstance::HasShaderVariableInstance(const ShaderVariable::Semantic& sem) const
{
    return this->shaderVariableInstances.Contains(sem);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ShaderVariableInstance>&
MaterialNodeInstance::GetShaderVariableInstance(const ShaderVariable::Semantic& sem) const
{
    n_assert(this->shaderVariableInstances.Contains(sem));
    return this->shaderVariableInstances[sem];
}

} // namespace Models
