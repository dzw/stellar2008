//------------------------------------------------------------------------------
//  groupcell.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/sorting/convexzonemanager.h"
#include "graphics/cameraentity.h"
//#include "graphics/sorting/portalstage.h"

namespace Graphics
{
ImplementClass(Graphics::ConvexZoneManager, 'CZMR', Graphics::Cell);

using namespace Math;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ConvexZoneManager::ConvexZoneManager() 
{
}

//------------------------------------------------------------------------------
/**
*/
ConvexZoneManager::~ConvexZoneManager()
{
}

//------------------------------------------------------------------------------
/**
 1、根据box循环判断当前被哪个Zone包含，如果有zone包含，针对该zone的所有portal判断是否有穿过，有的话说明在该zone里面，进行portal视锥裁剪。
 2、如果第1步检测发现没有被包含或没有穿透，说明在室外，将所有室外zone进行摄像机裁剪，如果发现有portal在裁剪体内，进行portal视锥裁剪。
*/
//void
//ConvexZoneManager::UpdateLinks(const Ptr<GraphicsEntity>& observerEntity, 
//							   uint entityTypeMask, 
//							   GraphicsEntity::LinkType linkType)
void 
ConvexZoneManager::RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, 
									  uint entityTypeMask, 
									  GraphicsEntity::LinkType linkType, 
									  Math::ClipStatus::Type clipStatus)
{
	if (linkType != GraphicsEntity::CameraLink)
	{
		for (SizeT i = 0; i < this->zones.Size(); i++)
			this->zones[i]->UpdateLinks(observerEntity, entityTypeMask, linkType);
	}
	else
	{
		ComputeLine(observerEntity);
		for (SizeT i = 0; i < this->zones.Size(); i++)
		{
			if (this->zones[i]->GetBoundingBox().contains(endPoint))
			{
				if (this->zones[i]->ComputePortalAcross(startPoint, endPoint))
				{
					this->zones[i]->UpdateLinks(observerEntity, entityTypeMask, linkType);
					return;
				}
			}
		}

		// 在室外，做室外处理
		for (SizeT i = 0; i < this->outDoorZones.Size(); i++)
		{
			this->outDoorZones[i]->UpdateLinks(observerEntity, entityTypeMask, linkType);
		}
	}
	//Cell::RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, ClipStatus::Invalid);
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexZoneManager::AttachZone(const Ptr<ConvexZone>& zone)
{
	n_assert(zone.isvalid());
	this->zones.Append(zone);

	if (zone->IsOutDoor())
		this->outDoorZones.Append(zone);
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexZoneManager::RemoveZone(const Ptr<ConvexZone>& zone)
{
	IndexT ind = this->zones.FindIndex(zone);
	if (ind != InvalidIndex)
		this->zones.EraseIndex(ind);
	
	ind = this->outDoorZones.FindIndex(zone);
	if (ind != InvalidIndex)
		this->outDoorZones.EraseIndex(ind);
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexZoneManager::ComputeLine(const Ptr<GraphicsEntity>& cameraEntity)
{
	startPoint = endPoint;
	
	matrix44 invview = matrix44::inverse(cameraEntity.downcast<CameraEntity>()->GetViewTransform());
	endPoint = invview.row3();
}

} // namespace Graphics
