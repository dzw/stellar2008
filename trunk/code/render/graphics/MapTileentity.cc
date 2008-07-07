//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/maptileentity.h"
#include "models/modelserver.h"
#include "wow/world/worldmanager.h"
#include "foundation/timing\timer.h"

namespace Graphics
{
ImplementClass(Graphics::MapTileEntity, 'MPTE', Graphics::ModelEntity);

using namespace Models;

//------------------------------------------------------------------------------
/**
*/
MapTileEntity::MapTileEntity()
{
}

//------------------------------------------------------------------------------
/**
*/
MapTileEntity::~MapTileEntity()
{
}

//------------------------------------------------------------------------------
/**
	重用顶点数据，reset新的MAPTILE
*/
void 
MapTileEntity::Reset(const Resources::ResourceId& resId, int x, int y)
{
	this->x = x;
	this->y = y;
	if (this->managedModel.isvalid())
	{
		// 删除原来的maptile
		// discard our model instance (if exists)
		if (this->modelInstance.isvalid())
		{
			this->modelInstance->Discard();
			this->modelInstance = 0;
		}

		// discard our managed model
		ModelServer::Instance()->DiscardManagedModel(this->managedModel);
		this->managedModel = 0;
	}

	this->SetValid(false);
	this->managedModel = ModelServer::Instance()->LoadManagedModel(this->resId);
	//this->ValidateModelInstance();
}

} // namespace Graphics
