#pragma once
#ifndef GRAPHICS_GROUPCELL_H
#define GRAPHICS_GROUPCELL_H
//------------------------------------------------------------------------------
/**
    @class Graphics::GroupCell
  
	组织wmo在每个group

	主要用于Portal裁剪

    (C) 2008 ctuo
*/
#include "graphics/cell.h"
#include "graphics/sorting/portal.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Cell;
class Portal;

class GroupCell : public Cell
{
    DeclareClass(GroupCell);
public:
    /// constructor
    GroupCell();
    /// destructor
    virtual ~GroupCell();
	///
	void AttachPortal(const Ptr<Portal>& portal);
	///
	void RemovePortal(const Ptr<Portal>& portal);
	/// attach a graphics entity to this Cell
	virtual void AttachEntity(const Ptr<GraphicsEntity>& entity);
	/// insert an entity into the Cell hierarchy
	virtual void InsertEntity(const Ptr<GraphicsEntity>& entity);
private:
    friend class GraphicsEntity;

    /// create links between visible entities
    virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);
	///
	void ReducedFrustum(Math::matrix44& proj, const Ptr<Portal>& portal, const Ptr<GraphicsEntity>& observerEntity);

	Util::Array<Ptr<Portal>> portals;
	/// 是否可以看到室外场景
	bool outDoor;
	/// 目前是否在该group里面
	bool inGroup;
	 
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
