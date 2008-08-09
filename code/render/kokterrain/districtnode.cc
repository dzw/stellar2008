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
	�������򶥵�

	ע�⣺
	1���������ļ����ǰ�����˳�����У����������ǰ�����ʽ��֯��������ȡ�߶ȵ�
	ʱ����Ҫ���ϵ�ͼ�Ŀ��(mapWide)������ȷȡ�����ݡ�
	2�������ǰ�ʵ��λ�ü��㣨���Ѿ����������ƫ�ƣ�
*/
void 
DistrictNode::Update()
{
	// ���λ�þ���MapMeshGridNode��meshNodes��λ��
	Ptr<Terrain> terr = this->model.downcast<Terrain>();
	//SizeT curPos = this->disX * 256 + this->disZ;//terr->DistrictPos2TilePos(this->disX, this->disZ);
	float tilePosOffset = 0;//terr->GetTilePosOffset();
	int size = terr->GetTileSize();
	int mapWide = terr->GetMapWide();
	SizeT cur = 0;

	float high, minHigh, maxHigh;
	minHigh = 10000.0f;
	maxHigh = -10000.0f;
	int disOffsetZ = this->disZ * DISTRICT_VERTS;	// ������ӵĿ�ʼλ��(����ʼ�����ڵ�ͼ�ϵ�ƫ��λ��)
	int disOffsetX = this->disX * DISTRICT_VERTS;	
	int x, z;
	// ����һ��������Ϣ
	MeshTest* vertices = (MeshTest*)this->meshPool->Lock(this->curGroup);
	for (SizeT j = 0; j < DISTRICT_VERTS; j++)
	{
		for (SizeT i = 0; i < DISTRICT_VERTS; i++)
		{
			x = disOffsetX + i;
			z = disOffsetZ + j;
			high = this->terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide); // �߶�
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