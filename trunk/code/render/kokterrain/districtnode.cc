//------------------------------------------------------------------------------
//  districtnode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/districtnode.h"
#include "kokterrain/districtnodeinstance.h"
#include "kokterrain/terrain.h"
#include "models/visresolver.h"
#include "math/vector.h"

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
	curGroup(-1),
	meshPool(0),
	disX(-1),
	disZ(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
DistrictNode::~DistrictNode()
{
	this->meshPool = 0;
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
	this->meshPool = 0;

    StateNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
DistrictNode::ApplySharedState()
{
	AllocMeshPool();

	this->frameIndex = VisResolver::Instance()->GetFrameIndex();
	this->meshPool->ApplyPrimitive(curGroup);

    // first call parent class
    return StateNode::ApplySharedState();
}

//------------------------------------------------------------------------------
/**
	设置区域顶点

	注意：
	1、顶点在文件中是按行列顺序排列，而程序中是按区域方式组织，所以在取高度的
	时候，需要加上地图的宽度(mapWide)才能正确取到数据。
	2、顶点是按实际位置计算（即已经加上区域的偏移）
*/
void 
DistrictNode::Update()
{
	// 这个位置就是MapMeshGridNode中meshNodes的位置
	Ptr<Terrain> terr = this->model.downcast<Terrain>();
	//SizeT curPos = this->disX * 256 + this->disZ;//terr->DistrictPos2TilePos(this->disX, this->disZ);
	float tilePosOffset = 0;//terr->GetTilePosOffset();
	int size = terr->GetTileSize();
	int mapWide = terr->GetMapWide();
	SizeT cur = 0;

	float high, minHigh, maxHigh;
	minHigh = 10000.0f;
	maxHigh = -10000.0f;
	int disOffsetZ = this->disZ * DISTRICT_VERTS;	// 计算格子的开始位置(即开始格子在地图上的偏移位置)
	int disOffsetX = this->disX * DISTRICT_VERTS;	
	int x, z;
	// 设置一块区域信息
	MeshTest* vertices = (MeshTest*)this->meshPool->Lock(this->curGroup);
	for (SizeT j = 0; j < DISTRICT_VERTS; j++)
	{
		for (SizeT i = 0; i < DISTRICT_VERTS; i++)
		{
			x = disOffsetX + i;
			z = disOffsetZ + j;
			high = this->terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide); // 高度
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			vertices[cur++].p.set(x * size - tilePosOffset, high, z * size - tilePosOffset );

			z = disOffsetZ + j + 1;
			high = this->terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide);
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			vertices[cur++].p.set(x * size - tilePosOffset, high, z * size - tilePosOffset );

			x = disOffsetX + i + 1;
			z = disOffsetZ + j;
			high = this->terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide);
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			vertices[cur++].p.set(x * size - tilePosOffset, high,z * size - tilePosOffset );

			z = disOffsetZ + j+1;
			high = this->terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide);
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			vertices[cur++].p.set(x * size - tilePosOffset, high, z * size - tilePosOffset );
		}
	}
	this->meshPool->Unlock();
	
	vector min, max;
	min.set(scalar(disOffsetX * size), minHigh, scalar(disOffsetZ * size));
	max.set(scalar((disOffsetX+DISTRICT_VERTS) * size), maxHigh, scalar((disOffsetZ+DISTRICT_VERTS) * size));
	this->boundingBox.pmin = min;
	this->boundingBox.pmax = max;
}

void
DistrictNode::AllocMeshPool()()()()()()
{
	if (this->curGroup == -1)
	{
		if (this->meshPool.isvalid() && this->meshPool->Full())
			this->model.downcast<Terrain>()->UpdateMeshPool();

		this->meshPool = this->model.downcast<Terrain>()->GetMeshPool();
		this->curGroup = this->meshPool->Alloc(NULL);
		n_assert(this->curGroup != -1);

		Update();
	}
}

void 
DistrictNode::FreeMeshPool(IndexT frame)
{
	if (!this->meshPool.isvalid())
		return;

	if (this->curGroup != -1)
	{
		if (frame != this->frameIndex)
		{
			this->meshPool->Free(this->curGroup);
			this->curGroup = -1;
		}
	}
}

}