#pragma once
#ifndef KOK_PARTICLENODEINSTANCE_H
#define KOK_PARTICLENODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::ParticleNodeInstance

    (C) 2008 cTuo
*/
#include "kok/model/material/materialnodeinstance.h"
#include "kok/effect/c3dsMaxParticle.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ParticleNodeInstance : public MaterialNodeInstance
{
    DeclareClass(ParticleNodeInstance);
public:
    /// constructor
    ParticleNodeInstance();
    /// destructor
    virtual ~ParticleNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	virtual void ApplyState();
	virtual void OnNotifyVisible(IndexT frameIndex);

protected:
	
	Ptr<CoreGraphics::ShaderVariable> diffMap;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

