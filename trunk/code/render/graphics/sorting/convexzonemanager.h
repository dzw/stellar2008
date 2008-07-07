#pragma once
#ifndef GRAPHICS_CONVEXZONEMANAGER_H
#define GRAPHICS_CONVEXZONEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Graphics::ConvexZoneManager
  
	一个室内场景

    (C) 2008 ctuo
*/
#include "graphics/cell.h"
#include "graphics/sorting/convexzone.h"
#include "graphics/graphicsentity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Cell;

class ConvexZoneManager : public Cell
{
    DeclareClass(ConvexZoneManager);
public:
    /// constructor
    ConvexZoneManager();
    /// destructor
    virtual ~ConvexZoneManager();
	///
	void AttachZone(const Ptr<ConvexZone>& zone);
	///
	void RemoveZone(const Ptr<ConvexZone>& zone);
	///
	//void SetCurZone(const Ptr<ConvexZone>& zone);
	///
	//const Ptr<ConvexZone>& GetCurZone()const;
	/// recursively create visibility links between observers and observed entities
	//virtual void UpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType);
	/// create links between visible entities
	virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);
private:
	/// 计算摄像机移动的路线
	void ComputeLine(const Ptr<GraphicsEntity>& cameraEntity);

	Util::Array<Ptr<ConvexZone>> zones;
	/// 与室外相连的zone
	Util::Array<Ptr<ConvexZone>> outDoorZones;
	/// 记录摄像机近两次的位置
	Math::vector startPoint, endPoint;
	/// 当前在哪个室内
	//Ptr<ConvexZone> curConvexZone;
};

//------------------------------------------------------------------------------
/**
*/
//inline void 
//ConvexZoneManager::SetZone(const Ptr<ConvexZone>& zone)
//{
//	n_assert(zone.isvalid());
//	this->curConvexZone = zone;
//}

//------------------------------------------------------------------------------
/**
*/
//inline const Ptr<ConvexZone>& 
//ConvexZoneManager::GetZone()const
//{
//	return this->curConvexZone;
//}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
