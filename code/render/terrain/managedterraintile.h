#pragma once
#ifndef TERRAIN_MANAGEDTERRAINTILE_H
#define TERRAIN_MANAGEDTERRAINTILE_H
//------------------------------------------------------------------------------
/**
    对地形资源进行管理，需要在这里做LOD处理。
    
    (C) 
*/
#include "models/managedmodel.h"
#include "terrain/terraintile.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class ManagedTerrainTile : public Models::ManagedModel
{
    DeclareClass(ManagedTerrainTile);
public:
    /// get contained model resource
    const Ptr<TerrainTile>& GetTile() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<TerrainTile>&
ManagedTerrainTile::GetTile() const
{
    return this->GetResource().downcast<TerrainTile>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
