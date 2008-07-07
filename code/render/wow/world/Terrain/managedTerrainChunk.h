#pragma once
#ifndef WOW_MANAGEDTERRAIN_H
#define WOW_MANAGEDTERRAIN_H
//------------------------------------------------------------------------------
/**
    
*/
#include "resources/managedresource.h"
#include "wow/world/Terrain/Terrainchunk.h"

//------------------------------------------------------------------------------
namespace Models
{
class ManagedTerrainChunk : public Resources::ManagedResource
{
    DeclareClass(ManagedTerrainChunk);
public:
    /// get contained model resource
	const Ptr<TerrainChunk>& GetModel() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<TerrainChunk>&
ManagedTerrainChunk::GetModel() const
{
    return this->GetResource().downcast<TerrainChunk>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
