#pragma once
#ifndef GRAPHICS_PORTALSTAGE_H
#define GRAPHICS_PORTALSTAGE_H
//------------------------------------------------------------------------------
/**
    @class Graphics::Stage

    (C) 2008 ctuo
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "graphics/graphicsentity.h"
#include "graphics/stage.h"
#include "graphics/sorting/convexzone.h"
#include "graphics/sorting/convexzonemanager.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class StageBuilder;
class Cell;
class CameraEntity;

class PortalStage : public Stage
{
    DeclareClass(PortalStage);
public:
    /// constructor
    PortalStage();
    /// destructor
    virtual ~PortalStage();

    /// attach an entity to the stage
    virtual void AttachEntity(const Ptr<GraphicsEntity>& graphicsEntity);
	/// ��ӵ�ָ����zone
	void AttachEntity(const Ptr<GraphicsEntity>& graphicsEntity, const Ptr<ConvexZone>& zone);
	/// ��ȡ�������λ��
	void UpdateCameraLinks(const Ptr<CameraEntity>& cameraEntity);
	/// ��ȡ������ƶ���·��
	void GetMoveLine(Math::vector& sp, Math::vector& ep);
	///
	void SetCurConvexZoneManager(const Ptr<ConvexZoneManager>& zoneManager);
	///
	const Ptr<ConvexZoneManager>& GetCurConvexZoneManager();
protected:

	/// ��¼����������ε�λ��
	Math::vector startPoint, endPoint;
	/// ָ��ǰ���ڵ�����
	Ptr<ConvexZoneManager> curZoneManager;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
PortalStage::GetMoveLine(Math::vector& sp, Math::vector& ep)
{
	sp = this->startPoint;
	ep = this->endPoint;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PortalStage::SetCurConvexZoneManager(const Ptr<ConvexZoneManager>& zoneManager)
{
	n_assert(zoneManager.isvalid());
	this->curZoneManager = zoneManager;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ConvexZoneManager>& 
PortalStage::GetCurConvexZoneManager()
{
	return this->curZoneManager;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
