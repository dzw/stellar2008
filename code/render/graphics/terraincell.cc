//------------------------------------------------------------------------------
//  cell.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/terraincell.h"
#include "graphics/graphicsentity.h"
#include "graphics/worldstage.h"

namespace Graphics
{
ImplementClass(Graphics::TerrainCell, 'TNCL', Graphics::Cell);

using namespace Math;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
TerrainCell::TerrainCell():
	distId(-1)
{
    
}

//------------------------------------------------------------------------------
/**
*/
TerrainCell::~TerrainCell()
{
    
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainCell::RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity,
                         uint observedEntityTypeMask,
                         GraphicsEntity::LinkType linkType,
                         ClipStatus::Type clipStatus)
{
	n_assert(observerEntity.isvalid());

	// break immediately if no entity of wanted type in this cell or below
	/*if (this->GetNumEntitiesInHierarchyByTypeMask(observedEntityTypeMask) == 0)
	{
		return;
	}*/

	// if clip status unknown or clipped, get clip status of this cell against observer entity
	if ((ClipStatus::Invalid == clipStatus) || (ClipStatus::Clipped == clipStatus))
	{
		const bbox& cellBox = this->GetBoundingBox();
		clipStatus = observerEntity->ComputeClipStatus(cellBox);
	}

	// proceed depending on clip status of cell against observer entity
	if (ClipStatus::Outside == clipStatus)
	{
		// cell isn't visible by observer entity
		return;
	}
	else if (ClipStatus::Inside == clipStatus)
	{
		// cell is fully visible by observer entity, everything inside
		// the cell is fully visible as well
		IndexT observedType;
		for (observedType = 0; observedType < GraphicsEntity::NumTypes; observedType++)
		{
			if (0 != (observedEntityTypeMask & (1<<observedType)))
			{
				const Array<Ptr<GraphicsEntity> >& observedEntities = this->entitiesByType[observedType];
				IndexT index;
				SizeT num = observedEntities.Size();
				for (index = 0; index < num; index++)
				{
					const Ptr<GraphicsEntity>& observedEntity = observedEntities[index];

					// short cut: if generating light links, check if the observed
					// entity is actually visible by any camera
					if ((GraphicsEntity::LightLink == linkType) && 
						(observedEntity->GetLinks(GraphicsEntity::CameraLink).IsEmpty()))
					{
						continue;
					}
					else if (observedEntity->IsValid() && observedEntity->IsVisible())
					{
						// add bi-directional visibility link
						observerEntity->AddLink(linkType, observedEntity);
						observedEntity->AddLink(linkType, observerEntity);
					}
				}
			}
		}
	}
	else
	{
		// cell is partially visible, must check visibility of each contained entity
		IndexT observedType;
		for (observedType = 0; observedType < GraphicsEntity::NumTypes; observedType++)
		{
			if (0 != (observedEntityTypeMask & (1<<observedType)))
			{
				const Array<Ptr<GraphicsEntity> >& observedEntities = this->entitiesByType[observedType];
				IndexT index;
				SizeT num = observedEntities.Size();
				for (index = 0; index < num; index++)
				{
					const Ptr<GraphicsEntity>& observedEntity = observedEntities[index];

					// short cut: if generating light links, check if the observed
					// entity is actually visible by any camera
					if ((GraphicsEntity::LightLink == linkType) && 
						(observedEntity->GetLinks(GraphicsEntity::CameraLink).IsEmpty()))
					{
						continue;
					}
					else if (observedEntity->IsValid() && observedEntity->IsVisible())
					{
						if (observerEntity->ComputeClipStatus(observedEntity->GetGlobalBoundingBox()) != ClipStatus::Outside)
						{
							// add bi-directional visibility link
							observerEntity->AddLink(linkType, observedEntity);
							observedEntity->AddLink(linkType, observerEntity);
						}
					}
				}
			}
		}
	}

	if (linkType == GraphicsEntity::CameraLink)
	{
		if (this->distId != -1)
		{
			this->stage.downcast<WorldStage>()->AppendDistrict(this->id);
		}
	}

	// recurse into child cells (if this cell is fully or partially visible)
	IndexT childIndex;
	SizeT numChildren = this->childCells.Size();
	for (childIndex = 0; childIndex < numChildren; childIndex++)
	{
		this->childCells[childIndex]->RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, clipStatus);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainCell::OnRenderDebug(/*const bbox& b*/)
{
	/*IndexT childIndex;
	SizeT numChildren = this->childCells.Size();
	for (childIndex = 0; childIndex < numChildren; childIndex++)
	{
		this->childCells[childIndex]->OnRenderDebug();
	}*/
}

} // namespace Graphics
