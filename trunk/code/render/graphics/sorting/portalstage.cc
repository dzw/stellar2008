//------------------------------------------------------------------------------
//  stage.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/sorting/portalstage.h"
#include "graphics/cameraentity.h"

namespace Graphics
{
ImplementClass(Graphics::PortalStage, 'PLRE', Graphics::Stage);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
PortalStage::PortalStage()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PortalStage::~PortalStage()
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
PortalStage::AttachEntity(const Ptr<GraphicsEntity>& entity)
{
    n_assert(!entity->IsActive());
    n_assert(!entity->IsAttachedToStage());
    n_assert(entity->GetType() < GraphicsEntity::NumTypes);

    this->entities.Append(entity);
    this->entitiesByType[entity->GetType()].Append(entity);
    entity->OnActivate();
    entity->OnAttachToStage(this);
    this->rootCell->InsertEntity(entity);
}

//------------------------------------------------------------------------------
/**
*/
void 
PortalStage::AttachEntity(const Ptr<GraphicsEntity>& entity, const Ptr<ConvexZone>& zone)
{
	n_assert(!entity->IsActive());
	n_assert(!entity->IsAttachedToStage());
	n_assert(entity->GetType() < GraphicsEntity::NumTypes);

	this->entities.Append(entity);
	this->entitiesByType[entity->GetType()].Append(entity);
	entity->OnActivate();
	entity->OnAttachToStage(this);
	zone->AttachEntity(entity);
}

//------------------------------------------------------------------------------
/**
*/
void 
PortalStage::UpdateCameraLinks(const Ptr<CameraEntity>& cameraEntity)
{
	matrix44 invview = matrix44::inverse(cameraEntity->GetViewTransform());
	endPoint = invview.row3();

	Stage::UpdateCameraLinks(cameraEntity);

	startPoint = endPoint;
}

} // namespace Graphics