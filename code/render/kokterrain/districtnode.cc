//------------------------------------------------------------------------------
//  districtnode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/districtnode.h"
#include "kokterrain/districtnodeinstance.h"
#include "kokterrain/terrain.h"
#include "ddslib/ddslib.h"

namespace KOK
{
ImplementClass(KOK::DistrictNode, 'DTND', Models::StateNode);

using namespace Util;
using namespace Attr;
using namespace Resources;
using namespace CoreGraphics;
using namespace Models;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
DistrictNode::DistrictNode():
	disX(-1),
	disZ(-1)
{
	//int disOffsetZ = this->disZ * DISTRICT_VERTS;	// 计算格子的开始位置(即开始格子在地图上的偏移位置)
	//int disOffsetX = this->disX * DISTRICT_VERTS;	
	//int size = GetTileSize();

	//bbox box;
	//vector min, max;
	//min.set(scalar(disOffsetX * size), -1000.0f, scalar(disOffsetZ * size));
	//max.set(scalar((disOffsetX+DISTRICT_VERTS) * size), 1000.0f, scalar((disOffsetZ+DISTRICT_VERTS) * size));
	//box.pmin = min;
	//box.pmax = max;
	//this->SetBoundingBox(box);

	this->boundingBox.pmin = vector(-100.0f, -100.0f, -100.0f);
	this->boundingBox.pmax = vector(100.0f, 100.0f, 100.0f);
}

//------------------------------------------------------------------------------
/**
*/
DistrictNode::~DistrictNode()
{
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
DistrictNode::CreateNodeInstance() const
{
    Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) DistrictNodeInstance::Create();
    return newInst;
}

//------------------------------------------------------------------------------
/**
*/
Resource::State
DistrictNode::GetResourceState() const
{
	return Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
void
DistrictNode::LoadResources()
{
    // call parent class
    StateNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
DistrictNode::UnloadResources()
{    
    StateNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
DistrictNode::ApplySharedState()
{
    // first call parent class
    return StateNode::ApplySharedState();
}

int  
DistrictNode::GetTileSize()const
{
	return this->model.downcast<Terrain>()->GetTileSize();
}

int  
DistrictNode::GetMapWide()const
{
	return this->model.downcast<Terrain>()->GetMapWide();
}

float 
DistrictNode::GetTilePosOffset()const
{
	return this->model.downcast<Terrain>()->GetTilePosOffset();
}



}