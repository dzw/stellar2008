//------------------------------------------------------------------------------
//  simplestagebuilder.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/simplestagebuilder.h"
#include "graphics/stage.h"
#include "graphics/cell.h"
//#include "graphics/sorting/convexzonemanager.h"
#include "graphics/sorting/convexzone.h"

namespace Graphics
{
ImplementClass(Graphics::SimpleStageBuilder, 'SSBL', Graphics::StageBuilder);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
SimpleStageBuilder::SimpleStageBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SimpleStageBuilder::~SimpleStageBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleStageBuilder::BuildStage(const Ptr<Stage>& stage)
{
    // the stage just consists of one big root cell
    Ptr<Cell> rootCell = Cell::Create();
    rootCell->SetBoundingBox(bbox(point(0.0f, 0.0f, 0.0f), vector(10000000.0f, 10000000.0f, 10000000.0f)));
    stage->SetRootCell(rootCell);
	
	/*Ptr<ConvexZone> rootCell = ConvexZone::Create();
	rootCell->SetBoundingBox(bbox(point(0.0f, 0.0f, 0.0f), vector(1000.0f, 1000.0f, 1000.0f)));
	stage->SetRootCell(rootCell.upcast<Cell>());*/
}

} // namespace Graphics
