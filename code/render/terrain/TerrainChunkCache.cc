//------------------------------------------------------------------------------
//  managedmodel.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainchunkcache.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "resources/resourceloader.h"

const Resources::ResourceId meshName = "ChunkCache";

namespace Terrain
{
ImplementClass(Terrain::TerrainChunkCache, 'MDCC', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
TerrainChunkCache::TerrainChunkCache()
{
}

//------------------------------------------------------------------------------
/**
*/
TerrainChunkCache::~TerrainChunkCache()
{
}

//------------------------------------------------------------------------------
/**
	增加一个chunk用于渲染
*/
DWORD 
TerrainChunkCache::AddChunk(void* data)
{
	return this->vertexBufferPool->Alloc(data);
}

void 
TerrainChunkCache::FreeChunk(DWORD offset)
{
    this->vertexBufferPool->Free(offset);
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainChunkCache::ApplyCache()
{
	if (this->vertexBufferPool.isvalid())
	{
		RenderDevice::Instance()->SetVertexBuffer(this->vertexBufferPool->GetBuffer());   
	}
	if (this->indexBuffer.isvalid())
	{
		RenderDevice::Instance()->SetIndexBuffer(this->indexBuffer);
	}
}

//------------------------------------------------------------------------------
/**
	这里可以建lod
*/
void
TerrainChunkCache::CreateIndexBuffer()
{
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			indexBufferData[(j*8+i)*12+0] = j*9+j*8+i;
			indexBufferData[(j*8+i)*12+1] = (j+1)*9+j*8+i;
			indexBufferData[(j*8+i)*12+2] = (j+1)*9+(j+1)*8+i;

			indexBufferData[(j*8+i)*12+3] = j*9+j*8+i;
			indexBufferData[(j*8+i)*12+4] = j*9+j*8+i+1;
			indexBufferData[(j*8+i)*12+5] = (j+1)*9+j*8+i;

			indexBufferData[(j*8+i)*12+6] = (j+1)*9+j*8+i;
			indexBufferData[(j*8+i)*12+7] = j*9+j*8+i+1;
			indexBufferData[(j*8+i)*12+8] = (j+1)*9+(j+1)*8+i+1;

			indexBufferData[(j*8+i)*12+9] = (j+1)*9+(j+1)*8+i;
			indexBufferData[(j*8+i)*12+10] = (j+1)*9+j*8+i;
			indexBufferData[(j*8+i)*12+11] = (j+1)*9+(j+1)*8+i+1;
		}
	}

	//LOD
	/*indexbuf[(j*8)*6+0] = j*9+j*8+i;
	indexbuf[(j*8)*6+1] = (j+1)*9+(j+1)*8+i;
	indexbuf[(j*8)*6+2] = j*9+j*8+i+1;

	indexbuf[(j*8)*6+3] = (j+1)*9+(j+1)*8+i+1;
	indexbuf[(j*8)*6+4] = j*9+j*8+i+1;
	indexbuf[(j*8)*6+5] = (j+1)*9+(j+1)*8+i;
	*/
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainChunkCache::CreateChunkCache(SizeT chunkNum)
{
	if (this->vertexComponents.Size() == 0)
	{
		this->vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
		this->vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
		this->vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
		this->vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 1, VertexComponent::Float2));
	}

	this->vertexBufferPool = VertexBufferPool::Create();
	this->vertexBufferPool->Reset(sizeof(TerrainChunkFVF), 8*8+9*9, chunkNum, this->vertexComponents);

	this->indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, samplerstripsize, indexBufferData, sizeof(uint16)*samplerstripsize);
	this->indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->indexBuffer->Load();
	this->indexBuffer->SetLoader(0);
	n_assert(this->indexBuffer->GetState() == IndexBuffer::Loaded);

	/*Ptr<Mesh> mesh = SharedResourceServer::Instance()->CreateSharedResource(meshName, Mesh::RTTI, StreamMeshLoader::RTTI).downcast<Mesh>();
	mesh->SetState(Resource::Loaded);
	mesh->SetVertexBuffer(vertexBufferPool->GetBuffer());
	mesh->SetIndexBuffer(indexBuffer);*/
}

} // namespace WOW