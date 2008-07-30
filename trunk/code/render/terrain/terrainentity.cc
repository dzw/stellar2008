//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainentity.h"
#include "terrain/terrainnode.h"
#include "terrain/terrainnodeinstance.h"
#include "models/model.h"
#include "resources/resourcemanager.h"
#include "models/modelinstance.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainEntity, 'TNTY', Graphics::ModelEntity);

using namespace Models;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::TerrainEntity()
{
	this->SetType(TerrainType);
    //this->SetUpdateType(NoUpdate);
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
    this->managedModel = ResourceManager::Instance()->CreateManagedResource(TerrainTile::RTTI, this->resId).downcast<ManagedModel>();
    if (this->managedModel.isvalid())
        this->managedModel.downcast<ManagedTerrainTile>()->GetTile()->SetXZ(this->x, this->z);
    this->ValidateModelInstance();
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

//------------------------------------------------------------------------------
/**
*/
//void
//TerrainEntity::OnUpdate()
//{
//    // first check if our model has been loaded already, if yes
//    // our model instance must be initialized
//    this->ValidateModelInstance();
//
//    // if our model instance is valid, let it update itself
//    if (this->modelInstance.isvalid())
//    {
//        this->modelInstance->SetTime(this->GetTime());
//        this->modelInstance->Update();
//    }
//
//    // important: call parent class!
//    GraphicsEntity::OnUpdate();
//}

//------------------------------------------------------------------------------
/**
*/
//void
//TerrainEntity::ValidateModelInstance()
//{
//    if (!this->modelInstance.isvalid())
//    {
//        if (this->managedTerrainTile->GetState() == Resource::Loaded)
//        {
//            const Ptr<TerrainTile> terrain = this->managedTerrainTile->GetTile();
//            n_assert(terrain->IsLoaded());
//            this->SetLocalBoundingBox(terrain->GetBoundingBox());
//            this->modelInstance = terrain->CreateInstance();
//            this->modelInstance->SetTransform(this->GetTransform());
//            this->modelInstance->SetModelEntity(this);
//            this->modelInstance->SetAllNodeInstancesVisible(false);
//            this->SetValid(true);
//        }
//        else
//        {
//            // @todo: check for load failed!
//        }
//    }
//}

} // namespace Graphics
