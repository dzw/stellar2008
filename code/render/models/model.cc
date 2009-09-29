//------------------------------------------------------------------------------
//  model.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/model.h"
#include "models/modelinstance.h"
#include "models/modelserver.h"
#include "models/visresolver.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shaderinstance.h"
#include "lighting/lightserver.h"
#include "graphics/modelentity.h"

namespace Models
{
ImplementClass(Models::Model, 'MODL', Resources::Resource);

using namespace Util;
using namespace Attr;
using namespace Resources;
using namespace CoreGraphics;
using namespace Frame;
using namespace Lighting;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
Model::Model() :
    inLoadResources(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Model::~Model()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
Model::LoadFromAttrs(const AttributeContainer& a)
{
    this->attrs = a;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Model::SaveToAttrs(AttributeContainer& a)
{
    a = this->attrs;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Model::Unload()
{    
    // discard model nodes
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        this->nodes[i]->OnRemoveFromModel();
    }
    this->nodes.Clear();
    this->nodeMap.Clear();
    this->visibleModelNodes.Reset();

    // discard instances
    for (i = 0; i < this->instances.Size(); i++)
    {
        this->instances[i]->OnRemoveFromModel();
    }
    this->instances.Clear();
        
    // call parent class
    Resource::Unload();
}

//------------------------------------------------------------------------------
/**
    This method asks all model nodes to load their resources. Note that 
    actual resource loading may be asynchronous and placeholder resources
    may be in place right after this method returns.
*/
void
Model::LoadResources()
{
    n_assert(!this->inLoadResources);
    this->inLoadResources = true;
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        this->nodes[i]->LoadResources();
    }
}

//------------------------------------------------------------------------------
/**
    This method asks all model nodes to unload their resources.
*/
void
Model::UnloadResources()
{
    n_assert(this->inLoadResources);
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        this->nodes[i]->UnloadResources();
    }
    this->inLoadResources = false;
}

//------------------------------------------------------------------------------
/**
    Checks all model nodes and returns a cumulative resource loading state.
*/
Resource::State
Model::GetResourceState() const
{
    n_assert(this->inLoadResources);
    Resource::State result = Resource::Initial;
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        Resource::State state = this->nodes[i]->GetResourceState();
        if (state > result)
        {
            result = state;
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
Model::AttachNode(const Ptr<ModelNode>& node)
{
    n_assert(node->GetName().Value().IsValid());
    n_assert(!this->HasNode(node->GetName()));
    this->nodes.Append(node);
    this->nodeMap.Add(node->GetName(), this->nodes.Size() - 1);
    node->OnAttachToModel(this);
}

//------------------------------------------------------------------------------
/**
*/
void
Model::RemoveNode(const Ptr<ModelNode>& node)
{
    IndexT nodeIndex = this->nodes.FindIndex(node);
    n_assert(InvalidIndex != nodeIndex);
    node->OnRemoveFromModel();
    this->nodeMap.Erase(node->GetName());
    this->nodes.EraseIndex(nodeIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
Model::AddVisibleModelNode(IndexT frameIndex, ModelNodeType::Code t, const Ptr<ModelNode>& modelNode)
{
    this->visibleModelNodes.Add(frameIndex, t, modelNode);
    if (!this->visibleModelNodes.IsResolved(t))
    {
        this->visibleModelNodes.SetResolved(t, true);
        VisResolver::Instance()->AddVisibleModel(frameIndex, t, this);
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelInstance>
Model::CreateInstance()
{
    Ptr<ModelInstance> modelInstance = ModelInstance::Create();
    this->AttachInstance(modelInstance);
    return modelInstance;
}

//------------------------------------------------------------------------------
/**
*/
void
Model::AttachInstance(const Ptr<ModelInstance>& modelInstance)
{
    n_assert(!modelInstance->IsAttachedToModel());
    this->instances.Append(modelInstance);
    modelInstance->OnAttachToModel(this);
}

//------------------------------------------------------------------------------
/**
*/
void
Model::RemoveInstance(const Ptr<ModelInstance>& modelInstance)
{
    n_assert(modelInstance->IsAttachedToModel());
    n_assert(modelInstance->GetModel().get() == this);
    modelInstance->OnRemoveFromModel();
    IndexT index = this->instances.FindIndex(modelInstance);
    n_assert(InvalidIndex != index);
    this->instances.EraseIndex(index);
}

void
Model::Render(const ModelNodeType::Code& nodeFilter, const Frame::LightingMode::Code& lightingMode, CoreGraphics::ShaderFeature::Mask& shaderFeatures)
{
	ShaderServer* shaderServer = ShaderServer::Instance();
	LightServer* lightServer = LightServer::Instance();

	// for each visible model node of the model...
	const Array<Ptr<ModelNode> >& modelNodes = this->GetVisibleModelNodes(nodeFilter);//visResolver->GetVisibleModelNodes(this->nodeFilter, models[modelIndex]);
	IndexT modelNodeIndex;  
	for (modelNodeIndex = 0; modelNodeIndex < modelNodes.Size(); modelNodeIndex++)
	{
		// apply render state which is shared by all instances
		shaderServer->ResetFeatureBits();
		shaderServer->SetFeatureBits(shaderFeatures);
		const Ptr<ModelNode>& modelNode = modelNodes[modelNodeIndex];            
		// 资源没准备好，不渲染
		if (!modelNode->ApplySharedState())
			continue;

		// if lighting mode is Off, we can render all node instances with the same shader
		const Ptr<ShaderInstance>& shaderInst = shaderServer->GetActiveShaderInstance();
		if (!shaderInst.isvalid())
			continue;

		if (LightingMode::None == lightingMode)
		{
			shaderInst->SelectActiveVariation(shaderServer->GetFeatureBits());
			SizeT numPasses = shaderInst->Begin();
			n_assert(1 == numPasses);
			shaderInst->BeginPass(0);
		}

		// render instances
		const Array<Ptr<ModelNodeInstance> >& nodeInstances = modelNode->GetVisibleModelNodeInstances(nodeFilter);//visResolver->GetVisibleModelNodeInstances(this->nodeFilter, modelNode);
		IndexT nodeInstIndex;
		for (nodeInstIndex = 0; nodeInstIndex < nodeInstances.Size(); nodeInstIndex++)
		{
			const Ptr<ModelNodeInstance>& nodeInstance = nodeInstances[nodeInstIndex];

			// if single-pass lighting is enabled, we need to setup the lighting 
			// shader states
			// FIXME: This may set a new shader variation for every node instance
			// which is expensive! Would be better to sort node instances by number
			// of active lights!!!
			if (LightingMode::SinglePass == lightingMode)
			{
				// setup lighting render states
				// NOTE: this may change the shader feature bit mask which may select
				// a different shader variation per entity
				const Ptr<ModelEntity>& modelEntity = nodeInstance->GetModelInstance()->GetModelEntity();
				lightServer->ApplyModelEntityLights(modelEntity);
				shaderInst->SelectActiveVariation(shaderServer->GetFeatureBits());
				SizeT numPasses = shaderInst->Begin();
				n_assert(1 == numPasses);
				shaderInst->BeginPass(0);
			}

			// render the node instance
			nodeInstance->ApplyState();
			shaderInst->Commit();
			nodeInstance->Render();

			if (LightingMode::SinglePass == lightingMode)
			{
				shaderInst->EndPass();
				shaderInst->End();
			}
		}

		if (LightingMode::None == lightingMode)
		{
			shaderInst->EndPass();
			shaderInst->End();
		}                
	}
}

} // namespace Models
