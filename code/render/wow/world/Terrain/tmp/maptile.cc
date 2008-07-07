//------------------------------------------------------------------------------
//  model.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/Terrain/maptile.h"

namespace WOW
{
ImplementClass(WOW::MapTile, 'MPTE', Resources::Resource);

//------------------------------------------------------------------------------
/**
*/
MapTile::MapTile()
{
}

//------------------------------------------------------------------------------
/**
*/
MapTile::~MapTile()
{
    // empty
}

} // namespace Models
