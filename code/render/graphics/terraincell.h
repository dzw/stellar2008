#pragma once
#ifndef GRAPHICS_TERRAINCELL_H
#define GRAPHICS_TERRAINCELL_H
//------------------------------------------------------------------------------
/**
    @class Graphics::StaticCell

	静态cell，地形和静态物件裁剪

    (C) 2008 cTuo
*/
#include "graphics/cell.h"
#include "graphics/graphicsentity.h"
#include "models/modelnode.h"

//------------------------------------------------------------------------------
namespace Graphics
{

class StaticCell : public Cell
{
    DeclareClass(TerrainCell);
public:
    /// constructor
    TerrainCell();
    /// destructor
    virtual ~TerrainCell();

	///
	virtual void OnRenderDebug(/*const bbox& b*/);

	void SetId(int id);
private:
    /// create links between visible entities
    virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);

	int id;
};

inline void 
TerrainCell::SetId(int id)
{
	this->id = id;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
