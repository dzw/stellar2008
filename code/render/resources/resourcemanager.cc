//------------------------------------------------------------------------------
//  resourcemanager.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resourcemanager.h"
#include "resources/resourcemapper.h"
#include "resources/managedresource.h"

namespace Resources
{
ImplementClass(Resources::ResourceManager, 'RMGR', Core::RefCounted);
ImplementSingleton(Resources::ResourceManager);

using namespace Core;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ResourceManager::ResourceManager() :
    isOpen(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ResourceManager::~ResourceManager()
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
ResourceManager::Open()
{
    n_assert(!this->IsOpen());
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::Close()
{
    n_assert(this->IsOpen());
    this->RemoveAllMappers();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::AttachMapper(const Ptr<ResourceMapper>& mapper)
{
    n_assert(this->IsOpen());
    n_assert(mapper.isvalid());
    n_assert(!this->mappers.Contains(&(mapper->GetResourceType())));
    this->mappers.Add(&(mapper->GetResourceType()), mapper);
    mapper->OnAttachToResourceManager();
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::RemoveMapper(const Core::Rtti& resType)
{
    n_assert(this->IsOpen());
    n_assert(this->HasMapper(resType));
    this->mappers[&resType]->OnRemoveFromResourceManager();
    this->mappers.Erase(&resType);
}
   
//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::RemoveAllMappers()
{
    n_assert(this->IsOpen());
    while (this->mappers.Size() > 0)
    {
        this->mappers.ValueAtIndex(0)->OnRemoveFromResourceManager();
        this->mappers.EraseAtIndex(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceManager::HasMapper(const Rtti& resType) const
{
    n_assert(this->IsOpen());
    return this->mappers.Contains(&resType);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ResourceMapper>&
ResourceManager::GetMapperByResourceType(const Rtti& resType) const
{
    n_assert(this->IsOpen());
    n_assert(this->HasMapper(resType));
    return this->mappers[&resType];
}

//------------------------------------------------------------------------------
/**
    This method must be called per-frame before rendering begins. This will 
    call the OnPrepare() method on all attached resource mappers, which will 
    at least reset the render statistics in the managed resource.
*/
void
ResourceManager::Prepare()
{
    n_assert(this->IsOpen());
    IndexT i;
    for (i = 0; i < this->mappers.Size(); i++)
    {
        this->mappers.ValueAtIndex(i)->OnPrepare();
    }
}

//------------------------------------------------------------------------------
/**
    This method must be called by the application after render statistics 
    have been gathered and before the actual rendering. The ResourceManager
    will call the OnUpdate() method on all attached resource mappers. This is 
    the place where the actual resource management will happen. 
*/
void
ResourceManager::Update()
{
    n_assert(this->IsOpen());
    IndexT i;
    for (i = 0; i < this->mappers.Size(); i++)
    {
        this->mappers.ValueAtIndex(i)->OnUpdate();
    }
}

//------------------------------------------------------------------------------
/**
    Create a shared ResourceManager object. If a managed resource with the same
    resource name already exists, its client count will be incremented
    and it will be returned. Otherwise the right ResourceMapper will
    be asked to create a new managed resource.
*/
Ptr<ManagedResource>
ResourceManager::CreateManagedResource(const Core::Rtti& resType, const ResourceId& resId)
{
    n_assert(this->IsOpen());
	n_assert(this->HasMapper(resType));
	return this->mappers[&resType]->OnCreateManagedResource(resType, resId);

    // check if managed resource already exists
    if (this->managedResources.Contains(resId.Value()))
    {
        // yes exists, increment client count and return existing managed resource
        const Ptr<ManagedResource>& managedResource = this->managedResources[resId.Value()];
        n_assert(managedResource.isvalid());
        n_assert(&resType == managedResource->GetResourceType());
        managedResource->IncrClientCount();
        return managedResource;
    }
    else
    {
        // managed resource doesn't exist yet, ask the right resource mapper to create a new one
        n_assert(this->HasMapper(resType));
        Ptr<ManagedResource> managedResource = this->mappers[&resType]->OnCreateManagedResource(resType, resId);
        n_assert(managedResource.isvalid());
        this->managedResources.Add(resId.Value(), managedResource);
        return managedResource;
    }
}

//------------------------------------------------------------------------------
/**
    Discard a shared ManagedResource object. This will decrement the
    client count. If the client count reaches zero, the ManagedResource
    object will be released as well.
*/
void
ResourceManager::DiscardManagedResource(const Ptr<ManagedResource>& managedResource)
{
    n_assert(this->IsOpen());
    //n_assert(this->managedResources.Contains(managedResource->GetResourceId().Value()));
    this->mappers[managedResource->GetResourceType()]->OnDiscardManagedResource(managedResource);
    /*if (managedResource->GetClientCount() == 0)
    {
        this->managedResources.Erase(managedResource->GetResourceId().Value());
    }*/
}

//------------------------------------------------------------------------------
/**
	这个函数尽量少用，速度比较慢。
*/
bool
ResourceManager::HasManagedResource(const ResourceId& resId) const
{
    n_assert(this->IsOpen());
	IndexT i;
	for (i = 0; i < this->mappers.Size(); i++)
	{
		if (this->mappers.ValueAtIndex(i)->HasManagedResource(resId))
			return true;
	}
	return false;

    return this->managedResources.Contains(resId.Value());
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ManagedResource>&
ResourceManager::LookupManagedResource(const ResourceId& resId) const
{
    n_assert(this->IsOpen());

	IndexT i;
	for (i = 0; i < this->mappers.Size(); i++)
	{
		const Ptr<ManagedResource>& res = this->mappers.ValueAtIndex(i)->LookupManagedResource(resId);
		if (res.isvalid())
			return res;
	}
	return 0;

    return this->managedResources[resId.Value()];
}

} // namespace Resources