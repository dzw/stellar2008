//------------------------------------------------------------------------------
//  simpleresourcemapper.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/simpleresourcemapper.h"
#include "resources/sharedresourceserver.h"

namespace Resources
{
ImplementClass(Resources::SimpleResourceMapper, 'SRSM', Resources::ResourceMapper);

using namespace Core;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
SimpleResourceMapper::SimpleResourceMapper() :
    resourceClass(0),
    resLoaderClass(0),
    managedResourceClass(0)
	#if NEBULA3_USEPLACEHOLDER
    ,placeholderResource(0)
	#endif
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SimpleResourceMapper::~SimpleResourceMapper()
{
    n_assert(!this->IsAttachedToResourceManager());
}

//------------------------------------------------------------------------------
/**
*/
const Rtti&
SimpleResourceMapper::GetResourceType() const
{
    n_assert2(0 != this->resourceClass, "SimpleResourceMapper::SetResourceType() method not called!");
    return *(this->resourceClass);
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleResourceMapper::OnAttachToResourceManager()
{
    n_assert2(0 != this->resourceClass, "SimpleResourceMapper::SetResourceClass() not called!");
    n_assert2(0 != this->managedResourceClass, "SimpleResourceMapper::SetManagedResourceClass() not called!");
    n_assert2(0 != this->resLoaderClass, "SimpleResourceMapper::SetResourceLoaderClass() not called!");
	#if NEBULA3_USEPLACEHOLDER
	n_assert2(this->placeholderResourceId.IsValid(), "SimpleResourceMapper::SetPlaceholderResourceId() not called!"); 
	#endif

    // call parent class
    ResourceMapper::OnAttachToResourceManager();
    n_assert(this->managedResources.IsEmpty());
    n_assert(this->pendingResources.IsEmpty());

	#if NEBULA3_USEPLACEHOLDER
    // load placeholder resource
    this->placeholderResource = (Resource*) this->resourceClass->Create();
    this->placeholderResource->SetResourceId(this->placeholderResourceId);
    this->placeholderResource->SetLoader((ResourceLoader*)this->resLoaderClass->Create());
    this->placeholderResource->SetAsyncEnabled(false);
    this->placeholderResource->Load();
    if (!this->placeholderResource->IsLoaded())
    {
        n_error("SimpleResourceMapper::OnAttachToServer(): could not not load placeholder resource '%s' of class '%s'!",
            this->placeholderResourceId.Value().AsCharPtr(), this->resourceClass->GetName().AsCharPtr());
    }
	#endif
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleResourceMapper::OnRemoveFromResourceManager()
{
    n_assert(this->IsAttachedToResourceManager());

    // discard all remaining resources

	Array<Ptr<ManagedResource>> manages = this->managedResources.GetContents();
	for (SizeT i = 0; i < manages.Size(); i++)
	{
		this->OnDiscardManagedResource(manages[i]);
	}
	manages.Clear();

	this->managedResources.Clear();
    n_assert(this->pendingResources.IsEmpty());

	#if NEBULA3_USEPLACEHOLDER
    // unload the placeholder resource
    this->placeholderResource->Unload();
    this->placeholderResource->SetLoader(0);
    this->placeholderResource = 0;
	#endif

    // finally call parent class
    ResourceMapper::OnRemoveFromResourceManager();
}

//------------------------------------------------------------------------------
/**
    This method is called by the ResourceManager when a new
    ManagedResource must be created. The resource manager will only call this
    method if this is the first request for the resource name.
*/
Ptr<ManagedResource>
SimpleResourceMapper::OnCreateManagedResource(const Rtti& resType, const ResourceId& resId)
{
    n_assert(0 != this->managedResourceClass);

	//this->managedResources.Find(resId);
	Ptr<ManagedResource> res;
	if (this->managedResources.Find(resId.Value(), res))
	{
		res->IncrClientCount();
		return res;
	}

    // ResourceManager should call this method only when 
    // a managed resource with the same resource id doesn't exist yet:
    //n_assert(!this->managedResources.Contains(resId.Value()));

    // for the same reason the resource shouldn't be pending already:
    //n_assert(!this->pendingResources.Contains(resId.Value()));

    // create a new managed resource, and set the placeholder resource on it
    Ptr<ManagedResource> managedResource = (ManagedResource*) this->managedResourceClass->Create();
    managedResource->IncrClientCount();
    managedResource->SetResourceId(resId);
    managedResource->SetResourceType(&this->GetResourceType());
	#if NEBULA3_USEPLACEHOLDER
    managedResource->SetPlaceholder(this->placeholderResource);
	#endif
    this->managedResources.Add(resId.Value(), managedResource);

    // check if the resource already exists as shared resource
    SharedResourceServer* sharedServ = SharedResourceServer::Instance();
    Ptr<Resource> resource = sharedServ->CreateSharedResource(resId, *(this->resourceClass), *(this->resLoaderClass));
    if (resource->IsLoaded())
    {
        // the resource is already loaded... (must have been somebody else...)
        managedResource->SetResource(resource);
    }
    else if (resource->IsPending())
    {
        // the resource is already pending... (must have been somebody else...)
        //this->pendingResources.Add(resId.Value(), resource);
		this->pendingResources.Append(resource);
    }
    else
    {
        // initiate an synchrnouse or asynchronous load (depending on setting)
        resource->SetAsyncEnabled(this->asyncEnabled);
        resource->Load();
        if (resource->IsLoaded() || resource->LoadFailed())
        {
            // synchronous loading succeeded (or failed)
            managedResource->SetResource(resource); 
        }
        else if (resource->IsPending())
        {
            // asynchronous load initiayed and pending
            //this->pendingResources.Add(resId.Value(), resource);
			this->pendingResources.Append(resource);
        }
    }
    return managedResource;
}

//------------------------------------------------------------------------------
/**
    This method will be called by the ResourceManager whenever a ManagedResource
    should be discarded. 
*/
void
SimpleResourceMapper::OnDiscardManagedResource(const Ptr<ManagedResource>& managedResource)
{
    n_assert(managedResource->GetClientCount() > 0);
    n_assert(this->managedResources.Contains(managedResource->GetResourceId().Value()));
    n_assert(managedResource->GetResourceType() == &this->GetResourceType());
    n_assert(managedResource->IsInstanceOf(*(this->managedResourceClass)));

    managedResource->DecrClientCount();
    if (managedResource->GetClientCount() == 0)
    {
        // unregister the shared resource
        SharedResourceServer::Instance()->UnregisterSharedResource(managedResource->GetResourceId());
        
        // drop pending resource
		for (IndexT i = 0; i < this->pendingResources.Size(); i++)
		{
			if (this->pendingResources[i] == managedResource->GetResource())
				this->pendingResources.EraseIndex(i);
		}
		/*IndexT index = this->pendingResources.BinarySearchIndex(managedResource->GetResource());
		if ( InvalidIndex != index)
        {
			this->pendingResources.EraseIndex(index);
        }*/

        // finally drop the managed resource itself
        this->managedResources.Erase(managedResource->GetResourceId().Value());

        // clear the contained resource
        managedResource->Clear();
    }
}

//------------------------------------------------------------------------------
/**
    This method will go through all ManagedResources and reset their
    render statistics. It will also check whether pending resources
    have finished loading, and update the associated managed resources 
    accordingly.
*/
void
SimpleResourceMapper::OnPrepare()
{
    // first reset render statistics
    /*IndexT resIndex;
    for (resIndex = 0; resIndex < this->managedResources.Size(); resIndex++)
    {
        this->managedResources.ValueAtIndex(resIndex)->ClearRenderStats();
    }*/

    // now check pending resources...
    IndexT pendingIndex, loadNum = 0;
    for (pendingIndex = 0; pendingIndex < this->pendingResources.Size();)
    {
        Ptr<Resource>& resource = this->pendingResources[pendingIndex];
        n_assert(resource->IsPending());

		if (loadNum > 1)
			break;

        // try load...
        resource->Load();
        if (resource->IsLoaded() || resource->LoadFailed())
        {
            loadNum++;
            // resource has finished loading, or failed to load
			if (resource->LoadFailed())
			{
			}
            this->managedResources[resource->GetResourceId().Value()]->SetResource(resource);            
            this->pendingResources.EraseIndex(pendingIndex);
        }
        else
        {
            // still pending...
            n_assert(resource->IsPending());
            pendingIndex++;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleResourceMapper::OnUpdate()
{
    // @todo: do we need to do anything here?
}

bool 
SimpleResourceMapper::HasManagedResource(const ResourceId& resId) const
{
	return managedResources.Contains(resId.Value());
}

const Ptr<ManagedResource>& 
SimpleResourceMapper::LookupManagedResource(const ResourceId& resId) const
{
	if (!managedResources.Contains(resId.Value()))
		n_assert(0);
	return managedResources[resId.Value()];
}

} // namespace Resources

