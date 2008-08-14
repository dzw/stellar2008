//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terrainentity.h"
#include "kokterrain/districtnode.h"
#include "kokterrain/districtnodeinstance.h"
#include "kokterrain/terraininstance.h"
#include "models/model.h"
#include "resources/resourcemanager.h"
#include "models/modelinstance.h"


namespace KOK
{
ImplementClass(KOK::TerrainEntity, 'KTNE', Graphics::ModelEntity);

using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::TerrainEntity()
{
	this->SetType(TerrainType);
}

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::~TerrainEntity()
{
    n_assert(!this->modelInstance.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainEntity::OnActivate()
{
    n_assert(!this->IsActive());
    n_assert(this->resId.IsValid());
    GraphicsEntity::OnActivate();
    
    // note: we will remain invalid until at least our model has loaded
    this->SetValid(false);
    this->managedModel = ResourceManager::Instance()->CreateManagedResource(Terrain::RTTI, this->resId).downcast<ManagedModel>();
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainEntity::OnDeactivate()
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
    ResourceManager::Instance()->DiscardManagedResource(this->managedModel.upcast<ManagedResource>());
    this->managedModel = 0;

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

void
TerrainEntity::OnUpdate()
{
	//ValidateModelInstance();
	//
	//if (this->modelInstance.isvalid())
	//{
	//	this->modelInstance->SetTime(this->GetTime());
	//	this->modelInstance->Update();
	//}

	//// important: call parent class!
	//GraphicsEntity::OnUpdate();
	ModelEntity::OnUpdate();
}

void
TerrainEntity::ValidateModelInstance()
{
	//if (!this->modelInstance.isvalid())
	//{
	//	if (this->managedModel->GetState() == Resource::Loaded)
	//	{
	//		const Ptr<Model> model = this->managedModel->GetModel();
	//		n_assert(model->IsLoaded());
	//		this->SetLocalBoundingBox(model->GetBoundingBox());
	//		this->modelInstance = model->CreateInstance();				
	//		this->modelInstance->SetTransform(this->GetTransform());
	//		this->modelInstance->SetModelEntity(this);
	//		this->modelInstance->SetAllNodeInstancesVisible(this->setModelNodesVisible);
	//		this->SetValid(true);
	//	}
	//	else
	//	{
	//		// @todo: check for load failed!
	//	}
	//}
}

} // namespace Graphics
