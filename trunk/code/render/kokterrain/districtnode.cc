//------------------------------------------------------------------------------
//  districtnode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/districtnode.h"
#include "kokterrain/terrain.h"
#include "memory/memory.h"

#include "coregraphics/vertexchunkpool.h"
#include "coregraphics/indexbufferpool.h"
#include "coregraphics/renderdevice.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/cameraentity.h"
#include "kokterrain/terrainentity.h"

namespace KOK
{
ImplementClass(KOK::DistrictNode, 'DTND', Models::StateNode);

using namespace Util;
using namespace Attr;
using namespace Resources;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Models;
using namespace Math;
using namespace Memory;

//------------------------------------------------------------------------------
/**
*/
DistrictNode::DistrictNode():
	disX(-1),
	disZ(-1),
	vertices(0),
	vertexSize(0),
	indices(0),
	indexSize(0),
	drawTable(0),
	shadowIndexSize(0),
	vertexStart(-1),
	indexStart(-1)
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
//Ptr<ModelNodeInstance>
//DistrictNode::CreateNodeInstance() const
//{
//    Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) DistrictNodeInstance::Create();
//    return newInst;
//}

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

void
DistrictNode::CreateMeshData()
{
	MemoryMark mm(MemStack);

	Ptr<Terrain> terr = this->model.downcast<Terrain>();
	TerrainMeshData** meshData = terr->GetMeshData();
	TerrainInfo terrInfo = terr->GetTerrainInfo();

	TerrainMeshData** terrMeshData = new(MemStack, DISTRICT_VERTS * DISTRICT_VERTS)TerrainMeshData*;
	for (SizeT m = 0; m < DISTRICT_VERTS; m++)
	{
		for (SizeT n = 0; n < DISTRICT_VERTS; n++)
		{
			int iX = (this->disX*DISTRICT_VERTS) + n;
			int iY = (this->disZ*DISTRICT_VERTS) + m;
			terrMeshData[n + m * DISTRICT_VERTS] = meshData[iX + iY * terrInfo.GetTileCountX()];
		}
	}

	UpdateDrawTable(terrMeshData);
	//CalWaterHeight(terrMeshData, fMin, fMax);
}

void
DistrictNode::UpdateDrawTable(TerrainMeshData** pMeshDatas)
{
	Ptr<Terrain> terr = this->model.downcast<Terrain>();

	int iSize = DISTRICT_VERTS * DISTRICT_VERTS;

	int iDrawIndexSize = 0, iDrawVertexSize = 0;

	int iLayeSize = terr->GetTextureCount() * 4;

	int iShadowIdxSize = 0;

	int i,j,k;

	float SHADOW_OFFSET = 0.9f;//5.0f/8.0f;
	float ERROR_SHADOW_UV = ( SHADOW_OFFSET / 512.0f);
	float Scale;
	Scale = ( 512.0f - 2.0f * SHADOW_OFFSET ) / ( 320.0f * 512.0f );

	float FishX, FishZ;
	FishX = (float)( this->tileStartX / 64 ) * 320;
	FishZ = (float)( this->tileStartY / 64 ) * 320;

	DWORD dwCliff1IndexSize = 0, dwCliff1VertexSize = 0;
	DWORD dwCliff2IndexSize = 0, dwCliff2VertexSize = 0;

	if( !terr->GetTextureCount() )
	{
		return;
	}

	TerrainMeshData* pMeshData;

	// 计算要画点的Index数量

	for( i = 0; i < iSize; i++ )
	{
		pMeshData = pMeshDatas[i];

		if(1/*!pMeshData->m_pCliffData*/ )
		{
			// 为地表部分
			for( j = 0; j < 4; j++ )
			{
				if( pMeshData->byLayer[j] )
				{
					iDrawIndexSize += 6;
				}
			}
			iDrawVertexSize += 4;
			iShadowIdxSize++;
		}
		else
		{
			//为悬崖的部份，8*8集中一起画
			/*cSubMeshSerializer* pTempMesh = pMeshData->m_pCliffData->getSubMeshSerializer();

			if( pMeshData->m_byCliffTableID == 0x01 )
			{
				dwCliff1IndexSize += pTempMesh->getSubMeshIndexBufferSize();
				dwCliff1VertexSize += pTempMesh->getSubMeshVerticesBufferSize();
			}
			else if( pMeshData->m_byCliffTableID == 0x02 )
			{
				dwCliff2IndexSize += pTempMesh->getSubMeshIndexBufferSize();
				dwCliff2VertexSize += pTempMesh->getSubMeshVerticesBufferSize();
			}*/
		}
	}
	
	this->vertexSize = iDrawVertexSize;
	this->vertices = new TileMesh[iDrawVertexSize];

	this->indexSize = iDrawIndexSize + this->shadowIndexSize;
	this->shadowIndexSize = iShadowIdxSize * 6;
	this->indices = new WORD[this->indexSize];

	this->drawTable = new TileDrawTable[iLayeSize];

	iDrawVertexSize = 0 ;

	this->boundingBox.pmin = vector(9999.0f, 9999.0f, 9999.0f);
	this->boundingBox.pmax = vector(-9999.0f, -9999.0f, -9999.0f);

	for( i = 0; i < iSize; i++ ) // 縥计
	{
		pMeshData = pMeshDatas[i];

		if( 1/*!pMeshData->m_pCliffData*/ )
		{
			this->vertices[iDrawVertexSize + 0] = pMeshData->meshData[0];
			this->vertices[iDrawVertexSize + 1] = pMeshData->meshData[1];
			this->vertices[iDrawVertexSize + 2] = pMeshData->meshData[2];
			this->vertices[iDrawVertexSize + 3] = pMeshData->meshData[3];

			for( int iIndex = 0; iIndex < 4; iIndex++ )
			{
				const vector3& vPos = pMeshData->meshData[iIndex].p;
				if( vPos.x < boundingBox.pmin.x() ) boundingBox.pmin.x() = vPos.x;
				if( vPos.x > boundingBox.pmax.x() ) boundingBox.pmax.x() = vPos.x;
				if( vPos.y < boundingBox.pmin.y() ) boundingBox.pmin.y() = vPos.y;
				if( vPos.y > boundingBox.pmax.y() ) boundingBox.pmax.y() = vPos.y;
				if( vPos.z < boundingBox.pmin.z() ) boundingBox.pmin.z() = vPos.z;
				if( vPos.z > boundingBox.pmax.z() ) boundingBox.pmax.z() = vPos.z;
			}

			pMeshData->wVertexStartIndex = iDrawVertexSize;

			iDrawVertexSize += 4;
		}
	}

	int iLayer;
	int iDrawTableID = 0;

	iDrawIndexSize = 0;

	for( j = 0; j < 4; j++ ) // 层级
	{
		for( k = 0; k < (int)terr->GetTextureCount(); k++ ) // 贴图
		{
			iDrawTableID = k + j * terr->GetTextureCount();
			this->drawTable[iDrawTableID].FaceStart = 0;
			this->drawTable[iDrawTableID].FaceCount = 0;

			this->drawTable[iDrawTableID].Layer = j;
			this->drawTable[iDrawTableID].Texture = k + 1;

			for( i = 0; i < iSize; i++ ) // 地砖数
			{
				pMeshData = pMeshDatas[i];

				iLayer = pMeshData->byLayer[j];

				if( 1/*!pMeshData->m_pCliffData*/ )
				{
					// 地表
					if( iLayer == ( k + 1 ) )
					{
						if( !this->drawTable[iDrawTableID].bUpdate )
						{
							this->drawTable[iDrawTableID].FaceStart = iDrawIndexSize;
							this->drawTable[iDrawTableID].bUpdate   = true;
						}

						this->drawTable[iDrawTableID].FaceCount += 6;

						WORD wTileMeshIndexS = pMeshData->wVertexStartIndex;

						this->indices[iDrawIndexSize + 0] = wTileMeshIndexS + 0;
						this->indices[iDrawIndexSize + 1] = wTileMeshIndexS + 1;
						this->indices[iDrawIndexSize + 2] = wTileMeshIndexS + 2;

						this->indices[iDrawIndexSize + 3] = wTileMeshIndexS + 1;
						this->indices[iDrawIndexSize + 4] = wTileMeshIndexS + 3;
						this->indices[iDrawIndexSize + 5] = wTileMeshIndexS + 2;

						iDrawIndexSize += 6;
					}
				}
			}
			this->drawTable[iDrawTableID].bUpdate = false;
		}
	}
}

void
DistrictNode::Render()
{
	//RenderDevice::Instance()->SetPrimitiveGroup(group);
	TerrainEntity::Instance()->terrMeshPool->ApplyPrimitive(group);
	RenderDevice::Instance()->Draw();
}  

void
DistrictNode::CalcSquareDistance()
{
	bbox box = this->boundingBox;
	Ptr<CameraEntity> camera = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
	box.transform(camera->GetViewTransform());
	this->squareDistance = box.pmax.z();
}

void
DistrictNode::NotifyVisible(IndexT frameIndex)
{
	this->frameIndex = frameIndex;

	Reset();
}

void 
DistrictNode::Reset()
{
	if (vertexStart != -1)
		return;

	/*const Ptr<VertexChunkPool>& pool = TerrainEntity::Instance()->GetVertexChunkPool();
	if (pool->Full())
		this->model.downcast<Terrain>()->UpdateVertexPool();
	
	vertexStart = pool->Alloc((void*)vertices);
	n_assert(vertexStart != -1);*/
	

	

	// 填充顶点
	const Ptr<VertexBufferPool>& vbPool = TerrainEntity::Instance()->terrMeshPool->GetVertexPool();
	if (!vbPool->CheckSpace((SizeT)vertexSize))
		vbPool->FlushAtFrameStart();

	void* ptr = NULL;
	vbPool->Lock(ptr, vertexSize, vertexStart);
	if (ptr != NULL)
	{
		Memory::Copy(vertices, ptr, sizeof(TileMesh)*vertexSize);
		vbPool->Unlock();
	}

	// index buffer
	//const Ptr<IndexBufferPool>& ibPool = TerrainEntity::Instance()->GetIndexBufferPool();
	const Ptr<IndexBufferPool>& ibPool = TerrainEntity::Instance()->terrMeshPool->GetIndexPool();
	if (!ibPool->CheckSpace((SizeT)indexSize))
		ibPool->FlushAtFrameStart();

	//void* ptr;
	ibPool->Lock(ptr, indexSize, indexStart);
	if (ptr != NULL)
	{
		Memory::Copy(indices, ptr, sizeof(WORD)*indexSize);
		ibPool->Unlock();
	}
}

bool 
DistrictNode::SetRenderGroup(int pass, int texId)
{
	if (drawTable[pass].FaceCount <= 0 || 
		!drawTable[pass].Texture || 
		(drawTable[pass].Texture - 1) != texId)
		return false; 

	group.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	group.SetBaseVertex(vertexStart);
	group.SetNumVertices(vertexSize);
	group.SetBaseIndex(drawTable[pass].FaceStart + indexStart);
	group.SetNumIndices(drawTable[pass].FaceCount);
	/*group.SetBaseIndex(ibStart);
	group.SetNumIndices(indexSize);*/

	return true;
}

}