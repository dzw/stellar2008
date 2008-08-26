//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/thingentity.h"
#include "kok/model/managedthing.h"
#include "resources/resourcemanager.h"

namespace KOK
{
ImplementClass(KOK::ThingEntity, 'TGET', Graphics::ModelEntity);
ImplementSingleton(KOK::ThingEntity);

using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ThingEntity::ThingEntity()	
{
	ConstructSingleton;

	this->SetType(ModelType);
}

//------------------------------------------------------------------------------
/**
*/
ThingEntity::~ThingEntity()
{
	DestructSingleton;
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
    this->SetValid(false);
    this->managedModel = ResourceManager::Instance()->CreateManagedResource(Thing::RTTI, this->resId).downcast<ManagedThing>();
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
    ResourceManager::Instance()->DiscardManagedResource(this->managedModel.upcast<ManagedResource>());
    this->managedModel = 0;

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

void
ThingEntity::OnUpdate()
{
	ModelEntity::OnUpdate();
}

} // namespace Graphics
