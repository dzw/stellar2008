//------------------------------------------------------------------------------
//  modelserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/worldserver.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "math/vector3.h"
#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "wow/world/streamworldloader.h"
#include "wow/world/terrain/maptile.h"
#include "wow/world/terrain/managedmaptile.h"
#include "wow/world/Terrain/managedChunk.h"

namespace WOW
{
ImplementClass(WOW::WorldServer, 'WODS', Core::RefCounted);
ImplementSingleton(WOW::WorldServer);

using namespace Core;
using namespace Util;
using namespace IO;
using namespace Attr;
using namespace Resources;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
WorldServer::WorldServer() :
    isOpen(false)
{
    ConstructSingleton;
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

	Ptr<SimpleResourceMapper> resMapper = SimpleResourceMapper::Create();
	resMapper->SetPlaceholderResourceId(ResourceId("mdl:system/placeholder.n2"));   // FIXME: n3!
	resMapper->SetResourceClass(World::RTTI);
	resMapper->SetResourceLoaderClass(StreamWorldLoader::RTTI);
	resMapper->SetManagedResourceClass(ManagedWorld::RTTI);
	ResourceManager::Instance()->AttachMapper(resMapper.upcast<ResourceMapper>());
	
	// MapTile resource
	resMapper = SimpleResourceMapper::Create();
	resMapper->SetPlaceholderResourceId(ResourceId("mdl:system/placeholder.n2"));   // FIXME: n3!
	resMapper->SetResourceClass(Models::MapTile::RTTI);
	resMapper->SetResourceLoaderClass(StreamWorldLoader::RTTI);
	resMapper->SetManagedResourceClass(Models::ManagedMapTile::RTTI);
	ResourceManager::Instance()->AttachMapper(resMapper.upcast<ResourceMapper>());

	// MapTile resource
	resMapper = SimpleResourceMapper::Create();
	resMapper->SetPlaceholderResourceId(ResourceId("mdl:system/placeholder.n2"));   // FIXME: n3!
	resMapper->SetResourceClass(Models::Chunk::RTTI);
	resMapper->SetResourceLoaderClass(StreamWorldLoader::RTTI);
	resMapper->SetManagedResourceClass(Models::ManagedChunk::RTTI);
	ResourceManager::Instance()->AttachMapper(resMapper.upcast<ResourceMapper>());

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
	this->isOpen = false;
}


//------------------------------------------------------------------------------
/**
*/
//bool
//WorldServer::SaveModel(const Ptr<Model>& model, const URI& uri, const Rtti& saverClass)
//{
//    n_error("IMPLEMENT ME!");
//    return false;
//}

//------------------------------------------------------------------------------
/**
*/
bool
WorldServer::HasManagedWorld(const ResourceId& resId) const
{
	return ResourceManager::Instance()->HasManagedResource(resId);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ManagedWorld>
WorldServer::LoadManagedWorld(const ResourceId& resId)
{
	return ResourceManager::Instance()->CreateManagedResource(World::RTTI, resId).downcast<ManagedWorld>();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ManagedWorld>&
WorldServer::LookupManagedWorld(const ResourceId& resId) const
{
	n_assert(this->HasManagedWorld(resId));
	return ResourceManager::Instance()->LookupManagedResource(resId).downcast<ManagedWorld>();
}

//------------------------------------------------------------------------------
/**
	使用LoadmanagedWMO创建的ManagedWorld资源必需手动调用DiscardManagedWMO释放，否则
	会出错，因为在ResourceManager里面释放资源后还需要将对象清空，这时如果不是创建
	的位置传入指针，就会出现针指被清0，从而导致出错。

	必需哪里Load就要哪里Discard！！！
*/
void
WorldServer::DiscardManagedWorld(const Ptr<ManagedWorld>& managedModel) const
{
	ResourceManager::Instance()->DiscardManagedResource(managedModel.upcast<ManagedResource>());
}

//------------------------------------------------------------------------------
/**
*/
//void 
//WorldServer::CreateWMOInstance(const Resources::ResourceId& resId, vector3& pos, vector3& dir, int doodadset)
//{
//	Ptr<World> wmo = LookupManagedWMO(resId)->GetModel();
//	wmo->CreateInstance(pos, dir, doodadset);
//}

} // namespace Models