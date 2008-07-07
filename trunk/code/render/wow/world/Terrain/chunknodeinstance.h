#pragma once
#ifndef MODELS_CHUNKNODEINSTANCE_H
#define MODELS_CHUNKNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class Models::TransformNodeInstance

    Holds and applies per-node-instance transformation.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/nodes/transformnodeinstance.h"
#include "math/vector.h"
#include "math/point.h"
#include "math/quaternion.h"
#include "math/transform44.h"

//------------------------------------------------------------------------------
namespace Models
{
class ChunkNodeInstance : public TransformNodeInstance
{
    DeclareClass(ChunkNodeInstance);
public:
    /// constructor
    ChunkNodeInstance();
    /// destructor
    virtual ~ChunkNodeInstance();

	///
	friend bool operator <= (const ChunkNodeInstance& a, const ChunkNodeInstance& b);
	/// perform rendering
	virtual void Render();
	/// apply per-instance state prior to rendering
	virtual void ApplyState();
protected:
	/// notify that we are visible
	virtual void OnNotifyVisible(IndexT frameIndex);
	/// called when attached to ModelInstance
	virtual void OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst);
	/// called when removed from ModelInstance
	virtual void OnRemoveFromModelInstance();
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
operator <= (const ChunkNodeInstance& a, const ChunkNodeInstance& b)
{
	return a.GetPosition().z() > b.GetPosition().z();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
