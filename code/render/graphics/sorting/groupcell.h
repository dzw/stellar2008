#pragma once
#ifndef GRAPHICS_GROUPCELL_H
#define GRAPHICS_GROUPCELL_H
//------------------------------------------------------------------------------
/**
    @class Graphics::GroupCell
  
	��֯wmo��ÿ��group

	��Ҫ����Portal�ü�

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
	/// �Ƿ���Կ������ⳡ��
	bool outDoor;
	/// Ŀǰ�Ƿ��ڸ�group����
	bool inGroup;
	 
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
