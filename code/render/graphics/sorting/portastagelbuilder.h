#pragma once
#ifndef GRAPHICS_PORTALSTAGEBUILDER_H
#define GRAPHICS_PORTALSTAGEBUILDER_H
//------------------------------------------------------------------------------
/**
    @class Graphics::PortalStageBuilder
    
    (C) 2008 ctuo
*/
#include "graphics/stagebuilder.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Cell;

class PortalStageBuilder : public StageBuilder
{
    DeclareClass(PortalStageBuilder);
public:
    /// constructor
    PortalStageBuilder();
    /// destructor
    virtual ~PortalStageBuilder();
    /// setup the stage
    virtual void BuildStage(const Ptr<Stage>& stage);
    /// set quad tree depth and boudning box
    void SetQuadTreeSettings(uchar depth, const Math::bbox& worldBBox);

private:
    /// create a quad tree and its children, recursively
    Graphics::Cell* CreateQuadTreeCell(Graphics::Cell* parentCell, uchar curLevel, ushort curCol, ushort curRow);

    int numCellsBuilt;
    Util::QuadTree<Cell> quadTree;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
QuadtreeStageBuilder::SetQuadTreeSettings(uchar depth, const Math::bbox& worldBBox)
{
    this->quadTree.SetDepth(depth);
    this->quadTree.SetBoundingBox(worldBBox);
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    