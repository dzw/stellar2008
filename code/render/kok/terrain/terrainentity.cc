//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/terrain/terrainentity.h"
#include "kok/terrain/districtnode.h"
#include "resources/resourcemanager.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/indextype.h "
#include "models/modelnodetype.h"
#include "models/visresolver.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/indexbufferpool.h"
#include "coregraphics/indextype.h"
#include "graphics/stage.h"

namespace KOK
{
ImplementClass(KOK::TerrainEntity, 'TNET', Graphics::ModelEntity);
ImplementSingleton(KOK::TerrainEntity);

using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::TerrainEntity():
	terrain(0),
	terrVertexPool(0),
	indexPool(0)
{
	ConstructSingleton;

	this->SetType(TerrainType);
}

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::~TerrainEntity()
{
	DestructSingleton;

	this->terrVertexPool = 0;
	this->indexPool   = 0;
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

	CreateMeshPool();
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
	if (!this->terrain.isvalid() && this->modelInstance.isvalid())
	{
		this->terrain = this->managedModel.downcast<ManagedTerrain>()->GetTerrain();
		this->terrain->CreateQuadTree(this->stage->GetRootCell());
		this->SetLocalBoundingBox(this->terrain->GetBoundingBox());
		this->terrain->AttachThingsToStage(this->stage);
	}
}

void
TerrainEntity::CreateMeshPool()
{
	// …Ë÷√∂•µ„ª∫≥Â
	int vertexSize = DISTRICT_VERTS*DISTRICT_VERTS*4;
	/*int indexSize = DISTRICT_VERTS*DISTRICT_VERTS*6;
	WORD indices[DISTRICT_VERTS*DISTRICT_VERTS*6];

	SizeT curIndex = 0;
	for (SizeT i = 0; i < indexSize; i+=6)
	{
	indices[i + 0] = curIndex + 0;
	indices[i + 1] = curIndex + 1;
	indices[i + 2] = curIndex + 2;

	indices[i + 3] = curIndex + 1;
	indices[i + 4] = curIndex + 3;
	indices[i + 5] = curIndex + 2;

	curIndex += 4;
	}*/

	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 1, VertexComponent::Float2));
	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 2, VertexComponent::Float2));
	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 3, VertexComponent::Float2));
	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 4, VertexComponent::Float2));
	
	DWORD tileVertexSize = 256 * 64;
	DWORD tileIndexSize = 384 * 2 * 64;

	this->terrVertexPool = VertexChunkPool::Create();
	this->terrVertexPool->Reset(sizeof(TileMesh), vertexSize, 64, vertexComponents);

	this->indexPool = IndexBufferPool::Create();
	this->indexPool->Reset(tileIndexSize, IndexType::Index16);

	//this->terrMeshPool = DynamicMeshPool::Create();
	//this->terrMeshPool->Reset(sizeof(TileMesh), tileVertexSize, vertexComponents, tileIndexSize, IndexType::Index16);
}

} // namespace Graphics
