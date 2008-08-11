//------------------------------------------------------------------------------
//  shapenodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/districtnodeinstance.h"
#include "kokterrain/terraininstance.h"
#include "coregraphics/renderdevice.h"
#include "models/modelnode.h"
#include "models/visresolver.h"
#include "math/vector.h"
#include "math/bbox.h"

namespace KOK
{
ImplementClass(KOK::DistrictNodeInstance, 'DTNI', Models::StateNodeInstance);

using namespace CoreGraphics;
using namespace Models;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
DistrictNodeInstance::DistrictNodeInstance():
	curGroup(-1),
	meshPool(0),
	frameIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
DistrictNodeInstance::~DistrictNodeInstance()
{
    this->meshPool = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
DistrictNodeInstance::Render()
{
	if (this->curGroup == -1)
		return;

    StateNodeInstance::Render();
	RenderDevice::Instance()->Draw();
}    

void
DistrictNodeInstance::ApplyState()
{
	if (this->curGroup == -1)
		return;

	StateNodeInstance::ApplyState();
	this->meshPool->ApplyPrimitive(curGroup);
}

//------------------------------------------------------------------------------
/**
*/
void
DistrictNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	AllocMeshPool();

	this->frameIndex = frameIndex;
    // just tell our model node that we are a visible instance
    this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
DistrictNodeInstance::Update()
{
	StateNodeInstance::Update();
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
DistrictNodeInstance::Reset(const Ptr<DistrictNode>& district)
{
	if (district.isvalid())
		this->modelNode = district.upcast<ModelNode>();

	Ptr<DistrictNode> node = this->modelNode.downcast<DistrictNode>();
	Ptr<TerrainMeshGrid> terrMeshGrid = node->GetTerrainMeshGrid();
	//SizeT curPos = this->disX * 256 + this->disZ;//terr->DistrictPos2TilePos(this->disX, this->disZ);
	float tilePosOffset = 0;//node->GetTilePosOffset();
	int size = node->GetTileSize();
	int mapWide = node->GetMapWide();
	SizeT cur = 0;

	float high, minHigh, maxHigh;
	minHigh = 10000.0f;
	maxHigh = -10000.0f;
	int disOffsetZ = node->GetZ() * DISTRICT_VERTS;	// ������ӵĿ�ʼλ��(����ʼ�����ڵ�ͼ�ϵ�ƫ��λ��)
	int disOffsetX = node->GetX() * DISTRICT_VERTS;	
	int x, z;
	// ����һ��������Ϣ
	MeshTest* vertices = (MeshTest*)this->meshPool->Lock(this->curGroup);
	for (SizeT j = 0; j < DISTRICT_VERTS; j++)
	{
		for (SizeT i = 0; i < DISTRICT_VERTS; i++)
		{
			x = disOffsetX + i;
			z = disOffsetZ + j;
			high = terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide); // �߶�
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			vertices[cur].x = x * size - tilePosOffset;
			vertices[cur].y = high;
			vertices[cur].z = z*size - tilePosOffset;
			vertices[cur].tu = 0.125*i;
			vertices[cur].tv = 0.125*j;
			//vertices[cur].p.set(x * size - tilePosOffset, high, z * size - tilePosOffset );
			//vertices[cur].tex.set(0.125*i, 0.125*j);
			cur++;

			z = disOffsetZ + j + 1;
			high = terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide);
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			//vertices[cur].p.set(x * size - tilePosOffset, high, z * size - tilePosOffset );
			//vertices[cur].tex.set(0.125*i, 0.125*(j+1));
			vertices[cur].x = x * size - tilePosOffset;
			vertices[cur].y = high;
			vertices[cur].z = z*size - tilePosOffset;
			vertices[cur].tu = 0.125*i;
			vertices[cur].tv = 0.125*(j+1);
			cur++;

			x = disOffsetX + i + 1;
			z = disOffsetZ + j;
			high = terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide);
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			//vertices[cur].p.set(x * size - tilePosOffset, high,z * size - tilePosOffset );
			//vertices[cur].tex.set(0.125*(i+1), 0.125*j);
			vertices[cur].x = x * size - tilePosOffset;
			vertices[cur].y = high;
			vertices[cur].z = z*size - tilePosOffset;
			vertices[cur].tu = 0.125*(i+1);
			vertices[cur].tv = 0.125*j;
			cur++;

			z = disOffsetZ + j+1;
			high = terrMeshGrid->CalcTileHeightWithCliffWater(x, z*mapWide);
			if (minHigh > high) minHigh = high;
			if (maxHigh < high) maxHigh = high;
			//vertices[cur].p.set(x * size - tilePosOffset, high, z * size - tilePosOffset );
			//vertices[cur].tex.set(0.125*(i+1), 0.125*(j+1));
			vertices[cur].x = x * size - tilePosOffset;
			vertices[cur].y = high;
			vertices[cur].z = z*size - tilePosOffset;
			vertices[cur].tu = 0.125*(i+1);
			vertices[cur].tv = 0.125*(j+1);
			cur++;
		}
	}
	this->meshPool->Unlock();

	/*bbox box;
	vector min, max;
	min.set(scalar(disOffsetX * size), minHigh, scalar(disOffsetZ * size));
	max.set(scalar((disOffsetX+DISTRICT_VERTS) * size), maxHigh, scalar((disOffsetZ+DISTRICT_VERTS) * size));
	box.pmin = min;
	box.pmax = max;
	this->modelNode->SetBoundingBox(box);*/
}

void 
DistrictNodeInstance::Clear()
{
	
}

void
DistrictNodeInstance::AllocMeshPool()
{
	if (this->curGroup == -1)
	{
		if (this->meshPool.isvalid() && this->meshPool->Full())
			this->modelInstance.downcast<TerrainInstance>()->UpdateMeshPool();

		this->meshPool = this->modelInstance.downcast<TerrainInstance>()->GetMeshPool();
		this->curGroup = this->meshPool->Alloc(NULL);
		n_assert(this->curGroup != -1);

		Reset();
	}
}

void 
DistrictNodeInstance::FreeMeshPool(IndexT frame)
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

} // namespace Models
