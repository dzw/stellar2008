#pragma once
#ifndef TERRAIN_TERRAINENTITY_H
#define TERRAIN_TERRAINENTITY_H
//------------------------------------------------------------------------------
/**
    cTuo
*/
#include "graphics/modelentity.h"
#include "terrain/terraindef.h"
#include "terrain/managedterraintile.h"

//------------------------------------------------------------------------------
namespace Terrain
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
    //virtual void OnUpdate();

    int GetX()const;
	int GetZ()const;
	void SetXZ(int x, int z);
	Math::float2 GetPos()const;
protected:
    //void ValidateModelInstance();

    //Ptr<ManagedTerrainTile> managedTerrainTile;
    
    int x;
    int z;
};

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainEntity::GetX()const
{
	return this->x;
}

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainEntity::GetZ()const
{
	return this->z;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TerrainEntity::SetXZ(int x, int z)
{
	this->x = x;
	this->z = z;
}

inline Math::float2 
TerrainEntity::GetPos()const
{
	return Math::float2(this->x * TILESIZE, this->z * TILESIZE);
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    