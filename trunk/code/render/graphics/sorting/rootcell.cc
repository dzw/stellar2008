//------------------------------------------------------------------------------
//  groupcell.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/sorting/convexzonemanager.h"
#include "graphics/cameraentity.h"
#include "coregraphics/transformdevice.h"

namespace Graphics
{
ImplementClass(Graphics::RootCell, 'GPCL', Graphics::Cell);

using namespace Math;
using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
RootCell::RootCell()
{
}

//------------------------------------------------------------------------------
/**
*/
RootCell::~RootCell()
{
}

//------------------------------------------------------------------------------
/**
    Recursively update visibility links. This method is called by the
    top level method UpdateLinks(). 
    
    NOTE: This is the core visibility detection method and must be FAST.
*/
void
RootCell::RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity,
                         uint observedEntityTypeMask,
                         GraphicsEntity::LinkType linkType,
                         ClipStatus::Type clipStatus)
{
    n_assert(observerEntity.isvalid());

	if (this->walked)
		return;

	Ptr<PortalStage> pstage = this->stage.downcast<PortalStage>();
	Ptr<ConvexZoneManager> zoneManage = pstage->GetCurConvexZoneManager();
	// 当前正在室内
	if (zoneManage.isvalid())
	{
		Ptr<ConvexZone> zone = zoneManage->GetCurZone();
	}
	else
	{
		
	}
	for (SizeT i = 0; i < zoneManagers.Size(); i++)
	{
		
	}

	//Cell::RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, clipStatus);
}

//------------------------------------------------------------------------------
/**
*/
void 
RootCell::AttachConvexZoneManager(const Ptr<ConvexZoneManager>& zoneManager)
{
	n_assert(zoneManager.isvalid());
	this->zoneManagers.Append(zoneManager);
}

} // namespace Graphics
