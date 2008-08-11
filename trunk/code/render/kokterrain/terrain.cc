//------------------------------------------------------------------------------
//  terrain.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terrain.h"
#include "kokterrain/districtnode.h"
#include "models/attributes.h"
#include "kokterrain/terraininstance.h"

namespace KOK
{
ImplementClass(KOK::Terrain, 'TERN', Models::Model);

using namespace Util;
using namespace Resources;
using namespace Models;
using namespace CoreGraphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Terrain::Terrain():
	tileMeshScale(1),
	tilePosOffset(0.0)
{
	
	
	// 设置district缓冲
	//for (SizeT i = 0; i < DISTRICTCACHESIZE; i++)
	//{
	//	Ptr<DistrictNode> node = DistrictNode::Create();
	//	//node->Init(0, 0);
	//	String name;
	//	name.Format("DIST%d", i);
	//	node->SetName(Resources::ResourceId(name));
	//	node->SetString(Attr::Shader, "shd:terrain");
	//	//node->SetTerrainMeshGrid(this->terrain->terrMeshGrid);
	//	//node->SetPosition(-1, -1);
	//	this->AttachNode(node.upcast<Models::ModelNode>());
	//}

	/*@test code
	bbox b;
	vector min, max;
	min.set(-1000., -500.0f, -1000.);
	max.set(1000., 500.0f, 1000.);
	b.pmin = min;
	b.pmax = max;
	this->SetBoundingBox(b);*/
}

//------------------------------------------------------------------------------
/**
*/
Terrain::~Terrain()
{
	this->terrMeshGrid = 0;
	this->cliffTable = 0;
}

Ptr<ModelInstance> 
Terrain::CreateInstance()
{
	Ptr<ModelInstance> modelInstance = (ModelInstance*)TerrainInstance::Create();
	this->AttachInstance(modelInstance);
	return modelInstance;
}

//------------------------------------------------------------------------------
/**
*/
void
Terrain::Unload()
{    
	// call parent class
    Model::Unload();
}

//------------------------------------------------------------------------------
/**
*/
void 
Terrain::ComputeTileMesh()
{
	//float size = (float)(COMP * this->tileMeshScale);
	//TerrainMeshData* pMeshData;
	//int iIndex = 0;
	//float shadowOffset = 1.0f;
	//float shadowSize = DISTRICT_VERTS * size * 8.0f;
	//float errorShadowUV = shadowOffset / 512.0f;
	//float uvScale = (512.0f - 2.0f*shadowOffset) / (shadowSize * 512.0f);
	//float uvOffsetX, uvOffsetZ;

	//// 阴影为8X8个District使用一张贴图，贴图轴内缩shadowOffset个pixel值
	//for (SizeT j = 0; j < this->terrInfo.GetTileCountX(); j++)
	//{
	//	uvOffsetZ = (j/64) * shadowSize;
	//	for (SizeT i = 0; i < this->terrInfo.GetTileCountX(); i++)
	//	{
	//		uvOffsetX = (i/64) * shadowSize;

	//		iIndex = i + j * this->terrInfo.GetTileCountX();
	//		pMeshData = this->meshData[iIndex];

	//		pMeshData->meshData[0].p.set(i * size - this->tilePosOffset,
	//			this->terrMeshGrid->CalcTileHeightWithCliffWater(i, j),
	//			j * size - this->tilePosOffset);
	//		pMeshData->meshData[0].n = this->terrMeshGrid->GetGridNormal(i, j);
	//		pMeshData->byWaterLevel[0] = this->terrMeshGrid->GetGridWaterLevel(i, j);
	//		pMeshData->meshData[0].t5.set((pMeshData->meshData[0].p.x() + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
	//			(pMeshData->meshData[0].p.z() + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
	//		this->terrMeshGrid->GetGridPosition(i, j) = pMeshData->meshData[0].p;

	//		pMeshData->meshData[1].p.set(i * size - this->tilePosOffset, 
	//			this->terrMeshGrid->CalcTileHeightWithCliffWater(i, j+1),
	//									(j+1) * size - this->tilePosOffset);
	//		pMeshData->meshData[1].t5.set((pMeshData->meshData[1].p.x() + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
	//			(pMeshData->meshData[1].p.z() + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
	//		if (j == this->terrInfo.GetTileCountX() - 1)
	//			this->terrMeshGrid->GetGridPosition(i, j+1) = pMeshData->meshData[1].p;


	//		pMeshData->meshData[2].p.set((i+1) * size - this->tilePosOffset,
	//			this->terrMeshGrid->CalcTileHeightWithCliffWater(i+1, j),
	//									 j * size - this->tilePosOffset);
	//		pMeshData->meshData[2].n = this->terrMeshGrid->GetGridNormal(i+1, j);
	//		pMeshData->byWaterLevel[2] = this->terrMeshGrid->GetGridWaterLevel(i+1, j);
	//		pMeshData->meshData[2].t5.set((pMeshData->meshData[2].p.x() + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
	//			(pMeshData->meshData[2].p.z() + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
	//		if (i == (this->terrInfo.GetTileCountX() - 1))
	//			this->terrMeshGrid->GetGridPosition(i+1, j) = pMeshData->meshData[2].p;

	//		pMeshData->meshData[3].p.set((i+1) * size - this->tilePosOffset,
	//			this->terrMeshGrid->CalcTileHeightWithCliffWater(i+1, j+1),
	//			(j +1) * size - this->tilePosOffset);
	//		pMeshData->meshData[3].n = this->terrMeshGrid->GetGridNormal(i+1, j+1);
	//		pMeshData->byWaterLevel[3] = this->terrMeshGrid->GetGridWaterLevel(i+1, j+1);
	//		pMeshData->meshData[3].t5.set((pMeshData->meshData[3].p.x() + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
	//			(pMeshData->meshData[3].p.z() + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
	//		if (i == (this->terrInfo.GetTileCountX() - 1) && j == (this->terrInfo.GetTileCountX() - 1))
	//			this->terrMeshGrid->GetGridPosition(i+1, j+1) = pMeshData->meshData[3].p;


	//		UpdateTileTex(i, j);
	//	}
	//}
}

void
Terrain::UpdateTileTex(int iX, int iY)
{
	//if (iX < 0 || iX >= (int)this->terrInfo.GetTileCountX() ||
	//	iY < 0 || iY >= (int)this->terrInfo.GetTileCountX())
	//{
	//	return;
	//}

	//if (!this->meshData->isvalid())
	//	return;

	//int iIndex = iX + iY * this->terrInfo.GetTileCountX();
	//DWORD dwTexData = this->thingTex[iIndex]->dTile;
	//TerrainMeshData* pMeshData = this->meshData[iIndex];
	//stDWORD cellInfo;
	//BYTE byTexture;
	//BYTE byID;

	//// 首先取出底图的 Pattern Index
	//byTexture = (BYTE)( ( dwTexData >> 8 ) & 0xF );
	//// 接着取出底图的 Texture Index 0~17
	//byID = (BYTE)( dwTexData & 0xFF );
	//pMeshData->SetTextureData( byTexture, byID, 0 );

	//cellInfo.dwDWORD = this->thingTex[iIndex]->stTile.dwDWORD;

	//for( int i = 0; i < 3; i++ )
	//{
	//	byTexture = ( cellInfo.bBYTE[i] >> 4 ) & 0xF;
	//	if( byTexture )
	//	{
	//		byID = cellInfo.bBYTE[i] & 0xF;
	//		pMeshData->SetTextureData( byTexture, byID, i + 1 );
	//	}
	//}
}



void 
Terrain::SetMapSize(SizeT mapSize)
{
	this->terrInfo.SetTileCountX(mapSize);

	this->tilePosOffset = ((this->tileMeshScale - 1.0f) * COMP * this->terrInfo.GetTileCountX()) * 0.5f;

	if (!this->terrMeshGrid.isvalid())
		this->terrMeshGrid = TerrainMeshGrid::Create();
	this->terrMeshGrid->Init(this->terrInfo.GetGridCountX(), this->terrInfo.GetTotalGridCount());

	bbox box;
	vector min, max;
	min.set(-tilePosOffset, -500.0f, -tilePosOffset);
	max.set(scalar(GetMapWide() * GetTileSize() - tilePosOffset), 500.0f, scalar(GetMapWide() * GetTileSize() - tilePosOffset));
	box.pmin = min;
	box.pmax = max;
	this->SetBoundingBox(box);
}

Ptr<DistrictNode>
Terrain::CreateNewDistrict(int x, int z)
{
	Ptr<DistrictNode> newNode;
	String name;
	name.Format("dist_%2d_%2d", x,z);

	if (this->HasNode(name))
	{
		newNode = this->LookupNode(name).downcast<DistrictNode>();
		return newNode;
	}

	newNode = DistrictNode::Create();
	newNode->SetName(Resources::ResourceId(name));
	newNode->SetString(Attr::Shader, "shd:terrain");
	newNode->SetString(Attr::DiffMap0, "mapdata\\border0100.dds");
	newNode->SetTerrainMeshGrid(this->terrMeshGrid);
	newNode->SetPosition(x, z);
	this->AttachNode(newNode.upcast<Models::ModelNode>());
	newNode->LoadResources();

	return newNode;
}

} // namespace Models
