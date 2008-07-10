//------------------------------------------------------------------------------
//  managedmodel.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainchunkStorage.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainChunkCache, 'MDCC', Core::RefCounted);

TerrainChunkCache::TerrainChunkCache()
{
}

TerrainChunkCache::~TerrainChunkCache()
{
}

void 
TerrainChunkCache::Alloc(SizeT num)
{
    // setup new vertex buffer
	this->vertexBuffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(this->vertexComponents, this->header->nVertices, dataPtr, this->header->nVertices * sizeof(vfvf));
	this->vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->vertexBuffer->Load();
	this->vertexBuffer->SetLoader(0);
	delete[] dataPtr;
	n_assert(this->vertexBuffer->GetState() == VertexBuffer::Loaded);

    this->indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, nIndices, indices, sizeof(uint16)*nIndices);
	this->indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->indexBuffer->Load();
	this->indexBuffer->SetLoader(0);
	n_delete_array(indices);
	n_assert(this->indexBuffer->GetState() == IndexBuffer::Loaded);

    Ptr<Mesh> mesh = SharedResourceServer::Instance()->CreateSharedResource(meshName, Mesh::RTTI, StreamMeshLoader::RTTI).downcast<Mesh>();
	mesh->SetState(Resource::Loaded);
	mesh->SetVertexBuffer(this->vertexBuffer);
	mesh->SetIndexBuffer(this->indexBuffer);
	//mesh->SetPrimitiveGroups(this->primGroups);
}

/// 清空
void 
TerrainChunkCache::Dealloc()
{
}

/// 重新初始化
void 
TerrainChunkCache::Realloc()
{
}

/// 增加一个chunk
DWORD 
TerrainChunkCache::AddChunk(void *data)
{
}

Ptr<Mesh> 
TerrainChunkCache::GetMesh()
{
}

} // namespace WOW