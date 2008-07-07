#pragma once
#ifndef WOW_MANAGEDMAPTILE_H
#define WOW_MANAGEDMAPTILE_H
//------------------------------------------------------------------------------
/**
    @class Models::ManagedModel
    
    A specialized managed resource for Models.
    
    (C) 2007 Radon Labs GmbH
*/
#include "resources/managedresource.h"
#include "wow/world/terrain/maptile.h"

//------------------------------------------------------------------------------
namespace Models
{
class ManagedMapTile : public Resources::ManagedResource
{
    DeclareClass(ManagedMapTile);
public:
    /// get contained model resource
    const Ptr<MapTile>& GetMapTile() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<MapTile>&
ManagedMapTile::GetMapTile() const
{
    return this->GetResource().downcast<MapTile>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
