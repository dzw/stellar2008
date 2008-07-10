#pragma once
#ifndef TERRAIN_TERRAINCHUNKENTITY_H
#define TERRAIN_TERRAINCHUNKENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::TerrainChunk
    
    一个地形块
    
    (C) 2007 Radon Labs GmbH
*/
#include "resources/resourceid.h"
#include "Terrain/managedchunkcacha.h"
#include "terrain/worldserver.h"
//#include "wow/world/Terrain/managedchunk.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainChunk : public Core::RefCounted
{
    DeclareClass(TerrainChunk);
public:
    /// constructor
    TerrainChunk();
    /// destructor
    virtual ~TerrainChunk();

    void Init(const Ptr<Stream>& stream, const Ptr<TerrainTile>& tile);

	/// 在terrainnodeinstance中调用设置渲染
	void Render();
	TerrainChunkFVF* GetVertexData();
	void SetVertexOffsetInCache(SizeT offset);

protected:
    Ptr<CoreGraphics::Mesh> mesh;
	IndexT primGroupIndex;

    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
    /// 前四个代表地形纹理，第五个是混合纹理
    Ptr<CoreGraphics::ShaderVariable> diffMap[5];
	Ptr<Resources::ManagedTexture> tex[5];

    Math::bbox box;
    Math::vector posBase;
    int x, z;

    Util::String shaderName;    // shd:terrain

    TerrainChunkFVF* dataBuf;	// size=9*9+8*8
	int dataSize;

	unsigned char* blendbuf;	// TexBlend0	size=64*64*4
	int bufSize;
	bool isEmpty;				// 有时候没有混合纹理，需要用一张空纹理代替，避免shader出错，这个标志共享同一张空纹理，不要重复建立

    bool isOk;                  // 数据加载完成

	// render
	CoreGraphics::PrimitiveGroup primGroup;
};

//------------------------------------------------------------------------------
/**
*/
inline TerrainChunkFVF* 
TerrainChunk::GetVertexData()
{
	return this->dataBuf;
}

inline void 
TerrainChunk::SetVertexOffsetInCache(SizeT offset)
{
	this->primGroup.SetBaseVertex(offset);
	this->primGroup.SetNumVertices(8*8+9*9);
	this->primGroup.SetBaseIndex(0);
	this->primGroup.SetNumIndices(samplerstripsize);
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    