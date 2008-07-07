#pragma once
#ifndef WOW_TERRAIN_H
#define WOW_TERRAIN_H
//------------------------------------------------------------------------------
/**
    
*/
#include "models/model.h"
#include "wow/world/Terrain/maptile.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/primitivegroup.h"
#include "resources/managedmesh.h"

#include "coregraphics/d3d9/d3d9renderdevice.h"

//------------------------------------------------------------------------------
namespace Models
{

class TerrainChunk : public Model
{
    DeclareClass(TerrainChunk);
public:
    /// constructor
    TerrainChunk();
    /// destructor
    virtual ~TerrainChunk();

	bool UpdateChunks(const ChunkData& data);
	bool CreateChunkNode(const ChunkData& chunk);

	///
	IndexT GetChunkId()const;
	///
	void SetChunkId(IndexT id);
    ///
    bool NeedUpdate()const;
    ///
    void SetNeedUpdate(bool update);
private:
	const Util::String CreateTextureManual(const void* srcData, int srcDataSize);
	const Util::String UpdateVertexBuffer(const Ptr<Resources::ManagedMesh>& managMesh, const TerrainChunkFVF* dataPtr, int vertices);
	void CreateMesh(Ptr<CoreGraphics::Mesh>& mesh);

	void ReadPrimitiveGroups();
	void SetupVertexComponents();
	void SetupIndexBuffer();
	int IndexMapBuf(int x, int y);

	/// 所有chunk共享的数据，只创建一次
	static Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	static Util::Array<CoreGraphics::PrimitiveGroup> primGroups;

	/// 
	IndexT chunkId;

    IDirect3DTexture9* d3d9Texture;
    /// 更新chunk标志
    bool needUpdate;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
TerrainChunk::NeedUpdate()const
{
    return this->needUpdate;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TerrainChunk::SetNeedUpdate(bool update)
{
    this->needUpdate = update;
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT 
TerrainChunk::GetChunkId()const
{
	return this->chunkId;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TerrainChunk::SetChunkId(IndexT id)
{
	this->chunkId = id;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    