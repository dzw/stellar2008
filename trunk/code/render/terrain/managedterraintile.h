#pragma once
#ifndef TERRAIN_MANAGEDTERRAINTILE_H
#define TERRAIN_MANAGEDTERRAINTILE_H
//------------------------------------------------------------------------------
/**
    对地形资源进行管理，需要在这里做LOD处理。
    
    (C) 
*/
#include "resources/managedresource.h"
#include "terrain/terraintile.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class ManagedTerrainTile : public Resources::ManagedResource
{
    DeclareClass(ManagedTerrainTile);
public:
    /// get contained model resource
    const Ptr<TerrainTile>& GetTerrainTile() const;

	///
	int GetX()const;
	int GetZ()const;
	void SetXZ(int x, int z);
	
protected:
	int x, z;
	Math::float2 posBase;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<TerrainTile>&
ManagedTerrainTile::GetTerrainTile() const
{
    return this->GetResource().downcast<TerrainTile>();
}
//------------------------------------------------------------------------------
/**
*/
inline int 
ManagedTerrainTile::GetX()const
{
	return this->x;
}

//------------------------------------------------------------------------------
/**
*/
inline int 
ManagedTerrainTile::GetZ()const
{
	return this->z();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ManagedTerrainTile::SetXZ(int x, int z)
{
	this->x = x;
	this->z = z;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
