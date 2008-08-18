#pragma once
#ifndef KOK_TERRAININSTANCE_H
#define KOK_TERRAININSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::TerrainInstance
    
    (C) 2008 cTuo
*/
#include "models/modelinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class TerrainInstance : public Models::ModelInstance
{
    DeclareClass(TerrainInstance);
public:
    /// constructor
    TerrainInstance();
    /// destructor
    virtual ~TerrainInstance();

    /// perform per-frame update (after setting transform, visibility, time, etc)
    virtual void Update();

protected:
	virtual void OnAttachToModel(const Ptr<Models::Model>& model);
	virtual void NotifyVisible(IndexT frameIndex);
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
