//------------------------------------------------------------------------------
//  modelserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terrainserver.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "math/vector3.h"
#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/stage.h"
#include "kokterrain/terrain.h"
#include "kokterrain/managedterrain.h"
#include "kokterrain/load/streamterrainloader.h"

namespace KOK
{
ImplementClass(KOK::TerrainServer, 'TNSR', Core::RefCounted);
ImplementSingleton(KOK::TerrainServer);

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
TerrainServer::TerrainServer():
	isOpen(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
TerrainServer::~TerrainServer()
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
TerrainServer::Open()
{
	n_assert(!this->IsOpen());
	this->isOpen = true;

	// setup resource mapper for meshes
	Ptr<SimpleResourceMapper> meshMapper = SimpleResourceMapper::Create();
	//meshMapper->SetPlaceholderResourceId(ResourceId("msh:system/placeholder_s_0.nvx2"));
	meshMapper->SetResourceClass(Terrain::RTTI);
	meshMapper->SetResourceLoaderClass(StreamTerrainLoader::RTTI);
	meshMapper->SetManagedResourceClass(ManagedTerrain::RTTI);
	ResourceManager::Instance()->AttachMapper(meshMapper.upcast<ResourceMapper>());
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainServer::Close()
{
    n_assert(this->IsOpen());

	this->isOpen = false;
}

void 
TerrainServer::LoadTerrain(const Resources::ResourceId resId)
{
	this->resId = resId;
	this->terrain = TerrainEntity::Create();
	this->terrain->SetResourceId(resId);
	GraphicsServer::Instance()->GetDefaultView()->GetStage()->AttachEntity(this->terrain.upcast<GraphicsEntity>());
}

} // namespace Models