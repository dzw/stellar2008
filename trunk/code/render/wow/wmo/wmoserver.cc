//------------------------------------------------------------------------------
//  modelserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/wmo/wmoserver.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "math/vector3.h"
#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "wow/wmo/streamwmoloader.h"


namespace WOW
{
ImplementClass(WOW::WMOServer, 'WMOS', Core::RefCounted);
ImplementSingleton(WOW::WMOServer);

using namespace Core;
using namespace Util;
using namespace IO;
using namespace Attr;
using namespace Resources;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
WMOServer::WMOServer() :
    isOpen(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
WMOServer::~WMOServer()
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
WMOServer::Open()
{
    n_assert(!this->IsOpen())
    this->isOpen = true;

	Ptr<SimpleResourceMapper> resMapper = SimpleResourceMapper::Create();
	resMapper->SetPlaceholderResourceId(ResourceId("mdl:system/placeholder.n2"));   // FIXME: n3!
	resMapper->SetResourceClass(WMO::RTTI);
	resMapper->SetResourceLoaderClass(StreamWMOLoader::RTTI);
	resMapper->SetManagedResourceClass(ManagedWMO::RTTI);
	ResourceManager::Instance()->AttachMapper(resMapper.upcast<ResourceMapper>());
}

//------------------------------------------------------------------------------
/**
*/
void
WMOServer::Close()
{
    n_assert(this->IsOpen());

	// detach our model resource mapper
	ResourceManager::Instance()->RemoveMapper(WMO::RTTI);
	this->isOpen = false;
}


//------------------------------------------------------------------------------
/**
*/
//bool
//WMOServer::SaveModel(const Ptr<Model>& model, const URI& uri, const Rtti& saverClass)
//{
//    n_error("IMPLEMENT ME!");
//    return false;
//}

//------------------------------------------------------------------------------
/**
*/
bool
WMOServer::HasManagedWMO(const ResourceId& resId) const
{
	return ResourceManager::Instance()->HasManagedResource(resId);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ManagedWMO>
WMOServer::LoadManagedWMO(const ResourceId& resId)
{
	return ResourceManager::Instance()->CreateManagedResource(WMO::RTTI, resId).downcast<ManagedWMO>();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ManagedWMO>&
WMOServer::LookupManagedWMO(const ResourceId& resId) const
{
	n_assert(this->HasManagedWMO(resId));
	return ResourceManager::Instance()->LookupManagedResource(resId).downcast<ManagedWMO>();
}

//------------------------------------------------------------------------------
/**
	使用LoadmanagedWMO创建的ManagedWMO资源必需手动调用DiscardManagedWMO释放，否则
	会出错，因为在ResourceManager里面释放资源后还需要将对象清空，这时如果不是创建
	的位置传入指针，就会出现针指被清0，从而导致出错。

	必需哪里Load就要哪里Discard！！！
*/
void
WMOServer::DiscardManagedWMO(const Ptr<ManagedWMO>& managedModel) const
{
	ResourceManager::Instance()->DiscardManagedResource(managedModel.upcast<ManagedResource>());
}

//------------------------------------------------------------------------------
/**
*/
void 
WMOServer::CreateWMOInstance(const Resources::ResourceId& resId, vector& pos, vector& dir, int doodadset)
{
	Ptr<ManagedWMO> mWmo = LookupManagedWMO(resId);
	Ptr<WMO> wmo = mWmo->GetModel();
	wmo->CreateInstance(pos, dir, doodadset);
}

} // namespace Models