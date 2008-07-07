#pragma once
#ifndef GRAPHICS_CONVEXZONEMANAGER_H
#define GRAPHICS_CONVEXZONEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Graphics::ConvexZoneManager
  
	һ�����ڳ���

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
	/// ����������ƶ���·��
	void ComputeLine(const Ptr<GraphicsEntity>& cameraEntity);

	Util::Array<Ptr<ConvexZone>> zones;
	/// ������������zone
	Util::Array<Ptr<ConvexZone>> outDoorZones;
	/// ��¼����������ε�λ��
	Math::vector startPoint, endPoint;
	/// ��ǰ���ĸ�����
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
