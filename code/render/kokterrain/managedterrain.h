#pragma once
#ifndef TERRAIN_MANAGEDTERRAIN_H
#define TERRAIN_MANAGEDTERRAIN_H
//------------------------------------------------------------------------------
/**
    对地形资源进行管理，需要在这里做LOD处理。
    
    (C) 2008 cTuo
*/
#include "models/managedmodel.h"
#include "kokterrain/terrain.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ManagedTerrain : public Models::ManagedModel
{
    DeclareClass(ManagedTerrain);
public:
    /// get contained model resource
    const Ptr<Terrain>& GetTerrain() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Terrain>&
ManagedTerrain::GetTerrain() const
{
    return this->GetResource().downcast<Terrain>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
