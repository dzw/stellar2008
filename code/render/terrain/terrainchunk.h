#pragma once
#ifndef TERRAIN_TERRAINCHUNKENTITY_H
#define TERRAIN_TERRAINCHUNKENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::TerrainChunk
    
    һ�����ο�
    
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

	/// ��terrainnodeinstance�е���������Ⱦ
	void Render();
	TerrainChunkFVF* GetVertexData();
	void SetVertexOffsetInCache(SizeT offset);

protected:
    Ptr<CoreGraphics::Mesh> mesh;
	IndexT primGroupIndex;

    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
    /// ǰ�ĸ������������������ǻ������
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
	bool isEmpty;				// ��ʱ��û�л��������Ҫ��һ�ſ�������棬����shader���������־����ͬһ�ſ�������Ҫ�ظ�����

    bool isOk;                  // ���ݼ������

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
    