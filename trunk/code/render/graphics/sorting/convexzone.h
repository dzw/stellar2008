#pragma once
#ifndef GRAPHICS_CONVEXZONE_H
#define GRAPHICS_CONVEXZONE_H
//------------------------------------------------------------------------------
/**
    @class Graphics::ConvexZone
  
	室内场景的每个group

	主要用于Portal裁剪

	注意：
		1、ConvexZone的根用于管理所有其它Zone。
		2、添加到根中的子Zone不按空间排序，而是按Portal面裁剪，所以不管是Attach到根Zone或子Zone都一样

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
	/// 是否为与室外相连
	bool IsOutDoor();
	/// 设置是否室外相连
	void SetOutDoor(bool b);
	/// 计算是否穿过portal进入该zone
	bool ComputePortalAcross(Math::vector& startPoint, Math::vector& endPoint);
	///
	virtual void OnRenderDebug(/*const Math::bbox& b*/);
	///
	virtual void UpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType);

private:
    friend class GraphicsEntity;
	friend class WMOZone;

	/// 找到最小包围体，而cell只是找到第一个包围体就结束。
	virtual Ptr<Cell> FindEntityContainmentCell(const Ptr<GraphicsEntity>& entity);
	///
	Ptr<Cell> FindZone(const Math::bbox& entityBox);
    /// create links between visible entities
    virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);
	///
	bool ReducedFrustum(Math::matrix44& proj, const Ptr<Portal>& portal, const Ptr<GraphicsEntity>& observerEntity);
	///
	void UpdateZoneLinks(const Ptr<GraphicsEntity>& observerEntity, uint observedEntityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);
	/// 判断是否需要更新当前位置
	bool UpdateLocation(const Ptr<GraphicsEntity>& observerEntity);
	/// 判断是否穿过portal
	void CheckAcrossPortal();

	///
	Util::Array<Ptr<Portal>> portals;
	/// 是否是室外场景(外层框架)
	bool isOutDoor;
	/// 目前是否在该group里面
	static bool inGroup;
	/// 在哪个graoup里面
	static Ptr<ConvexZone> curConvexZone;
	///
	static Math::matrix44 view;
	/// 摄像机位置
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
