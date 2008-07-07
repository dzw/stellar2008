//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/Terrain/Terrainentity.h"
#include "models/modelserver.h"
#include "coregraphics/shaperenderer.h"
#include "resources/resourcemanager.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"

#include "wow/world/worldmanager.h"
#include "wow/world/Terrain/terrainchunkentity.h"
#include "wow/asyncload/msg/closemaptile.h"
#include "interface/iointerface.h"

namespace Graphics
{
ImplementClass(Graphics::TerrainEntity, 'TENE', Graphics::GraphicsEntity);

using namespace CoreGraphics;
using namespace Math;
using namespace Models;
using namespace Resources;
using namespace Util;
using namespace WOW;

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::TerrainEntity()
{
    this->SetType(TerrainType);
}

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::~TerrainEntity()
{
}

//------------------------------------------------------------------------------
/**
    This will initialize the managed model inside the TerrainEntity.
*/
void
TerrainEntity::OnActivate()
{
    n_assert(!this->IsActive());
    n_assert(this->resId.IsValid());
    GraphicsEntity::OnActivate();
    
    // note: we will remain invalid until at least our model has loaded
    this->SetValid(false);

	terrainChunk.SetSize(256);


    // 创建256个chunk
	GraphicsServer* gfxServer = GraphicsServer::Instance();
	Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

	for (IndexT i = 0; i < terrainChunk.Size(); i++)
	{
		if (!terrainChunk[i].isvalid())
		{
			terrainChunk[i] = TerrainChunkEntity::Create();

			stage->AttachEntity(terrainChunk[i].upcast<GraphicsEntity>());
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainEntity::OnDeactivate()
{
    n_assert(this->IsActive());

    // discard our managed model
	if (this->managedMapTile.isvalid())
	{
	    ResourceManager::Instance()->DiscardManagedResource(managedMapTile.upcast<ManagedResource>());
		this->managedMapTile = 0;
	}

	// 在这里释放会导致Stage计数出错!!!重新设计TerrainEntity，不用继承自GraphicsEntity，直接放到WorldManager管理即可！
	GraphicsServer* gfxServer = GraphicsServer::Instance();
	Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

	for (SizeT i = 0; i < this->terrainChunk.Size(); i++)
	{
		stage->RemoveEntity(this->terrainChunk[i].upcast<GraphicsEntity>());
		this->terrainChunk[i] = 0;
	}
	this->terrainChunk.SetSize(0);

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
	加载新的地型
*/
void
TerrainEntity::ChangeTerrain(const Resources::ResourceId& resId, int x, int z)
{
	if (managedMapTile.isvalid())
	{
		ResourceManager::Instance()->DiscardManagedResource(managedMapTile.upcast<ManagedResource>());
		this->managedMapTile = 0;
	}
	this->resId = resId;
	//this->SetResourceId(resId);
	this->SetValid(false);
	this->managedMapTile = ResourceManager::Instance()->CreateManagedResource(MapTile::RTTI, resId).downcast<ManagedMapTile>();
	this->ValidateMapTile();

	this->SetPos(x, z);
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainEntity::OnUpdate()
{
	this->ValidateMapTile();

	// important: call parent class!
    GraphicsEntity::OnUpdate();
}

//------------------------------------------------------------------------------
/**
	验证maptile加载完成，并创建terrain
*/
void 
TerrainEntity::ValidateMapTile()
{
    if (this->IsValid())
        return;

	if (this->managedMapTile.isvalid() && this->managedMapTile->GetState() == Resource::Loaded)
	{
		for (IndexT i = 0; i < terrainChunk.Size(); i++)
		{
            terrainChunk[i]->LoadChunk(this->resId, i);
		}
		
		// 释放已加载的资源
		ResourceManager::Instance()->DiscardManagedResource(managedMapTile.upcast<ManagedResource>());
		this->managedMapTile = 0;
	}
    else
    {
        int loaded = 0;
	    for (IndexT i = 0; i < terrainChunk.Size(); i++)
	    {
		    if (terrainChunk[i]->IsLoaded())
			    loaded++;
            else
            {
                // update chunk
                //if (terrainChunk[i]->Update())
                //    return;
            }
	    }
	    if (loaded == 256)
	    {
		    // send message to unload adt file
            Ptr<CloseMapTile> msg = CloseMapTile::Create();
            msg->SetURI(this->resId.Value());
            Interface::IOInterface::Instance()->Send(msg.upcast<Messaging::Message>());

            this->SetValid(true);
	    }
    }
}

//------------------------------------------------------------------------------
/**
	limitNum:限制加载的数量
*/
int 
TerrainEntity::Update(int limitNum)
{
	int num = 0;
	for (IndexT i = 0; i < this->terrainChunk.Size(); i++)
	{
		if (this->terrainChunk[i]->Update())
			num++;

		if (num == limitNum)
			break;
	}

	return num;
}

} // namespace Graphics
