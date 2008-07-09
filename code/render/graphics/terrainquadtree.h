#pragma once
#ifndef GRAPHICS_TERRAINQUADTREE_H
#define GRAPHICS_TERRAINQUADTREE_H
//------------------------------------------------------------------------------
/**
*/
#include "graphics/cell.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class TerrainQuadTree : public Cell
{
    DeclareClass(TerrainQuadTree);
public:
    /// constructor
    TerrainQuadTree();
    /// destructor
    virtual ~TerrainQuadTree();

private:

};

//------------------------------------------------------------------------------
/**
*/

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
