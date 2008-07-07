//------------------------------------------------------------------------------
//  transformnodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/modelinstance.h"
#include "wow/world/Terrain/chunknodeinstance.h"
#include "wow/world/Terrain/chunknode.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/transformdevice.h"

namespace Models
{
ImplementClass(Models::ChunkNodeInstance, 'CKNI', Models::ModelNodeInstance);

using namespace Math;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ChunkNodeInstance::ChunkNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ChunkNodeInstance::~ChunkNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkNodeInstance::Render()
{
	ModelNodeInstance::Render();
	RenderDevice::Instance()->Draw();
}    

//------------------------------------------------------------------------------
/**
*/
void
ChunkNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	// just tell our model node that we are a visible instance
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkNodeInstance::OnRemoveFromModelInstance()
{
	TransformNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkNodeInstance::ApplyState()
{
	TransformNodeInstance::ApplyState();

	// apply any needed model transform state to shader
	TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();    
}
} // namespace Models
