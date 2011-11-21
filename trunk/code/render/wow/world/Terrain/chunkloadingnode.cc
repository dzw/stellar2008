//------------------------------------------------------------------------------
//  transformnode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/Terrain/chunkloadingnode.h"
#include "wow/world/Terrain/chunkloadingnodeinstance.h"
#include "models/attributes.h"

namespace Models
{
ImplementClass(Models::ChunkLoadingNode, 'CKLN', Models::ModelNode);

using namespace Attr;
using namespace Math;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
ChunkLoadingNode::ChunkLoadingNode() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ChunkLoadingNode::~ChunkLoadingNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
ChunkLoadingNode::CreateNodeInstance() const
{
    Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) ChunkLoadingNodeInstance::Create();
    return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkLoadingNode::LoadResources()
{
	n_assert(!this->managedMaptile.isvalid());
	n_assert(this->HasAttr(Attr::MeshResourceId));

	// create a managed mesh resource
	ResourceId meshResId = this->GetString(Attr::MeshResourceId);
	
	this->managedMaptile = ResourceManager::Instance()->CreateManagedResource(MapTile::RTTI, meshResId).downcast<ManagedMaptile>();
	n_assert(this->managedMaptile.isvalid());

	// call parent class
	ModelNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkLoadingNode::UnloadResources()
{
	n_assert(this->managedMaptile.isvalid());

	// discard managed resource
	ResourceManager::Instance()->DiscardManagedResource(this->managedMaptile.upcast<ManagedResource>());
	this->managedMaptile = 0;

	// call parent class
	ModelNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
ChunkLoadingNode::ApplySharedState()
{
    if (this->managedMaptile->GetState() == Resource::Loaded)
    {
        // 创建或更新chunk
    }
}

} // namespace Models
