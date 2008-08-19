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

class TerrainCell : public Cell
{
    DeclareClass(TerrainCell);
public:
    /// constructor
    TerrainCell();
    /// destructor
    virtual ~TerrainCell();

	///
	virtual void OnRenderDebug(/*const bbox& b*/);

	void SetDistrict(const Ptr<DistrictNodeInstance>& dist);
private:
    /// create links between visible entities
    virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);

	Ptr<DistrictNodeInstance> dist;
};

inline void 
TerrainCell::SetDistrict(const Ptr<DistrictNodeInstance>& dist)
{
	this->dist = dist;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
