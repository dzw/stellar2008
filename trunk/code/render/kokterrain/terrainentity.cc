//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terrainentity.h"
#include "kokterrain/districtnode.h"
#include "kokterrain/districtnodeinstance.h"
#include "models/model.h"
#include "resources/resourcemanager.h"
#include "models/modelinstance.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"

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
	ModelEntity::OnUpdate();

	if (this->managedModel.isvalid())
	{
		vector camPos = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity()->GetTransform().getrow3();
		if (camPos != savePos)
		{
			Ptr<Terrain> terr = this->managedModel.downcast<ManagedTerrain>()->GetTerrain();
			if (terr.isvalid())
				terr->CheckDistrict(camPos);
			savePos = camPos;
		}
	}
}

} // namespace Graphics
