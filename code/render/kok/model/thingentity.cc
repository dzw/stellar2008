//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/thingentity.h"
#include "kok/model/managedthing.h"
#include "resources/resourcemanager.h"
#include "models/visresolver.h"

#include "models/modelnodeinstance.h"
#include "kok/model/thingnodeinstance.h"
#include "kok/model/thingnode.h"
#include "kok/model/thinginstance.h"

namespace KOK
{
ImplementClass(KOK::ThingEntity, 'TGET', Graphics::ModelEntity);

using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ThingEntity::ThingEntity():
	repeatId(0)
{

	this->SetType(ModelType);
}

//------------------------------------------------------------------------------
/**
*/
ThingEntity::~ThingEntity()
{
}

//------------------------------------------------------------------------------
/**
*/
void
ThingEntity::OnActivate()
{
    n_assert(!this->IsActive());
    n_assert(this->resId.IsValid());
    GraphicsEntity::OnActivate();
    
    // note: we will remain invalid until at least our model has loaded
	this->managedModel = ResourceManager::Instance()->CreateManagedResource(Thing::RTTI, this->resId).downcast<ManagedThing>();
    this->SetValid(false);
}

//------------------------------------------------------------------------------
/**
*/
void
ThingEntity::OnDeactivate()
{
    n_assert(this->IsActive());
    n_assert(this->managedModel.isvalid());

	// discard our model instance (if exists)
	if (this->modelInstance.isvalid())
	{
		this->modelInstance->Discard();
		this->modelInstance = 0;
	}

    // discard our managed model
	if (this->managedModel.isvalid())
	{
	    ResourceManager::Instance()->DiscardManagedResource(this->managedModel.upcast<ManagedResource>());
		this->managedModel = 0;
	}

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

void
ThingEntity::OnUpdate()
{
	//ModelEntity::OnUpdate();

	if (!this->modelInstance.isvalid())
	{
		if (this->managedModel->GetState() == Resource::Loaded)
		{
			const Ptr<Thing> thing = this->managedModel->GetModel().downcast<Thing>();
			n_assert(thing->IsLoaded());
			this->SetLocalBoundingBox(thing->GetBoundingBox());
			this->modelInstance = thing->CreateInstance();
			this->modelInstance->SetTransform(this->GetTransform());
			this->modelInstance->SetModelEntity(this);
			//this->modelInstance->SetAllNodeInstancesVisible(this->setModelNodesVisible);
			
			const Ptr<ThingInstance>& t =this->modelInstance.downcast<ThingInstance>();
			t->SetVisiableRepeat(this->repeatId);
			for (SizeT i = 0; i < this->texs.Size(); i++)
				t->SetTexture(this->texs[i].nodeId, this->texs[i].texId);

			this->SetValid(true);
		}
		else
		{
			// @todo: check for load failed!
		}
	}
	else
	{
		this->modelInstance->SetTime(this->GetTime());
		this->modelInstance->Update();
	}
	
	GraphicsEntity::OnUpdate();
}

void 
ThingEntity::AttachVisibleInstance()
{
	// 如果没创建资源，就在这创建
	//if (!this->managedModel.isvalid())
	//	this->managedModel = ResourceManager::Instance()->CreateManagedResource(Thing::RTTI, this->resId).downcast<ManagedThing>();

	if (this->modelInstance.isvalid())
		VisResolver::Instance()->AttachVisibleModelInstance(this->modelInstance);
}

//------------------------------------------------------------------------------
/**
	设置显示的纹理，每个地物可能有很多套，在这里设置显示哪套纹理
*/
void 
ThingEntity::SetTextureId(int nodeId, int texId)
{
	NodeTextureParm tex;
	tex.nodeId = nodeId;
	tex.texId = texId;

	texs.Append(tex);
}

void 
ThingEntity::SetRepeatId(int id)
{
	this->repeatId = id;
}

} // namespace Graphics
