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
 1������boxѭ���жϵ�ǰ���ĸ�Zone�����������zone��������Ը�zone������portal�ж��Ƿ��д������еĻ�˵���ڸ�zone���棬����portal��׶�ü���
 2�������1����ⷢ��û�б�������û�д�͸��˵�������⣬����������zone����������ü������������portal�ڲü����ڣ�����portal��׶�ü���
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

		// �����⣬�����⴦��
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
