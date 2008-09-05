//------------------------------------------------------------------------------
//  quadtreestagebuilder.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/staticquadtreestagebuilder.h"
#include "graphics/stage.h"
#include "graphics/cell.h"

namespace Graphics
{
ImplementClass(Graphics::StaticQuadtreeStageBuilder, 'QSBL', Graphics::StageBuilder);

using namespace Math;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
StaticQuadtreeStageBuilder::StaticQuadtreeStageBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StaticQuadtreeStageBuilder::~StaticQuadtreeStageBuilder()
{
    // empty
}

void 
StaticQuadtreeStageBuilder::InitQuadtree(const Math::bbox& box, int level)
{
	this->quadTree.SetDepth(level);
	this->quadTree.SetBoundingBox(box);
	this->quadTree.Initialize();
	this->numCellsBuilt = 0;
}

void
StaticQuadtreeStageBuilder::SetLeafNodeBoundingBox(int row, int col, const bbox& b)
{
	this->quadTree.SetLeafNodeBoundingBox(row, col, b);
}

void
StaticQuadtreeStageBuilder::UpdateBoundingBox()
{
	this->quadTree.UpdateBoundingBox();	
}

//------------------------------------------------------------------------------
/**
	Çå³ýËÄ²æÊ÷
*/
void 
StaticQuadtreeStageBuilder::ClearBoundingBox()
{
	this->quadTree.ClearAllBoundingBox();
}

//------------------------------------------------------------------------------
/**
*/
void
StaticQuadtreeStageBuilder::BuildStage(const Ptr<Stage>& stage)
{
    Ptr<Graphics::Cell> rootCell = this->CreateQuadTreeCell(0, 0, 0, 0);
    stage->SetRootCell(rootCell);
}

void
StaticQuadtreeStageBuilder::AttachToCell(const Ptr<Cell>& cell)
{
	this->rootCell = this->CreateQuadTreeCell(0, 0, 0, 0);
	this->rootCell->OnAttachToStage(cell->GetStage());
	cell->AttachChildCell(this->rootCell);
}

//------------------------------------------------------------------------------
/**
*/
Graphics::Cell* 
StaticQuadtreeStageBuilder::CreateQuadTreeCell(Graphics::Cell* parentCell, uchar curLevel, ushort curCol, ushort curRow)
{
    // create a new cell
	Graphics::Cell* cell = Graphics::Cell::Create();
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
