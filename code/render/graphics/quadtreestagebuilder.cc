//------------------------------------------------------------------------------
//  quadtreestagebuilder.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/quadtreestagebuilder.h"
#include "graphics/stage.h"
#include "graphics/cell.h"
#include "graphics/terraincell.h"

namespace Attr
{
    DefineInt(QuadTreeStageBuilderDepth, 'QTBD', Attr::ReadOnly);
    DefineFloat4(QuadTreeStageBuilderCenter, 'QTBC', Attr::ReadOnly);
    DefineFloat4(QuadTreeStageBuilderExtents, 'QTBE', Attr::ReadOnly);
};

namespace Graphics
{
ImplementClass(Graphics::QuadtreeStageBuilder, 'QSBL', Graphics::StageBuilder);

using namespace Math;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
QuadtreeStageBuilder::QuadtreeStageBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
QuadtreeStageBuilder::~QuadtreeStageBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
QuadtreeStageBuilder::BuildStage(const Ptr<Stage>& stage)
{
    this->quadTree.SetDepth(this->attrs.GetInt(Attr::QuadTreeStageBuilderDepth));
    Math::bbox box(this->attrs.GetFloat4(Attr::QuadTreeStageBuilderCenter),
                   this->attrs.GetFloat4(Attr::QuadTreeStageBuilderExtents));
    this->quadTree.SetBoundingBox(box);
    this->quadTree.Initialize();
    this->numCellsBuilt = 0; 

    Ptr<Graphics::Cell> rootCell = this->CreateQuadTreeCell(0, 0, 0, 0);
    stage->SetRootCell(rootCell);
}

//------------------------------------------------------------------------------
/**
*/
Graphics::Cell* 
QuadtreeStageBuilder::CreateQuadTreeCell(Graphics::Cell* parentCell, uchar curLevel, ushort curCol, ushort curRow)
{
    // create a new cell
	Graphics::Cell* cell = (Graphics::Cell*)Graphics::TerrainCell::Create();
    int nodeIndex = this->quadTree.GetNodeIndex(curLevel, curCol, curRow);
    const QuadTree<Cell>::Node& node = this->quadTree.GetNodeByIndex(nodeIndex);
    cell->SetBoundingBox(node.GetBoundingBox());
    this->numCellsBuilt++;

    // create child cells
    uchar childLevel = curLevel + 1;
    if (childLevel < this->quadTree.GetDepth())
    {        
        ushort i;
        for (i = 0; i < 4; i++)
        {
            ushort childCol = 2 * curCol + (i & 1);
            ushort childRow = 2 * curRow + ((i & 2) >> 1);
            Ptr<Graphics::Cell> childCell = this->CreateQuadTreeCell(cell, childLevel, childCol, childRow);
            cell->AttachChildCell(childCell);
        }        
    }
    return cell;
}

} // namespace Graphics