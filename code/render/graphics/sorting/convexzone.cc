//------------------------------------------------------------------------------
//  groupcell.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/stage.h"
#include "graphics/sorting/convexzone.h"
#include "graphics/cameraentity.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/shaperenderer.h"
#include "graphics/modelentity.h"

namespace Graphics
{
ImplementClass(Graphics::ConvexZone, 'CXZE', Graphics::Cell);

using namespace Math;
using namespace Util;
using namespace CoreGraphics;

bool Graphics::ConvexZone::inGroup = false;
Ptr<ConvexZone> Graphics::ConvexZone::curConvexZone = 0;
matrix44 Graphics::ConvexZone::view = matrix44::identity();
vector Graphics::ConvexZone::startPoint = vector::nullvec();
vector Graphics::ConvexZone::endPoint = vector::nullvec();
vector Graphics::ConvexZone::viewPos = vector::nullvec();

//------------------------------------------------------------------------------
/**
*/
ConvexZone::ConvexZone() :
	isOutDoor(false)
{
}

//------------------------------------------------------------------------------
/**
*/
ConvexZone::~ConvexZone()
{
	this->portals.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexZone::OnRemoveFromStage()
{
	// 释放静态变量
	if (curConvexZone.get_unsafe() == this)
		curConvexZone = 0;

	this->portals.Clear();
	
	Cell::OnRemoveFromStage();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Cell> 
ConvexZone::FindEntityContainmentCell(const Ptr<GraphicsEntity>& entity)
{
	// get global bounding box of entity
	const bbox& entityBox = entity->GetGlobalBoundingBox();

	return this->FindZone(entityBox);
}

//------------------------------------------------------------------------------
/**
	每次从根开始找包含的Zone

	子类可以更改实现方式，以适合不同的情况，如WOW中entity的boundbox与Zone包含位置
	存在问题，所以不能直接使用包含关系来判断所属的Zone。
*/
Ptr<Cell>
ConvexZone::FindZone(const Math::bbox& entityBox)
{
	// find the first upward cell which completely contains the entity,
	// stop at tree root
	Ptr<Cell> curCell = this->stage->GetRootCell();//this;  每次都从根目录开始找最佳位置
	
	// find smallest downward cell which completely contains the entity
	vector minBoxsize = curCell->GetBoundingBox().size();
	IndexT cellIndex;
	SizeT numCells;
	do
	{
		const Array<Ptr<Cell>>& curChildren = curCell->GetChildCells();
		numCells = curChildren.Size();
		for (cellIndex = 0; cellIndex < numCells; cellIndex++)
		{
			const Ptr<Cell>& childCell = curChildren[cellIndex];
			if (childCell->GetBoundingBox().contains(entityBox))
			{			
				vector minbox = vector::minimize(minBoxsize, childCell->GetBoundingBox().size());
				if (minbox != minBoxsize)
				{
					curCell = childCell;
					minBoxsize = childCell->GetBoundingBox().size();
				}
			}
		}
		// check for loop fallthrough: this means that the current cell either has
		// no children, or that none of the children completely contains the entity
	}
	while (cellIndex != numCells);
	return curCell;
}

//------------------------------------------------------------------------------
/**
	根节点负责管理所有cell
*/
void
ConvexZone::UpdateLinks(const Ptr<GraphicsEntity>& observerEntity, uint observedEntityTypeMask, GraphicsEntity::LinkType linkType)
{
	UpdateLocation(observerEntity);

	if (inGroup && curConvexZone.isvalid())
	{
		curConvexZone->RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, ClipStatus::Invalid);
	}
	else
	{
		// 在室外，更新所有室外场景
		for (SizeT i = 0; i < this->childCells.Size(); i++)
		{
			if (!this->childCells[i]->IsA(ConvexZone::RTTI))
				continue;

			if (this->childCells[i].downcast<ConvexZone>()->IsOutDoor())
			{
				this->childCells[i]->RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, ClipStatus::Invalid);
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
    Recursively update visibility links. This method is called by the
    top level method UpdateLinks(). 
    
    NOTE: This is the core visibility detection method and must be FAST.
*/
void
ConvexZone::RecurseUpdateLinks(const Ptr<GraphicsEntity>& observerEntity,
                         uint observedEntityTypeMask,
                         GraphicsEntity::LinkType linkType,
                         ClipStatus::Type clipStatus)
{
	n_assert(observerEntity.isvalid());

	UpdateZoneLinks(observerEntity, observedEntityTypeMask, linkType, clipStatus);

	for (SizeT i = 0; i < portals.Size(); i++)
	{
		if (portals[i]->IsOpen())
			continue;
		portals[i]->SetOpen(true);

		Ptr<ConvexZone> gc = 0;
		gc = portals[i]->GetOtherZone(this);

		if (observerEntity->ComputeClipStatus(portals[i]->GetBoundBox()) != ClipStatus::Outside)
		{
			matrix44 oldProj, newProj;
			bool b = ReducedFrustum(newProj, portals[i], observerEntity);
			if (b)
			{
				oldProj = observerEntity.downcast<CameraEntity>()->GetProjTransform();
				observerEntity.downcast<CameraEntity>()->SetProjTransform(newProj);

				gc->RecurseUpdateLinks(observerEntity, observedEntityTypeMask, linkType, clipStatus);

				observerEntity.downcast<CameraEntity>()->SetProjTransform(oldProj);
			}
		}

		portals[i]->SetOpen(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ConvexZone::UpdateZoneLinks(const Ptr<GraphicsEntity>& observerEntity,
						 uint observedEntityTypeMask,
						 GraphicsEntity::LinkType linkType,
						 ClipStatus::Type clipStatus)
{
	n_assert(observerEntity.isvalid());

	// break immediately if no entity of wanted type in this cell or below
	if (this->GetNumEntitiesInHierarchyByTypeMask(observedEntityTypeMask) == 0)
	{
		return;
	}

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
				const Array<Ptr<GraphicsEntity>>& observedEntities = this->entitiesByType[observedType];
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
				const Array<Ptr<GraphicsEntity>>& observedEntities = this->entitiesByType[observedType];
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
}

//------------------------------------------------------------------------------
/**
	判断是否穿过portal到室内
*/
bool 
ConvexZone::UpdateLocation(const Ptr<GraphicsEntity>& observerEntity)
{
	matrix44 curview = observerEntity.downcast<CameraEntity>()->GetViewTransform();
	if (curview != view)
	{
		view = curview;
		matrix44 invview = matrix44::inverse(view);
		viewPos = invview.getrow3();
		//static vector startPoint = viewPos, endPoint = viewPos;
		if (viewPos != endPoint)
		{
			startPoint = endPoint;
			endPoint = viewPos;

		}
	
		Ptr<Cell> curCell = 0;

		if (!inGroup)
		{
			bbox b(endPoint, vector::nullvec());
			curCell = FindZone(b);
		}
		else
			curCell = curConvexZone;

		if (curCell.isvalid() && curCell->IsA(ConvexZone::RTTI))
		{
			curCell.downcast<ConvexZone>()->CheckAcrossPortal();
		}
	}

	return true;
}

//------------------------------------------------------------------------------
/**
	判断当前是否有被穿过的portal
*/
void
ConvexZone::CheckAcrossPortal()
{
	for (SizeT i = 0; i < portals.Size(); i++)
	{
		if (portals[i]->IntersectLine(startPoint, endPoint))
		{
			if (!inGroup)
			{
				// 室外到室内
				curConvexZone = portals[i]->GetIndoorZone();
			}
			else
			{
				curConvexZone = portals[i]->GetOtherZone(curConvexZone);
			}

			if (!curConvexZone.isvalid() || curConvexZone->IsOutDoor())
			{
				inGroup = false;
				curConvexZone = 0;
			}
			else
				inGroup = true;
		}
	}
}

//------------------------------------------------------------------------------
/**
	Render our debug visualization (the bounding box).
*/
void
ConvexZone::OnRenderDebug(/*const bbox& b*/)
{
	for (SizeT i = 0; i < this->childCells.Size(); i++)
		childCells[i]->OnRenderDebug();
	
	for (SizeT n = 0; n < portals.Size(); n++)
	{
		float4 color(0.0f, 0.0f, 1.0f, 0.3f);
		matrix44 shapeTransform = portals[n]->GetBoundBox().to_matrix44();
		ShapeRenderer::Instance()->DrawShape(shapeTransform, ShapeRenderer::Box, color);
	}

	//cell 包围盒
	/*float numcell = 0.2f;
	float4 color(0.0f, 1.0f, 1.0f, numcell);
	matrix44 shapeTransform = this->boundingBox.to_matrix44();
	ShapeRenderer::Instance()->DrawShape(shapeTransform, ShapeRenderer::Box, color);
	numcell += 0.3f;*/
}

//------------------------------------------------------------------------------
/**
*/
bool 
ConvexZone::ReducedFrustum(matrix44& proj, const Ptr<Portal>& portal, const Ptr<GraphicsEntity>& observerEntity)
{
	if (!observerEntity->IsA(CameraEntity::RTTI))
		return false;


	float l=9999.0f, r=-9999.0f, t=-9999.0f, b=9999.0f, zn, zf;
	vector trans;
	//matrix44 view, invview;
	const vector* vertices = portal->GetVertices();

	//view = observerEntity.downcast<CameraEntity>()->GetViewTransform();
	if (this->IsOutDoor())
	{
		// 如果在室外，如果portal与摄像机平行或在后面就不处理。
		//invview = matrix44::inverse(view);
		//vector viewPos = invview.row3();
		float dst = portal->Distance(vertices[0] - viewPos);
		if (dst <= 0.0f)
			return false;
	}

	zf = observerEntity.downcast<CameraEntity>()->GetZFar();
	zn = observerEntity.downcast<CameraEntity>()->GetZNear();
	
	for (SizeT i = 0; i < 4; i++)
	{
		trans = vector::transformcoord(vertices[i], view);

		// 计算portal点投影到近裁剪面的位置。
		// 用abs使比例不能为负，以保证x原来的值，如果是负的，最终结果与原来正好相反
		trans.x() = trans.x() * n_abs(zn/trans.z());	
		trans.y() = trans.y() * n_abs(zn/trans.z());

		if (trans.x() < l)
			l = trans.x();
		if (trans.x() > r)
			r = trans.x();
		if (trans.y() < b)
			b = trans.y();
		if (trans.y() > t)
			t = trans.y();
	}
	
	proj = matrix44::perspoffcenterrh(l, r, b, t, zn, zf);

	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexZone::AttachPortal(const Ptr<Portal>& portal)
{
	portals.Append(portal);
}

//------------------------------------------------------------------------------
/**
*/
void 
ConvexZone::RemovePortal(const Ptr<Portal>& portal)
{
	Array<Ptr<Portal>>::Iterator iter = portals.Find(portal);
	if (iter != portals.End())
	{
		n_delete(portal);
		portals.Erase(iter);
	}
}

//------------------------------------------------------------------------------
/**
	判断线段是否穿过了某个portal
*/
bool 
ConvexZone::ComputePortalAcross(Math::vector& startPoint, Math::vector& endPoint)
{
	for (SizeT i = 0; i < this->portals.Size(); i++)
	{
		if (this->portals[i]->IntersectLine(startPoint, endPoint))
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
NOTE: the cell hierarchy may only be built during the setup phase while
the cell hierarchy haven't been added to the stage yet.
*/
void
ConvexZone::AttachChildCell(const Ptr<Cell>& cell)
{
	//n_assert(!this->stage.isvalid());  // 直接加载cell,用于portal   by ctuo
	n_assert(cell.isvalid())
		n_assert(!cell->GetParentCell().isvalid()); 

	cell->parentCell = this;
	this->childCells.Append(cell);
}

} // namespace Graphics
