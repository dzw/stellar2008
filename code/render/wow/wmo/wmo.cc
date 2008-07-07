//------------------------------------------------------------------------------
//  model.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/wmo/wmo.h"
//#include "wow/wmo/wmoinstance.h"
#include "wow/wmo/wmoserver.h"
#include "graphics/graphicsserver.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"
#include "math/matrix44.h"
#include "math/quaternion.h"
#include "models/modelserver.h"

namespace WOW
{
ImplementClass(WOW::WMO, 'WMOW', Resources::Resource);

using namespace Util;
using namespace Attr;
using namespace Resources;
using namespace Graphics;
using namespace Math;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
WMO::Group::Group():
	ddr(0),
	nDoodads(0),
	gc(0),
	flages(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
WMO::Group::~Group()
{
	if (ddr)
	{
		n_delete_array(ddr);
		ddr = NULL;
	}
	this->gc = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
WMO::Group::SetConvexZone(const Ptr<ConvexZone>& g)
{
	n_assert(g.isvalid());
	gc = g;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ConvexZone>&
WMO::Group::GetConvexZone()
{
	return this->gc;
}

//------------------------------------------------------------------------------
/**
*/
void 
WMO::Group::SetGroupName(const Resources::ResourceId& name)
{
	this->resId = name;
}

//------------------------------------------------------------------------------
/**
*/
const Resources::ResourceId& 
WMO::Group::GetGroupName()
{
	return this->resId;
}
//////////////////////////////////////////////////////////////////////////



//------------------------------------------------------------------------------
/**
*/
WMO::WMO() :
    inLoadResources(false),
	mat(NULL)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WMO::~WMO()
{
}

//------------------------------------------------------------------------------
/**
*/
void
WMO::Unload()
{    
	if (mat) n_delete_array(mat);

	this->modelInstances.Clear();
	
	for (SizeT i = 0; i < this->zones.Size(); i++)
		n_delete(zones[i]);
	zones.Clear();

    // call parent class
    Resource::Unload();
}


//------------------------------------------------------------------------------
/**
	doodadset还未被使用
*/
bool
WMO::CreateInstance(vector& ofs, vector& dir, int ndoodadset)
{
	GraphicsServer* gfxServer = GraphicsServer::Instance();
	Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));

	matrix44 wmoTransform, doodadsTransform, tmpTransform;
	wmoTransform = matrix44::identity();
	if (dir.z())
		wmoTransform = matrix44::multiply(wmoTransform ,matrix44::rotationx(2.0f * N_PI * dir.z() / 360.0f));
	if (dir.y())
		wmoTransform = matrix44::multiply(wmoTransform ,matrix44::rotationy(2.0f * N_PI * dir.y() / 360.0f - N_PI / 2.0f));
	if (dir.x())
		wmoTransform = matrix44::multiply(wmoTransform ,matrix44::rotationz(2.0f * N_PI * dir.x() / 360.0f));
	
	wmoTransform = matrix44::multiply(wmoTransform, matrix44::translation(ofs));

	this->doodadset = 1;//ndoodadset;

	Ptr<ConvexZone> g1, g2;
	WMOPR *pr1, *pr2;
	for (SizeT i = 0; i <  this->pvs.Size(); i++)
	{
		g1 = 0;
		g2 = 0;
		pr1 = 0;
		pr2 = 0;
		for (SizeT n = 0; n < this->prs.Size(); n++)
		{
			if (this->prs[n].portal == i)
			{
				if (this->prs[n].dir > 0)
					pr1 = &this->prs[n];
				else
					pr2 = &this->prs[n];

				if (0 != pr1 && 0 != pr2)
					break;
			}
		}

		if (0 != pr1)
			g1 = this->zones[pr1->group]->GetConvexZone();//groups.ValueAtIndex(pr1->group)->GetConvexZone();
		if (0 != pr2)
			g2 = this->zones[pr2->group]->GetConvexZone();//groups.ValueAtIndex(pr2->group)->GetConvexZone();

		vector a(this->pvs[i].a.x, this->pvs[i].a.y, this->pvs[i].a.z);
		vector b(this->pvs[i].b.x, this->pvs[i].b.y, this->pvs[i].b.z);
		vector c(this->pvs[i].c.x, this->pvs[i].c.y, this->pvs[i].c.z);
		vector d(this->pvs[i].d.x, this->pvs[i].d.y, this->pvs[i].d.z);

		// 转换成世界坐标
		a = vector::transformcoord(a, wmoTransform);
		b = vector::transformcoord(b, wmoTransform);
		c = vector::transformcoord(c, wmoTransform);
		d = vector::transformcoord(d, wmoTransform);

		Ptr<Portal> p = Portal::Create();
		p->SetConvexZone(g1, g2);
		p->SetPlane(a, b, c, d); 

		if (g1.isvalid())
			g1->AttachPortal(p);
		if (g2.isvalid())
			g2->AttachPortal(p);
	}

	for (SizeT i = 0; i < this->zones.Size(); i++)
	{
		Group* gp = this->zones[i];
		Ptr<ModelEntity> group = ModelEntity::Create();
		
		group->SetResourceId(gp->GetGroupName());
		group->SetTransform(wmoTransform);
		stage->AttachEntity(group.upcast<GraphicsEntity>());

		//groupInstances.Append(group);
	}

	for (SizeT i = 0; i < this->models.Size(); i++)
	//SizeT i = 29;
	{
		vector &vpos  = this->models[i].pos;
		vector &vdir  = this->models[i].dir;
		float &fscale = this->models[i].sc;
		float &fw	  = this->models[i].w;

		Ptr<ModelEntity> model = ModelEntity::Create();
		model->SetResourceId(this->models[i].m2Name);

		// 设置变换
		doodadsTransform = matrix44::identity();
		quaternion q(-vdir.z(), vdir.x(), vdir.y(), fw);
		doodadsTransform = matrix44::scaling(fscale, -fscale, -fscale);
		doodadsTransform = matrix44::multiply(doodadsTransform, matrix44::rotationquaternion(q));
		doodadsTransform = matrix44::multiply(doodadsTransform, matrix44::translation(vpos));

		doodadsTransform = matrix44::multiply(doodadsTransform, wmoTransform);

		model->SetTransform(doodadsTransform);

		stage->AttachEntity(model.upcast<GraphicsEntity>());

		modelInstances.Append(model.upcast<GraphicsEntity>());
	}

	return true;
}

//------------------------------------------------------------------------------
/**
	判断model是否属于指定的zone，因为WOW模型空间判断不精确，用这个方法可以解决问题
*/
bool 
WMO::CheckModelToZone(const Ptr<Graphics::GraphicsEntity>& entity, const Ptr<Cell>& zone)
{
	if (!zone->IsA(ConvexZone::RTTI))
		return false;

	Group* gp = NULL;
	for (SizeT i = 0; i < this->zones.Size(); i++)
	{
		if (this->zones[i]->GetConvexZone() == zone.downcast<ConvexZone>())
		{
			gp = this->zones[i];
			break;
		}
	}
	
	if (gp == NULL)
		return false;

	/*IndexT index = modelInstances->FindIndex(entity);
	if (index != InvalidIndex)
	{
		if ((index >= this->doodadsets[doodadset].start) && (index < (this->doodadsets[doodadset].start+this->doodadsets[doodadset].size)))
			return true;
	}*/

	for (SizeT n = 0; n < (SizeT)gp->GetDoodads(); n++)
	{
		short dd = gp->GetDDR(n);
		if ((dd >= this->doodadsets[doodadset].start) && (dd < (this->doodadsets[doodadset].start+this->doodadsets[doodadset].size)))
		{
			if (modelInstances[dd] == entity)
				return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
WMO::GetTextureName(IndexT index)const
{
	n_assert(index < textures.Size());
	return textures[index];
}

//------------------------------------------------------------------------------
/**
*/
const WMOMaterial* 
WMO::GetMaterial(IndexT index)const
{
	n_assert(index < (IndexT)this->nTextures);
	return &this->mat[index];
}

//------------------------------------------------------------------------------
/**
*/
const WMODoodadSet* 
WMO::GetDoodadSet(IndexT index)const
{
	n_assert(index < (IndexT)this->nDoodadSets);
	return &this->doodadsets[index];
}

//------------------------------------------------------------------------------
/**
*/
//inline void 
//WMO::Rotate(float x0, float y0, float *x, float *y, float angle)
//{
//	float xa = *x - x0, ya = *y - y0;
//	*x = xa*n_cos(angle) - ya*n_sin(angle) + x0;
//	*y = xa*n_sin(angle) + ya*n_cos(angle) + y0;
//}


//------------------------------------------------------------------------------
/**
*/
WMO::Group* 
WMO::CreateGroup(const Resources::ResourceId& name, bbox& b, int flags)
{
	Group* g;
	g = GetGroup(name);
	if (g != NULL)
		return g;

	g = n_new(WMO::Group);

	/*GraphicsServer* gfxServer = GraphicsServer::Instance();
	Ptr<Stage> stage = gfxServer->GetStageByName(String("DefaultStage"));
	Ptr<WMOZone> gc = WMOZone::Create();
	gc->SetWMO(this);
	gc->SetBoundingBox(b);
	gc->SetOutDoor(!((flags&0x2000)>0));
	gc->OnAttachToStage(stage);
	stage->GetRootCell()->AttachChildCell(gc.upcast<Cell>());
	
	
	g->SetConvexZone(gc.upcast<ConvexZone>());
	g->SetGroupName(name);
	g->SetBoundBox(b);
	g->SetFlags(flags);

	this->zones.Append(g);*/
	return g;
}

//------------------------------------------------------------------------------
/**
*/
WMO::Group*
WMO::GetGroup(const Resources::ResourceId& name)
{
	for (SizeT i = 0; i < this->zones.Size(); i++)
	{
		if (this->zones[i]->GetGroupName() == name)
			return this->zones[i];
	}

	return NULL;
}

} // namespace Models
