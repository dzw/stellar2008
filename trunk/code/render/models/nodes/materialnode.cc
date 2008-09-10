//------------------------------------------------------------------------------
//  MaterialNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/nodes/materialnode.h"
#include "models/nodes/materialnodeinstance.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"
#include "resources/resourcemanager.h"
#include "models/attributes.h"

namespace Models
{
ImplementClass(Models::MaterialNode, 'MTNE', Models::TransformNode);

using namespace Attr;
using namespace Util;
using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
MaterialNode::MaterialNode()
{
    // empty    
}

//------------------------------------------------------------------------------
/**
*/
MaterialNode::~MaterialNode()
{
    n_assert(!this->shaderInstance.isvalid());
    n_assert(this->materialList.IsEmpty());
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
Resource::State
MaterialNode::GetResourceState() const
{
    Resource::State state = TransformNode::GetResourceState();
    IndexT i;
    for (i = 0; i < this->materialList.Size(); i++)
    {
        const Ptr<ManagedTexture>& managedTexture = this->materialList[i].managedTexture;
        if (managedTexture->GetState() > state)
        {
            state = managedTexture->GetState();
        }
    }
    return state;
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

	this->materialList.Clear();

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

	//const Array<CoreGraphics::ShaderVariableInstance>& variables = this->materialList[this->curMaterial];
	//for (SizeT i = 0; i < variables.Size(); i++)
	//{
	//	variables[i]->Apply();
	//	/*switch(variables[i]->GetType())
	//	{
	//	case ShaderVariable::IntType:
	//		
	//		break;
	//	case ShaderVariable::FloatType:
	//		break;
	//	case ShaderVariable::VectorType:
	//		break;
	//	case ShaderVariable::MatrixType:
	//		break;
	//	case ShaderVariable::BoolType:
	//		variables[i]->SetBool();
	//		break;
	//	case ShaderVariable::TextureType:
	//		variables[i]->SetTexture(tex->GetTexture());
	//		break;
	//	}*/
	//}
	return true;
}

//------------------------------------------------------------------------------
/**
	Ôö¼ÓäÖÈ¾×´Ì¬
*/
//Ptr<ShaderVariableInstance>
//MaterialNode::CreateShaderVariableInstance(const ShaderVariable::Semantic& sem)
//{
//	n_assert(!this->shaderVariableInstances.Contains(sem));
//
//	// get the shader from my StateNode
//	n_assert(this->modelNode->IsA(StateNode::RTTI));
//	const Ptr<ShaderInstance>& shaderInstance = this->modelNode.downcast<StateNode>()->GetShaderInstance();
//	n_assert(shaderInstance.isvalid());
//
//	// create new shader variable instance
//	n_assert(shaderInstance->HasVariableBySemantic(sem));
//	const Ptr<ShaderVariable>& var = shaderInstance->GetVariableBySemantic(sem);
//	Ptr<ShaderVariableInstance> varInst = var->CreateInstance();
//	this->shaderVariableInstances.Add(sem, varInst);
//	return varInst;
//}

} // namespace Models
