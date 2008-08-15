//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terrainentity.h"
#include "kokterrain/districtnode.h"
#include "kokterrain/districtnodeinstance.h"
#include "kokterrain/terraininstance.h"
#include "resources/resourcemanager.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/indextype.h "

namespace KOK
{
ImplementClass(KOK::TerrainEntity, 'TNET', Graphics::ModelEntity);
ImplementSingleton(KOK::TerrainEntity);

using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::TerrainEntity()
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
	this->terrMeshPool   = 0;
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
    this->managedTerrain = ResourceManager::Instance()->CreateManagedResource(Terrain::RTTI, this->resId).downcast<ManagedTerrain>();

	CreateMeshPool();
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainEntity::OnDeactivate()
{
    n_assert(this->IsActive());
    n_assert(this->managedTerrain.isvalid());

    // discard our managed model
    ResourceManager::Instance()->DiscardManagedResource(this->managedTerrain.upcast<ManagedResource>());
    this->managedTerrain = 0;

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
	GraphicsEntity::OnUpdate();
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

	this->terrVertexPool = VertexChunkPool::Create();
	this->terrVertexPool->Reset(sizeof(TileMesh), vertexSize, 100, vertexComponents);
	/*ushort* indexPtr = this->distMeshPool->LockIndexed();
	Memory::Copy(indices, indexPtr, indexSize*sizeof(WORD));
	this->distMeshPool->UnlockIndexed();*/

	DWORD tileVertexSize = 256 * 100;
	DWORD tileIndexSize = 384 * 2 * 100;

	this->terrMeshPool = DynamicMeshPool::Create();
	this->terrMeshPool->Reset(sizeof(TileMesh), tileVertexSize, vertexComponents, tileIndexSize, IndexType::Index16);
}

} // namespace Graphics
