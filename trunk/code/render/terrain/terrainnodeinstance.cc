//------------------------------------------------------------------------------
//  transformnodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/modelinstance.h"
#include "Terrain/terrainnodeinstance.h"
#include "Terrain/terrainnode.h"
#include "coregraphics/renderdevice.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainNodeInstance, 'CKNI', Models::ModelNodeInstance);

using namespace Math;
using namespace CoreGraphics;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
TerrainNodeInstance::TerrainNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TerrainNodeInstance::~TerrainNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainNodeInstance::Render()
{
	//ModelNodeInstance::Render();
	//RenderDevice::Instance()->Draw();

    const Ptr<TerrainNode> node = GetModelNode().downcast<TerrainNode>();
    if (node->GetResourceState() == Resources::Resource::Loaded)
    {
        node->Render();
    }
}    

//------------------------------------------------------------------------------
/**
*/
void
TerrainNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	// just tell our model node that we are a visible instance
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	ModelNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainNodeInstance::OnRemoveFromModelInstance()
{
	ModelNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainNodeInstance::ApplyState()
{
	//TransformNodeInstance::ApplyState();

	//// apply any needed model transform state to shader
	//TransformDevice* transformDevice = TransformDevice::Instance();
	//transformDevice->ApplyModelTransforms();
}
} // namespace Models
