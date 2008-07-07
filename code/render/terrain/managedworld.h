#pragma once
#ifndef TERRAIN_MANAGEDWORLD_H
#define TERRAIN_MANAGEDWORLD_H
//------------------------------------------------------------------------------
/**
    @class Terrain::ManagedWorld
    
    (C) cTuo
*/
#include "resources/managedresource.h"
#include "terrain/world.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class ManagedWorld : public Resources::ManagedResource
{
    DeclareClass(ManagedWorld);
public:
    /// get contained model resource
    const Ptr<World>& GetWorld() const;
	///
	bool IsValidTile(int i, int j)const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<World>&
ManagedWorld::GetWorld() const
{
    return this->GetResource().downcast<World>();
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
ManagedWorld::IsValidTile(int i, int j)const
{
	if(this->GetResource()->IsLoaded())
		return this->GetWorld()->IsValidTile(i, j);
	else
		return false;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
