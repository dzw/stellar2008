#pragma once
#ifndef TERRAIN_TERRAINCHUNKSTORAGE_H
#define TERRAIN_TERRAINCHUNKSTORAGE_H
//------------------------------------------------------------------------------
/**
    @class Terrain::ManagedWorld
    
	管理所有渲染用地形chunk的数据缓冲（如顶点），用于渲染，这里可能根据摄像机离
	地面距离设置显示的chunk数量，添加新的chunk时需要，如果缓冲中还有可用空间，
	就直接添加，否则需要把看不见的chunk都删除了，再添加。

	管理缓冲，在缓冲中添加\删除chunk
    (C) cTuo
*/
#include "core/refcounted.h"
#include "terrain/terraindef.h"
#include "coregraphics/vertexbufferpool.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/vertexcomponent.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainChunkCache : public Core::RefCounted
{
    DeclareClass(TerrainChunkCache);
public:
    TerrainChunkCache();
    ~TerrainChunkCache();

	void ApplyCache();
	void CreateChunkCache(SizeT chunkNum);
	/// 增加一个chunk
	DWORD AddChunk(void *data);
    void FreeChunk(DWORD offset);

protected:
	void CreateIndexBuffer();

	Ptr<CoreGraphics::VertexBufferPool> vertexBufferPool;
	Ptr<CoreGraphics::IndexBuffer> indexBuffer;
	Util::Array<CoreGraphics::VertexComponent> vertexComponents;

	uint16 indexBufferData[samplerstripsize];
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
