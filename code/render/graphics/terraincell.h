#pragma once
#ifndef GRAPHICS_TERRAINCELL_H
#define GRAPHICS_TERRAINCELL_H
//------------------------------------------------------------------------------
/**
    @class Graphics::TerrainCell

    (C) 2008 cTuo
*/
#include "graphics/cell.h"
#include "graphics/graphicsentity.h"
#include "kokterrain/districtnodeinstance.h"
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

	void SetDistrict(const Ptr<KOK::DistrictNodeInstance>& d);
private:
    /// create links between visible entities
    virtual void RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint entityTypeMask, GraphicsEntity::LinkType linkType, Math::ClipStatus::Type clipStatus);

	Ptr<KOK::DistrictNodeInstance> dist;
};

inline void 
TerrainCell::SetDistrict(const Ptr<KOK::DistrictNodeInstance>& d)
{
	this->dist = d;

	Math::bbox b = d->GetModelNode()->GetBoundingBox();
	this->boundingBox.pmin.y() = b.pmin.y();
	this->boundingBox.pmax.y() = b.pmax.y();
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
