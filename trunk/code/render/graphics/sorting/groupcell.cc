//------------------------------------------------------------------------------
//  groupcell.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/sorting/groupcell.h"
#include "graphics/cameraentity.h"
#include "coregraphics/transformdevice.h"

namespace Graphics
{
ImplementClass(Graphics::GroupCell, 'GPCL', Graphics::Cell);

using namespace Math;
using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
GroupCell::GroupCell() :
	outDoor(false),
	inGroup(false)
{
}

//------------------------------------------------------------------------------
/**
*/
GroupCell::~GroupCell()
{
}

//------------------------------------------------------------------------------
/**
    Recursively update visibility links. This method is called by the
    top level method UpdateLinks(). 
    
    NOTE: This is the core visibility detection method and must be FAST.
*/
void
GroupCell::RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity,
                         uint observedEntityTypeMask,
                         GraphicsEntity::LinkType linkType,
                         ClipStatus::Type clipStatus)
{
    n_assert(observerEntity.isvalid());

	if (this->walked)
		return;

	for (SizeT i = 0; i < portals.Size(); i++)
	{
		// 这里要加个角度判断，如果与portal面平行，就不用处理portal
		if (observerEntity->ComputeClipStatus(portals[i]->GetBoundBox()) != ClipStatus::Outside)
		{
			if (portals[i]->IsOpen())
				continue;
			portals[i]->SetOpen(true);

			matrix44 oldProj, newProj;
			oldProj = observerEntity.downcast<CameraEntity>()->GetProjTransform();
			ReducedFrustum(newProj, portals[i], observerEntity);
			observerEntity.downcast<CameraEntity>()->SetProjTransform(newProj);

			//portals[i]->GetOtherGroupCell(this)->UpdateLinks(observerEntity, observedEntityTypeMask, linkType);
			Ptr<GroupCell> gc = portals[i]->GetOtherGroupCell(this);
			if (gc.isvalid())
				gc->RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, clipStatus);

			observerEntity.downcast<CameraEntity>()->SetProjTransform(oldProj);
			portals[i]->SetOpen(false);
		}
	}

	Cell::RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, clipStatus);
}

//------------------------------------------------------------------------------
/**
*/
void 
GroupCell::ReducedFrustum(matrix44& proj, const Ptr<Portal>& portal, const Ptr<GraphicsEntity>& observerEntity)
{
	float l, t, r, b, zn, zf;
	l = -portal->GetWidth()  / 2.0f;
	r =  portal->GetWidth()  / 2.0f;
	t = -portal->GetHeight() / 2.0f;
	b =  portal->GetHeight() / 2.0f;

	if (observerEntity->IsInstanceOf(CameraEntity::RTTI))
		zf = observerEntity.downcast<CameraEntity>()->GetFarClipPlane();
	else
		zf = 1000.0f;

	TransformDevice* transDev = TransformDevice::Instance();
	matrix44 invview = matrix44::inverse(transDev->GetViewTransform());
	vector viewPos = invview.row3();

	//zn = (portal->GetCenter() - viewPos).z();
	plane pp = portal->GetPlane();
	plane np = plane::normalize(pp);
	zn = np.dot(portal->GetCenter() - viewPos);

	proj = matrix44::perspoffcenterrh(l, r, b, t, zn, zf);
}

//------------------------------------------------------------------------------
/**
*/
void 
GroupCell::AttachPortal(const Ptr<Portal>& portal)
{
	portals.Append(portal);
}

//------------------------------------------------------------------------------
/**
*/
void 
GroupCell::RemovePortal(const Ptr<Portal>& portal)
{
	Array<Ptr<Portal>>::Iterator iter = portals.Find(portal);
	if (iter != portals.End())
	{
		n_delete(portal);
		portals.Erase(iter);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
GroupCell::AttachEntity(const Ptr<GraphicsEntity>& entity)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
GroupCell::InsertEntity(const Ptr<GraphicsEntity>& entity)
{
	// 不做处理，使用AttachEntity手动指定。
}

} // namespace Graphics
