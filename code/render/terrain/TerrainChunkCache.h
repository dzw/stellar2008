#pragma once
#ifndef TERRAIN_TERRAINCHUNKSTORAGE_H
#define TERRAIN_TERRAINCHUNKSTORAGE_H
//------------------------------------------------------------------------------
/**
    @class Terrain::ManagedWorld
    
	����������Ⱦ�õ���chunk�����ݻ��壨�綥�㣩��������Ⱦ��������ܸ����������
	�������������ʾ��chunk����������µ�chunkʱ��Ҫ����������л��п��ÿռ䣬
	��ֱ����ӣ�������Ҫ�ѿ�������chunk��ɾ���ˣ�����ӡ�

	�����壬�ڻ��������\ɾ��chunk
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
	/// ����һ��chunk
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
