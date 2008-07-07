//------------------------------------------------------------------------------
//  transformnodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/modelinstance.h"
#include "Terrain/terrainchunknodeinstance.h"
#include "Terrain/terrainchunknode.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/transformdevice.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainChunkNodeInstance, 'CKNI', Models::ModelNodeInstance);

using namespace Math;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
TerrainChunkNodeInstance::TerrainChunkNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TerrainChunkNodeInstance::~TerrainChunkNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainChunkNodeInstance::Render()
{
	ModelNodeInstance::Render();
	RenderDevice::Instance()->Draw();
}    

//------------------------------------------------------------------------------
/**
*/
void
TerrainChunkNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	// just tell our model node that we are a visible instance
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainChunkNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainChunkNodeInstance::OnRemoveFromModelInstance()
{
	TransformNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainChunkNodeInstance::ApplyState()
{
	TransformNodeInstance::ApplyState();

	// apply any needed model transform state to shader
	TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();    
}
} // namespace Models
