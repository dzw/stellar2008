#pragma once
#ifndef WOW_MANAGEDTERRAIN_H
#define WOW_MANAGEDTERRAIN_H
//------------------------------------------------------------------------------
/**
    
*/
#include "resources/managedresource.h"
#include "wow/world/Terrain/Terrain.h"

//------------------------------------------------------------------------------
namespace Models
{
class ManagedTerrain : public Resources::ManagedResource
{
    DeclareClass(ManagedTerrain);
public:
    /// get contained model resource
	const Ptr<Terrain>& GetModel() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Terrain>&
ManagedTerrain::GetModel() const
{
    return this->GetResource().downcast<Terrain>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
