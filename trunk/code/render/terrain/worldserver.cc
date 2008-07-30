//------------------------------------------------------------------------------
//  modelserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "Terrain/worldserver.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "math/vector3.h"
#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "terrain/streamterrainloader.h"
#include "terrain/terraintile.h"
#include "terrain/managedterraintile.h"
#include "terrain/world.h"
#include "Terrain/managedworld.h"

namespace Terrain
{
ImplementClass(Terrain::WorldServer, 'WDSR', Core::RefCounted);
ImplementSingleton(Terrain::WorldServer);

using namespace Core;
using namespace Util;
using namespace IO;
using namespace Attr;
using namespace Resources;
using namespace Math;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
WorldServer::WorldServer() :
    isOpen(false),
	managedWorld(0),
	camera(0),
	stage(0),
	chunkCache(0),
	oob(false)
{
    ConstructSingleton;

	for (SizeT i = 0; i < 3; i++)
	{
		for (SizeT j = 0;j < 3; j++)
		{
			this->curMaptile[i][j] = 0;
		}
	}

	for (SizeT i = 0; i < MAPTILECACHESIZE; i++)
		this->mapTileCache[i] = 0;
}

//------------------------------------------------------------------------------
/**
*/
WorldServer::~WorldServer()
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
WorldServer::Open()
{
    n_assert(!this->IsOpen())
    this->isOpen = true;

	// setup resource mapper for world
	Ptr<SimpleResourceMapper> resMapper = SimpleResourceMapper::Create();
	resMapper->SetPlaceholderResourceId(ResourceId("mdl:system/placeholder.n2"));
	resMapper->SetResourceClass(World::RTTI);
	resMapper->SetResourceLoaderClass(StreamTerrainLoader::RTTI);
	resMapper->SetManagedResourceClass(ManagedWorld::RTTI);
	resMapper->SetAsyncEnabled(false);	// 主线程加载
	ResourceManager::Instance()->AttachMapper(resMapper.upcast<ResourceMapper>());

	// terrain tile resource
	resMapper = SimpleResourceMapper::Create();
	resMapper->SetPlaceholderResourceId(ResourceId("mdl:system/placeholder.n2"));
	resMapper->SetResourceClass(TerrainTile::RTTI);
	resMapper->SetResourceLoaderClass(StreamTerrainLoader::RTTI);
	resMapper->SetManagedResourceClass(ManagedTerrainTile::RTTI);
	ResourceManager::Instance()->AttachMapper(resMapper.upcast<ResourceMapper>());

	this->chunkCache = TerrainChunkCache::Create();
	this->chunkCache->CreateChunkCache(256);
}

//------------------------------------------------------------------------------
/**
*/
void
WorldServer::Close()
{
    n_assert(this->IsOpen());

	// detach our model resource mapper
	ResourceManager::Instance()->RemoveMapper(World::RTTI);
	ResourceManager::Instance()->RemoveMapper(TerrainTile::RTTI);

	UnloadWorld();

	this->chunkCache = 0;

	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
WorldServer::LoadWorld(const ResourceId& worldName)
{
	if (this->managedWorld.isvalid())
		n_assert(0);
	String wName;
	wName.Format("wow:World\\Maps\\%s\\%s.wdt", worldName.Value().AsCharPtr() , worldName.Value().AsCharPtr());
	this->managedWorld = ResourceManager::Instance()->CreateManagedResource(World::RTTI, wName).downcast<ManagedWorld>();

	this->baseName = worldName;

    if (this->camera.isvalid())
    {
        vector pos = this->camera->GetTransform().getrow3();
        EnterTile(int(pos.x() / TILESIZE), int(pos.z() / TILESIZE));
    }
}

//------------------------------------------------------------------------------
/**
	使用LoadmanagedWMO创建的ManagedWorld资源必需手动调用DiscardManagedWMO释放，否则
	会出错，因为在ResourceManager里面释放资源后还需要将对象清空，这时如果不是创建
	的位置传入指针，就会出现针指被清0，从而导致出错。

	必需哪里Load就要哪里Discard！！！
*/
void
WorldServer::UnloadWorld()
{
	if (this->managedWorld.isvalid())
	{
		ResourceManager::Instance()->DiscardManagedResource(this->managedWorld.upcast<ManagedResource>());
		this->managedWorld = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldServer::OnFrame()
{
	if (this->camera.isvalid())
	{
		vector pos = this->camera->GetTransform().getrow3();
		if (this->prePos != pos)
		{
			CheckTile(pos);
			this->prePos = pos;

			//UpdateVisiableChunk();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
WorldServer::CheckValidTile(int i, int j)const
{
	return i>=0 && j >= 0 && i<64 && j<64;
}

//------------------------------------------------------------------------------
/**
	摄像机改变时判断是否需要加载新的tile
*/
void
WorldServer::CheckTile(const vector& pos)
{
	//vector pos = this->camera->GetTransform().getrow3();
	if (!this->curMaptile[1][1].isvalid())
	{
		EnterTile(int(pos.x() / TILESIZE), int(pos.z() / TILESIZE));
	}
	else if (oob)
	{
		float2 centerPos = this->curMaptile[1][1]->GetPos();
		if (oob || (pos.x() < centerPos.x() || (pos.x() > (centerPos.x() + TILESIZE))
				|| pos.z() < centerPos.y() || (pos.z() > (centerPos.y() + TILESIZE))))
		{
			EnterTile(int(pos.x() / TILESIZE), int(pos.z() / TILESIZE));
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
WorldServer::EnterTile(int x, int z)
{
    // test
    static bool IsFirst = false;
    if (IsFirst)
        return ;
    IsFirst = true;
    x = 36;
    z = 26;

    char name[256];
	sprintf(name,"wow:World\\Maps\\%s\\%s_%d_%d.adt", this->baseName.Value().AsCharPtr(), this->baseName.Value().AsCharPtr(), x, z);

    this->curMaptile[1][1] = TerrainEntity::Create();
    this->curMaptile[1][1]->SetResourceId(ResourceId(name));
    this->curMaptile[1][1]->SetXZ(x, z);
    this->stage->AttachEntity(this->curMaptile[1][1].upcast<GraphicsEntity>());

    return;


	if (!CheckValidTile(x,z)) 
	{
		this->oob = true;
		return;
	}
	else
		this->oob = !this->managedWorld->IsValidTile(z, x);

	cx = x;
	cz = z;
	for (int j=0; j<3; j++) {
		for (int i=0; i<3; i++) {
			curMaptile[j][i] = LoadTile(x-1+i, z-1+j);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Ptr<TerrainEntity>& 
WorldServer::LoadTile(int x, int z)
{
	if (!CheckValidTile(x, z) || !this->managedWorld->IsValidTile(z, x))
		return Ptr<TerrainEntity>(0);

	vector pos(x * TILESIZE, 0, z * TILESIZE);
	IndexT firstnull = MAPTILECACHESIZE;
	for (IndexT i = 0; i < MAPTILECACHESIZE; i++)
	{
		if (this->mapTileCache[i].isvalid())
		{
			if (this->mapTileCache[i]->GetX() == x && this->mapTileCache[i]->GetZ() == z)
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
			int X = this->mapTileCache[i]->GetX();
			int Z = this->mapTileCache[i]->GetZ();

			score = int(n_abs(X - cx) + n_abs(Z - cz));
			if (score > maxscore)
			{
				maxscore = score;
				maxidx = i;
			}
		}

		// 释放MapTile相关的资源
		this->mapTileCache[maxidx] = 0;
        
		firstnull = maxidx;
	}

	char name[256];
	sprintf(name,"wow:World\\Maps\\%s\\%s_%d_%d.adt", this->baseName.Value().AsCharPtr(), this->baseName.Value().AsCharPtr(), x, z);

    this->mapTileCache[firstnull] = TerrainEntity::Create();
    this->mapTileCache[firstnull]->SetResourceId(ResourceId(name));
    this->mapTileCache[firstnull]->SetXZ(x, z);
    this->stage->AttachEntity(this->mapTileCache[firstnull].upcast<GraphicsEntity>());

	return this->mapTileCache[firstnull];
}

} // namespace Models