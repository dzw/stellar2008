//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/Terrain/Terrainchunkentity.h"
#include "coregraphics/shaperenderer.h"
#include "resources/resourcemanager.h"
#include "wow/world/streamworldloader.h"

#include "timing/timer.h"
#include "coregraphics/debugview.h"

namespace Graphics
{
ImplementClass(Graphics::TerrainChunkEntity, 'TNCE', Graphics::ModelEntity);

using namespace CoreGraphics;
using namespace Math;
using namespace Models;
using namespace Resources;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
TerrainChunkEntity::TerrainChunkEntity()
{
    this->SetType(ModelType);
	this->SetStaticEnttiy();
}

//------------------------------------------------------------------------------
/**
*/
TerrainChunkEntity::~TerrainChunkEntity()
{
    //n_assert(!this->modelInstance.isvalid());
}

//------------------------------------------------------------------------------
/**
    This will initialize the managed model inside the TerrainChunkEntity.
*/
void
TerrainChunkEntity::OnActivate()
{
    n_assert(!this->IsActive());
    //n_assert(this->resId.IsValid());
    GraphicsEntity::OnActivate();

	n_assert(!terrain.isvalid());
	this->terrain = TerrainChunk::Create();

    

	this->SetValid(false);
}

//------------------------------------------------------------------------------
/**
    Cleanup our managed model, and ModelInstance if it is already 
    initialized.
*/
void
TerrainChunkEntity::OnDeactivate()
{
    n_assert(this->IsActive());

    // discard our model instance (if exists)
    if (this->modelInstance.isvalid())
    {
        this->modelInstance->Discard();
        this->modelInstance = 0;
    }

	if (this->terrain.isvalid())
	{
		this->terrain->Unload();
		this->terrain->SetLoader(0);
		this->terrain = 0;
	}

 //   if (managedChunk.isvalid())
	//{
	//	ResourceManager::Instance()->DiscardManagedResource(managedChunk.upcast<ManagedResource>());
	//	this->managedChunk = 0;
	//}

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    In OnTransformChanged() we need to update the transformation
    of our ModelInstance (if it has already been initialised).
*/
void
TerrainChunkEntity::OnTransformChanged()
{
    if (this->modelInstance.isvalid())
    {
        this->modelInstance->SetTransform(this->GetTransform());
    }
}

//------------------------------------------------------------------------------
/**
    In OnUpdate() the TerrainChunkEntity first needs to check whether the
    ManagedModel has already been loaded, and if yes, a ModelInstance
    will be created, finally, the ModelInstance will be prepared for
    rendering.
*/
void
TerrainChunkEntity::OnUpdate()
{
	//this->ValidateModelInstance();

    // if our model instance is valid, let it update itself
    /*if (this->modelInstance.isvalid())
    {
        this->modelInstance->SetTime(this->GetTime());
        this->modelInstance->Update();
    }*/

    // important: call parent class!
    GraphicsEntity::OnUpdate();
}

//------------------------------------------------------------------------------
/**
    This creates and initializes our ModelInstance if needed. Since Model 
    loading happens asynchronously this may happen at any time after
    the TerrainChunkEntity is activated.
*/
void
TerrainChunkEntity::ValidateModelInstance()
{
	if (!this->terrain.isvalid())
		return;
	
    if (this->terrain->GetState() == Resource::Loaded && this->terrain->NeedUpdate())
    {
        /*Timing::Timer tt;
	    tt.Reset();
	    tt.Start();*/

		if (this->modelInstance.isvalid())
		{
			this->modelInstance->Discard();
			this->modelInstance = 0;
		}

        this->SetLocalBoundingBox(terrain->GetBoundingBox());
		this->modelInstance = terrain->CreateInstance();
		this->modelInstance->SetModelEntity((ModelEntity*)this);
		this->modelInstance->SetAllNodeInstancesVisible(true);
        this->modelInstance->SetTransform(this->GetTransform());

		this->modelInstance->SetTime(this->GetTime());
		this->modelInstance->Update();

        this->SetValid(true);

        /*if (managedChunk.isvalid())
	    {
		    ResourceManager::Instance()->DiscardManagedResource(managedChunk.upcast<ManagedResource>());
		    this->managedChunk = 0;
	    }*/

        this->terrain->SetNeedUpdate(false);

        /*tt.Stop();
	    float t = (float)tt.GetTime();
	    String output;
	    output.AppendFloat(t);
        CoreGraphics::DebugView::Instance()->AddDebugString("ValidateModelInstance", output);*/
    }

  //  if (!this->modelInstance.isvalid())
  //  {
		//if (this->terrain.isvalid() && this->terrain->GetState() == Resource::Loaded)
  //      {
  //          this->SetLocalBoundingBox(terrain->GetBoundingBox());
  //          this->modelInstance = terrain->CreateInstance();
  //          this->modelInstance->SetTransform(this->GetTransform());
  //          this->modelInstance->SetModelEntity((ModelEntity*)this);
  //          this->modelInstance->SetAllNodeInstancesVisible(true);
  //          this->SetValid(true);
  //      }
  //      else
  //      {
  //          // @todo: check for load failed!
  //      }
  //  }
}

//------------------------------------------------------------------------------
/**
    Render our debug visualization (the bounding box).
*/
void
TerrainChunkEntity::OnRenderDebug()
{
    float4 color(1.0f, 0.0f, 0.0f, 0.5f);
    matrix44 shapeTransform = this->localBox.to_matrix44();//this->GetGlobalBoundingBox().to_matrix44();
    ShapeRenderer::Instance()->DrawShape(shapeTransform, ShapeRenderer::Box, color);

    this->GetModelInstance()->RenderDebug();
}

//------------------------------------------------------------------------------
/**
   1_1.adt 转成  1_1_001.chk 其中001代表chunk编号
*/
void 
TerrainChunkEntity::LoadChunk(const Resources::ResourceId& resId, int chunkId)
{
	String name = resId.Value();
	String chkId;
	chkId.Format("_%03d.chk", chunkId);
	name.SubstituteString(".adt", chkId);
	
	this->resId = Resources::ResourceId(name);
    this->terrain->SetChunkId(chunkId);
	this->terrain->SetResourceId(this->resId);
	this->terrain->SetState(Resource::Initial);
	this->terrain->SetAsyncEnabled(true);
	this->terrain->SetLoader((ResourceLoader*)WOW::StreamWorldLoader::Create());
	this->terrain->Load();

    //this->managedChunk = ResourceManager::Instance()->CreateManagedResource(Chunk::RTTI, this->resId).downcast<ManagedChunk>();
}

} // namespace Graphics
