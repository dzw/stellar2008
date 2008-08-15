//------------------------------------------------------------------------------
//  modelserver.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terrainserver.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "math/vector3.h"
#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/stage.h"
#include "kokterrain/terrain.h"
#include "kokterrain/managedterrain.h"
#include "kokterrain/load/streamterrainloader.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/indextype.h "
#include "models/visresolver.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"

namespace KOK
{
ImplementClass(KOK::TerrainServer, 'TNSR', Core::RefCounted);
ImplementSingleton(KOK::TerrainServer);

using namespace Core;
using namespace Util;
using namespace IO;
using namespace Attr;
using namespace Resources;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
TerrainServer::TerrainServer():
	isOpen(false),
	terrMeshPool(0)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
TerrainServer::~TerrainServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainServer::Open()
{
	n_assert(!this->IsOpen());
	this->isOpen = true;

	// setup resource mapper for meshes
	Ptr<SimpleResourceMapper> meshMapper = SimpleResourceMapper::Create();
	//meshMapper->SetPlaceholderResourceId(ResourceId("msh:system/placeholder_s_0.nvx2"));
	meshMapper->SetResourceClass(Terrain::RTTI);
	meshMapper->SetResourceLoaderClass(StreamTerrainLoader::RTTI);
	meshMapper->SetManagedResourceClass(ManagedTerrain::RTTI);
	ResourceManager::Instance()->AttachMapper(meshMapper.upcast<ResourceMapper>());

	CreateMeshPool();
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainServer::Close()
{
    n_assert(this->IsOpen());

	this->terrMeshPool = 0;
	this->isOpen = false;
}

void 
TerrainServer::LoadTerrain(const Resources::ResourceId resId)
{
	this->resId = resId;
	this->terrain = TerrainEntity::Create();
	this->terrain->SetResourceId(resId);
	GraphicsServer::Instance()->GetDefaultView()->GetStage()->AttachEntity(this->terrain.upcast<GraphicsEntity>());
}

void
TerrainServer::CreateMeshPool()
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

} // namespace Models