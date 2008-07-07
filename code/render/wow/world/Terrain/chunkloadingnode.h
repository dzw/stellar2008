#pragma once
#ifndef MODELS_CHUNKLOADINGNODE_H
#define MODELS_CHUNKLOADINGNODE_H
//------------------------------------------------------------------------------
/**
    @class Models::TransformNode
    
    Defines a transformation in a transform hierarchy.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/modelnode.h"
#include "wow/world/terrain/maptile.h"

//------------------------------------------------------------------------------
namespace Models
{
class ChunkLoadingNode : public ModelNode
{
    DeclareClass(ChunkLoadingNode);
public:
    /// constructor
    ChunkLoadingNode();
    /// destructor
    virtual ~ChunkLoadingNode();

    /// apply state shared by all my ModelNodeInstances
    virtual void ApplySharedState();

protected:
    /// create a model node instance
    virtual Ptr<ModelNodeInstance> CreateNodeInstance() const;
	/// called when resources should be loaded
	virtual void LoadResources();
	/// called when resources should be unloaded
	virtual void UnloadResources();

	/// ¼ÓÔØadtÎÄ¼þ
	Ptr<Resource::ManagedMapTile> managedMaptile;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    