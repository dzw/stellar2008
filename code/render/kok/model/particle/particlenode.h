#pragma once
#ifndef KOK_PARTICLENODE_H
#define KOK_PARTICLENODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::ParticleNode

	СЃзгЪ§Он

    (C) 2008 cTuo
*/    
#include "kok/model/material/materialnode.h"
#include "kok/effect/c3dsMaxParticle.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ParticleNode : public MaterialNode
{
    DeclareClass(ParticleNode);
public:
    /// constructor
    ParticleNode();
    /// destructor
    virtual ~ParticleNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();
	/// called when resources should be loaded
    virtual void LoadResources();

	void SetEmitData(EmitterData* data);
protected:
	friend class ParticleNodeInstance;

	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance()const;
	virtual void UnloadResources();

	EmitterData* emitData;
};



} // namespace Models
//------------------------------------------------------------------------------
#endif
