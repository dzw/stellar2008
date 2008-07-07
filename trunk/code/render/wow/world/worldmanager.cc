//------------------------------------------------------------------------------
//  modelserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/worldmanager.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "math/vector3.h"
#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "wow/world/streamworldloader.h"
#include "wow/wmo/wmoserver.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "graphics/MapTileentity.h"
#include "resources/sharedresourceserver.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "wow/world/Terrain/Terrainentity.h"

namespace WOW
{
ImplementClass(WOW::WorldManager, 'WODM', Core::RefCounted);
ImplementSingleton(WOW::WorldManager);

using namespace Core;
using namespace Util;
using namespace IO;
using namespace Attr;
using namespace Resources;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
WorldManager::WorldManager() :
    isOpen(false),
	loading(false)
{
    ConstructSingleton;

	for (IndexT i = 0; i < 3; i++)
	{
		for (IndexT j = 0; j < 3; j++)
		{
			this->curMaptile[i][j] = 0;
		}
	}

	for (IndexT i = 0; i < MAPTILECACHESIZE; i++)
	{
		this->mapTileCache[i] = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
WorldManager::~WorldManager()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
WorldManager::Open()
{
    n_assert(!this->IsOpen())

	this->worldServer = WorldServer::Create();
	this->worldServer->Open();

	SetupIndexBuffer();

	// 创建9*16*16个mesh
	/*Array<PrimitiveGroup> primGroups;
	Array<VertexComponent> vertexComponents;
	ReadPrimitiveGroups(primGroups);
	SetupVertexComponents(vertexComponents);

	SetupIndexBuffer();

	meshs.SetSize(9*16*16);*/

	//int totalM2Model = 0;
	//String meshName = "ck_";

	//for (IndexT n = 0; n < 9*16*16; n++)
	//{
	//	meshName.AppendInt(totalM2Model++);
	//	/*meshs[n] =*/ CreateMesh(Resources::ResourceId(meshName), vertexComponents, primGroups);
	//}

	// 找到摄像机，有可能存在多个？
	GraphicsServer* gfxServer = GraphicsServer::Instance();
	Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));
	Array<Ptr<GraphicsEntity>> modelentites = stage->GetEntitiesByType(GraphicsEntity::CameraType);
	//for (SizeT i = 0; i < modelentites.Size(); i++)
	if (modelentites.Size() > 0)
		this->camera = modelentites[0];

    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
WorldManager::Close()
{
    n_assert(this->IsOpen());

	this->isOpen = false;

	WorldServer::Instance()->DiscardManagedWorld(this->managedWDT);
	this->managedWDT = 0;

	for (IndexT i = 0; i < this->gwmois.Size(); i++)
	{
		WMOServer::Instance()->DiscardManagedWMO(this->gwmois[i]);
	}
	this->gwmois.Clear();

	

	GraphicsServer* gfxServer = GraphicsServer::Instance();
	Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

	for (IndexT i = 0; i < MAPTILECACHESIZE; i++)
	{
		if (this->mapTileCache[i].isvalid())
			stage->RemoveEntity(this->mapTileCache[i].upcast<GraphicsEntity>());
		this->mapTileCache[i] = 0;
	}
	
	for (IndexT i = 0; i < 3; i++)
	{
		for (IndexT j = 0; j < 3; j++)
		{
			this->curMaptile[i][j] = 0;
		}
	}

	/*for (IndexT n = 0; n < 9; n++)
	{
		for (IndexT i = 0; i < 16; i++)
		{
			for (IndexT j = 0; j < 16; j++)
			{
				SharedResourceServer::Instance()->UnregisterSharedResource(meshs[i].upcast<Resources::Resource>());
			}
		}
	}
	meshs.SetSize(0);*/
	
	/*for (IndexT i = 0; i < this->managedTexture.Size(); i++)
	{
		ManagedTexture
	}
	this->managedTexture.Clear();*/

	this->worldServer->Close();
	this->worldServer = 0;

	this->camera = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldManager::Go(/*const vector& pos*/)
{
	vector pos = this->camera->GetTransform().getrow3();
	if (this->curMaptile[1][1].isvalid())
	{
		float2 centerPos = this->curMaptile[1][1]->GetPos();
		if (pos.x() < centerPos.x() || (pos.x() > (centerPos.x() + TILESIZE))
			|| pos.z() < centerPos.y() || (pos.z() > (centerPos.y() + TILESIZE)))
		{
			int x = (int)pos.x();
			int z = (int)pos.z();
			EnterTile(int(x / TILESIZE), int(z / TILESIZE));

			this->loading = false;
		}
	}
	else
	{
		if (this->loading)
		{
			int x = (int)pos.x();
			int z = (int)pos.z();
			EnterTile(int(x / TILESIZE), int(z / TILESIZE));

			this->loading = false;
		}
	}
	

	// 判断是否需要加载地图
	//if (0&& 0 != curMaptile[1][1] && 0 != curMaptile[1][1]->maptile)
	//{
	//	//vector curpos = curMaptile[1][1]->maptile->GetTransform().row3();
	//	if ( (pos.x() < curMaptile[1][1]->xbase) || 
	//		 (pos.x() > (curMaptile[1][1]->xbase + TILESIZE)) ||
	//		 (pos.z() < curMaptile[1][1]->zbase) || 
	//		 (pos.z() > (curMaptile[1][1]->zbase + TILESIZE)) )
	//	{
	//		int x = (int)pos.x();
	//		int z = (int)pos.z();
	//		EnterTile(int(x / TILESIZE), int(z / TILESIZE));
	//	}
	//}
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldManager::InitWorld(const String& worldName)
{
	Memory::Clear(this->maps, 64*64);

	//InitGlobalVBOs();
	//SetupIndexBuffer();

	/*for (int i=0; i<MAPTILECACHESIZE; i++) maptilecache[i] = 0;

	autoheight = false;

	for (int j=0; j<64; j++) {
		for (int i=0; i<64; i++) {
			lowrestiles[j][i] = 0;
		}
	}

	gnWMO = 0;
	nMaps = 0;
	water = 0;

	time = 1450;
	animtime = 0;

	ex = ez = -1;

	drawfog = false;*/

	basename = worldName;

	char szName[256];
	sprintf(szName, "wow:World\\Maps\\%s\\%s.wdt", basename.AsCharPtr(), basename.AsCharPtr());

	this->managedWDT = WorldServer::Instance()->LoadManagedWorld(ResourceId(szName));
	this->managedWDT->SetPriority(ManagedResource::HighestPriority);

	this->loading = true;
	//skies = 0;
	//ol = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldManager::EnterTile(int x, int z)
{
	if (!oktile(x,z) /*|| !this->maps[z][x]*/) 
		return;

	for (int j=0; j<3; j++) {
		for (int i=0; i<3; i++) {
			curMaptile[j][i] = LoadTile(x-1+i, z-1+j);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<TerrainEntity>&
WorldManager::LoadTile(int x, int z)
{
	/*if (!oktile(x,z) || !maps[z][x]) {
		return 0;
	}*/

	// 暂时只加载一个MAPTILE
	/*static int nload = 0;
	if (nload >= 1)
		return 0;
	else
		nload++;

	x = 34;
	z = 26;*/

	//x = 36;
	//z = 26;

	//x = 30;
	//z = 51;


	GraphicsServer* gfxServer = GraphicsServer::Instance();
	Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));


	vector pos(x * TILESIZE, 0, z * TILESIZE);
	IndexT firstnull = MAPTILECACHESIZE;
	for (IndexT i = 0; i < MAPTILECACHESIZE; i++)
	{
		if (this->mapTileCache[i].isvalid())
		{
			if (this->mapTileCache[i]->GetRow() == x && this->mapTileCache[i]->GetCol() == z)
				return this->mapTileCache[i];
		}
		else
		{
			if (i < firstnull)
				firstnull = i;
		}
	}

	if (firstnull == MAPTILECACHESIZE)
	{
		int score, maxscore = 0, maxidx = 0; 
		for (IndexT i = 0; i < MAPTILECACHESIZE; i++)
		{
			float2 tilePos = this->mapTileCache[i]->GetPos();

			score = int(n_abs(tilePos.x() - pos.x()) + n_abs(tilePos.y() - pos.z()));
			if (score > maxscore)
			{
				maxscore = score;
				maxidx = i;
			}
		}

		// 释放MapTile相关的资源
		stage->RemoveEntity(this->mapTileCache[maxidx].upcast<GraphicsEntity>());
		this->mapTileCache[maxidx] = 0;

		firstnull = maxidx;
	}

	char name[256];
	sprintf(name,"wow:World\\Maps\\%s\\%s_%d_%d.adt", basename.AsCharPtr(), basename.AsCharPtr(), x, z);

	this->mapTileCache[firstnull] = TerrainEntity::Create();
	this->mapTileCache[firstnull]->ChangeTerrain(ResourceId(name), x, z);

	stage->AttachEntity(this->mapTileCache[firstnull].upcast<GraphicsEntity>());

	return this->mapTileCache[firstnull];
}

//------------------------------------------------------------------------------
/**
*/
bool 
WorldManager::oktile(int i, int j)
{
	return i>=0 && j >= 0 && i<64 && j<64;
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldManager::AddManagedWMO(const Util::String& path)
{
	n_assert(!path.IsEmpty());

	this->gwmos.Append(path);
	this->gwmois.Append(WMOServer::Instance()->LoadManagedWMO(path));
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldManager::SetMaps(bool* maps, int numMaps)
{
	Memory::Copy(maps, this->maps, 64*64);
	this->nMaps = numMaps;
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldManager::SetWMOs(int numWmos)
{
	this->gnWMO = numWmos;
}

//------------------------------------------------------------------------------
/**
*/
void
WorldManager::Update()
{
	// 加载完资源就没用了，删除
	/*for (IndexT i = 0; i < this->managedWorlds.Size(); i++)
	{
		if (this->managedWorlds[i]->GetModel()->IsLoaded())
		{
			WorldServer::Instance()->DiscardManagedWorld(this->managedWorlds[i]);
			this->managedWorlds.EraseIndex(i);
		}
	}*/

	// 临时判断，只加载一个MAPTile
	//if (this->managedWDT->GetModel()->IsLoaded())
	//{
	//	for (IndexT i = 0; i < 64; i++)
	//	{
	//		for (IndexT j = 0; j < 64; j++)
	//		{
	//			//if (this->maps[i][j])
	//			{
	//				EnterTile(i, j);
	//				//return;
	//			}
	//		}
	//	}
	//}

	if (this->managedWDT->GetModel()->IsLoaded())
	{
		Go();
	}
	for (IndexT i = 0; i < 3; i++)
	{
		for (IndexT j = 0; j < 3; j++)
		{
			if (this->curMaptile[i][j].isvalid() && this->curMaptile[i][j]->Update(3) > 0)
				return;
		}
	}
	// 暂时将地图的模型放到这里加载，因为直接在Loader里面加载会导至加载队列出错。
	/*if (0&&this->loadModels.Size() > 0)
	{
		GraphicsServer* gfxServer = GraphicsServer::Instance();
		Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

		for (SizeT i = 0; i < this->loadModels.Size(); i++)
		{
			stage->AttachEntity(this->loadModels[i].upcast<GraphicsEntity>());
		}
		this->loadModels.Clear();
	}*/
}


//------------------------------------------------------------------------------
/**
*/
void
WorldManager::SetupIndexBuffer()
{
	n_assert(!this->indexBuffer.isvalid());

	//uint16 *indices = InitIndexBuffer();

	//uint16 indexbuf[stripsize];
	//uint16 defstrip[stripsize];// = n_new_array(uint16, stripsize);
	//for (int i=0; i<stripsize; i++) defstrip[i] = i; 
	//uint16 *indices = indexbuf;//n_new_array(uint16, stripsize);
	//for (int row=0; row<8; row++) {
	//	uint16 *thisrow = &defstrip[IndexMapBuf(0,row*2)];
	//	uint16 *nextrow = &defstrip[IndexMapBuf(0,(row+1)*2)];

	//	for (int col=0; col<9; col++) {
	//		*indices++ = nextrow[col];
	//		*indices++ = thisrow[col];
	//	}
	//	if (row < 7)
	//	{
	//		*indices++ = nextrow[8];
	//		*indices++ = nextrow[8];
	//	}

	//	/*if (row>0) *indices++ = thisrow[0];
	//	for (int col=0; col<9; col++) {
	//		*indices++ = thisrow[col];
	//		*indices++ = nextrow[col];
	//	}
	//	if (row<7) *indices++ = nextrow[8];*/
	//}

	//n_delete_array(defstrip);

	uint16 indexbuf[samplerstripsize];
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			indexbuf[(j*8+i)*12+0] = j*9+j*8+i;
			indexbuf[(j*8+i)*12+1] = (j+1)*9+j*8+i;
			indexbuf[(j*8+i)*12+2] = (j+1)*9+(j+1)*8+i;

			indexbuf[(j*8+i)*12+3] = j*9+j*8+i;
			indexbuf[(j*8+i)*12+4] = j*9+j*8+i+1;
			indexbuf[(j*8+i)*12+5] = (j+1)*9+j*8+i;

			indexbuf[(j*8+i)*12+6] = (j+1)*9+j*8+i;
			indexbuf[(j*8+i)*12+7] = j*9+j*8+i+1;
			indexbuf[(j*8+i)*12+8] = (j+1)*9+(j+1)*8+i+1;

			indexbuf[(j*8+i)*12+9] = (j+1)*9+(j+1)*8+i;
			indexbuf[(j*8+i)*12+10] = (j+1)*9+j*8+i;
			indexbuf[(j*8+i)*12+11] = (j+1)*9+(j+1)*8+i+1;
		}
	}


	// setup a new index buffer
	this->indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, samplerstripsize, indexbuf, sizeof(uint16)*samplerstripsize);
	this->indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->indexBuffer->Load();
	this->indexBuffer->SetLoader(0);
	//n_delete_array(indices);
	n_assert(this->indexBuffer->GetState() == IndexBuffer::Loaded);
	
	//LOD
	/*indexbuf[(j*8)*6+0] = j*9+j*8+i;
	indexbuf[(j*8)*6+1] = (j+1)*9+(j+1)*8+i;
	indexbuf[(j*8)*6+2] = j*9+j*8+i+1;

	indexbuf[(j*8)*6+3] = (j+1)*9+(j+1)*8+i+1;
	indexbuf[(j*8)*6+4] = j*9+j*8+i+1;
	indexbuf[(j*8)*6+5] = (j+1)*9+(j+1)*8+i;
	*/
}

//------------------------------------------------------------------------------
/**
*/
int 
WorldManager::IndexMapBuf(int x, int y)
{
	return ((y+1)/2)*9 + (y/2)*8 + x;
}


} // namespace Models