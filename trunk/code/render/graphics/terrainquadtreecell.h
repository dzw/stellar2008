#pragma once
#ifndef GRAPHICS_TERRAINQUADTREECELL_H
#define GRAPHICS_TERRAINQUADTREECELL_H
//------------------------------------------------------------------------------
/**
    
*/
#include "graphics/cell.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class TerrainQuadTreeCell : public Cell
{
    DeclareClass(TerrainQuadTreeCell);
public:
    /// constructor
    TerrainQuadTreeCell();
    /// destructor
    virtual ~TerrainQuadTreeCell();
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
