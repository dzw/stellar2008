//------------------------------------------------------------------------------
//  stage.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/worldstage.h"
#include "graphics/cell.h"

namespace Graphics
{
ImplementClass(Graphics::WorldStage, 'GWSG', Graphics::Stage);

using namespace Util;
using namespace Graphics;
using namespace KOK;

//------------------------------------------------------------------------------
/**
*/
WorldStage::WorldStage() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WorldStage::~WorldStage()
{
    
}

//------------------------------------------------------------------------------
/**
    Add an entity to the stage. The method OnAttachToStage() will be
    invoked on the entity, and the entity will be inserted into 
    the cell hierarchy of the stage (which in turn call OnAttachToCell()
    on the entity).
*/
void
WorldStage::AttachEntity(const Ptr<GraphicsEntity>& entity)
{
    n_assert(!entity->IsActive());
    n_assert(!entity->IsAttachedToStage());
    n_assert(entity->GetType() < GraphicsEntity::NumTypes);

    if (entity->GetUpdateType() == GraphicsEntity::NoUpdate)
	{
		this->staticEntities.Append(entity);
		this->entitiesByType[entity->GetType()].Append(entity);

		entity->OnActivate();
		entity->OnAttachToStage(this);
		this->rootCell->InsertEntity(entity);
	}
	else
	{
		Stage::AttachEntity(entity);
	}
}

//------------------------------------------------------------------------------
/**
    Remove an entity from the stage. This will remove the entity from
    the cell hierarchy of the stage (which invoked OnRemoveFromCell()
    on the entity), and then the method OnRemoveFromStage() will
    be called on the entity.
*/
void
WorldStage::RemoveEntity(const Ptr<GraphicsEntity>& entity)
{
    n_assert(entity->IsActive());
    n_assert(entity->IsAttachedToStage());
    n_assert(entity->GetStage().get() == this);
    n_assert(entity->IsAttachedToCell());
    n_assert(entity->GetType() < GraphicsEntity::NumTypes);

	if (entity->GetUpdateType() == GraphicsEntity::NoUpdate)
	{
		// first remove entity from its cell, stage and deactivate it
		entity->GetCell()->RemoveEntity(entity);
		entity->OnRemoveFromStage();
		entity->OnDeactivate();

		IndexT entitiesIndex = this->staticEntities.FindIndex(entity);
		n_assert(InvalidIndex != entitiesIndex);
		this->entities.EraseIndex(entitiesIndex);

		IndexT entitiesByTypeIndex = this->entitiesByType[entity->GetType()].FindIndex(entity);
		n_assert(InvalidIndex != entitiesByTypeIndex);
		this->entitiesByType[entity->GetType()].EraseIndex(entitiesByTypeIndex);
	}
	else
	{
		Stage::RemoveEntity(entity);
	}
}

void 
WorldStage::UpdateCameraLinks(const Ptr<CameraEntity>& cameraEntity)
{
	if (this->terrInstance.isvalid())
		this->terrInstance->ClearRenderList();
	
	Stage::UpdateCameraLinks(cameraEntity);
}

void 
WorldStage::AppendDist(const Ptr<KOK::DistrictNodeInstance>& dist)
{
	this->terrInstance->AppendRenderDist(dist);
}

} // namespace Graphics