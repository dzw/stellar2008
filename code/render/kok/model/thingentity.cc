//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/thingentity.h"
#include "kok/model/managedthing.h"
#include "resources/resourcemanager.h"
#include "models/visresolver.h"

namespace KOK
{
ImplementClass(KOK::ThingEntity, 'TGET', Graphics::ModelEntity);

using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ThingEntity::ThingEntity()	
{

	this->SetType(ModelType);
}

//------------------------------------------------------------------------------
/**
*/
ThingEntity::~ThingEntity()
{
}

//------------------------------------------------------------------------------
/**
*/
void
ThingEntity::OnActivate()
{
    n_assert(!this->IsActive());
    n_assert(this->resId.IsValid());
    GraphicsEntity::OnActivate();
    
    // note: we will remain invalid until at least our model has loaded
    this->SetValid(true);
}

//------------------------------------------------------------------------------
/**
*/
void
ThingEntity::OnDeactivate()
{
    n_assert(this->IsActive());
    n_assert(this->managedModel.isvalid());

	// discard our model instance (if exists)
	if (this->modelInstance.isvalid())
	{
		this->modelInstance->Discard();
		this->modelInstance = 0;
	}

    // discard our managed model
	if (this->managedModel.isvalid())
	{
	    ResourceManager::Instance()->DiscardManagedResource(this->managedModel.upcast<ManagedResource>());
		this->managedModel = 0;
	}

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

void
ThingEntity::OnUpdate()
{
	GraphicsEntity::OnUpdate();
	//ModelEntity::OnUpdate();

	if (this->managedModel.isvalid() && this->managedModel->GetState() == Resource::Loaded)
	{
		const Ptr<Thing>& thing = this->GetThing();
		if (thing.isvalid())
		{
			for (SizeT i = 0; i < texs.Size(); i++)
				thing->SetTextureId(texs[i].nodeId, texs[i].texId);
		}
	}

	//if (this->managedModel.isvalid() && this->managedModel->GetThing().isvalid())
	//{
	//	String path;
	//	switch(category)
	//	{
	//	case 0: 
	//		path = "Mesh\\Building\\";		// 建筑物
	//		break;
	//	case 1:
	//		path = "Mesh\\Article\\";		// 地上物
	//		break;
	//	case 2:
	//		path = "Mesh\\Nature\\";		// 自然物
	//		break;
	//	case 3:
	//		path = "Mesh\\Interior\\";		// 室内物
	//		break;
	//	case 4:
	//		path = "Mesh\\Cliff\\";			// 悬崖
	//	}
	//	this->managedModel->GetThing()->LoadTexture(path);
	//}
}

void 
ThingEntity::AttachVisibleInstance()
{
	// 如果没创建资源，就在这创建
	if (!this->managedModel.isvalid())
		this->managedModel = ResourceManager::Instance()->CreateManagedResource(Thing::RTTI, this->resId).downcast<ManagedThing>();

	if (this->modelInstance.isvalid())
		VisResolver::Instance()->AttachVisibleModelInstance(this->modelInstance);
}

void 
ThingEntity::SetTextureId(int nodeId, int texId)
{
	NodeTextureParm tex;
	tex.nodeId = nodeId;
	tex.texId = texId;

	texs.Append(tex);
}

} // namespace Graphics
