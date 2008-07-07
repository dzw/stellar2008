#pragma once
#ifndef GRAPHICS_CONVEXZONE_H
#define GRAPHICS_CONVEXZONE_H
//------------------------------------------------------------------------------
/**
    @class Graphics::ConvexZone
  
	���ڳ�����ÿ��group

	��Ҫ����Portal�ü�

	ע�⣺
		1��ConvexZone�ĸ����ڹ�����������Zone��
		2����ӵ����е���Zone�����ռ����򣬶��ǰ�Portal��ü������Բ�����Attach����Zone����Zone��һ��

    (C) 2008 ctuo
*/
#include "graphics/cell.h"
#include "graphics/sorting/portal.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Cell;
class Portal;

class ConvexZone : public Cell
{
    DeclareClass(ConvexZone);
public:
    /// constructor
    ConvexZone();
    /// destructor
    virtual ~ConvexZone();
	///
	virtual void OnRemoveFromStage();
	/// add a child cell (only during setup phase)
	virtual void AttachChildCell(const Ptr<Cell>& cell);
	///
	void AttachPortal(const Ptr<Portal>& portal);
	///
	void RemovePortal(const Ptr<Portal>& portal);
	/// �Ƿ�Ϊ����������
	bool IsOutDoor();
	/// �����Ƿ���������
	void SetOutDoor(bool b);
	/// �����Ƿ񴩹�portal�����zone
	bool ComputePortalAcross(Math::vector& startPoint, Math::vector& endPoint);
	///
	virtual void OnRenderDebug(/*const Math::bbox& b*/);
	///
	virtual void UpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType);

private:
    friend class GraphicsEntity;
	friend class WMOZone;

	/// �ҵ���С��Χ�壬��cellֻ���ҵ���һ����Χ��ͽ�����
	virtual Ptr<Cell> FindEntityContainmentCell(const Ptr<GraphicsEntity>& entity);
	///
	Ptr<Cell> FindZone(const Math::bbox& entityBox);
    /// create links between visible entities
    virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);
	///
	bool ReducedFrustum(Math::matrix44& proj, const Ptr<Portal>& portal, const Ptr<GraphicsEntity>& observerEntity);
	///
	void UpdateZoneLinks(const Ptr<GraphicsEntity>& observerEntity, uint observedEntityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);
	/// �ж��Ƿ���Ҫ���µ�ǰλ��
	bool UpdateLocation(const Ptr<GraphicsEntity>& observerEntity);
	/// �ж��Ƿ񴩹�portal
	void CheckAcrossPortal();

	///
	Util::Array<Ptr<Portal>> portals;
	/// �Ƿ������ⳡ��(�����)
	bool isOutDoor;
	/// Ŀǰ�Ƿ��ڸ�group����
	static bool inGroup;
	/// ���ĸ�graoup����
	static Ptr<ConvexZone> curConvexZone;
	///
	static Math::matrix44 view;
	/// �����λ��
	static Math::vector viewPos;
	///
	static Math::vector startPoint, endPoint;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
ConvexZone::IsOutDoor()
{
	return this->isOutDoor;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ConvexZone::SetOutDoor(bool b)
{
	this->isOutDoor = b;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
