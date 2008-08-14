#pragma once
#ifndef KOK_TERRAINENTITY_H
#define KOK_TERRAINENTITY_H
//------------------------------------------------------------------------------
/**
    cTuo
*/
#include "graphics/modelentity.h"
#include "kokterrain/terraindef.h"
#include "kokterrain/managedterrain.h"

//------------------------------------------------------------------------------
namespace KOK
{
class TerrainEntity : public Graphics::ModelEntity
{
    DeclareClass(TerrainEntity);
public:
    /// constructor
    TerrainEntity();
    /// destructor
    virtual ~TerrainEntity();
	
    virtual void OnActivate();
    virtual void OnDeactivate();
    virtual void OnUpdate();

protected:
	void ValidateModelInstance();
};
} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    