#pragma once
#ifndef GRAPHICS_ROOTCELL_H
#define GRAPHICS_ROOTCELL_H
//------------------------------------------------------------------------------
/**
    @class Graphics::GroupCell
  
	组织wmo在每个group

	主要用于Portal裁剪

    (C) 2008 ctuo
*/
#include "graphics/cell.h"
#include "graphics/sorting/convexzonemanager.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Cell;


class RootCell : public Cell
{
    DeclareClass(RootCell);
public:
    /// constructor
    RootCell();
    /// destructor
    virtual ~RootCell();
	///
	void AttachConvexZoneManager(const Ptr<ConvexZoneManager>& zoneManager);
	
private:

    /// create links between visible entities
    virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);

	Util::Array<Ptr<ConvexZoneManager>> zoneManagers;
	
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
