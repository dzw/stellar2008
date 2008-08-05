#pragma once
#ifndef TERRAIN_TERRAINNODE_H
#define TERRAIN_TERRAINNODE_H
//------------------------------------------------------------------------------
/**
	������Ⱦ��������Ļ��棨���㡢�����ȣ���ÿ��TerrainNodeInstance����һ�����ο�
	������Ⱦ

	������Ⱦ��Ҫ��������
*/
#include "models/nodes/statenode.h"
#include "Terrain/terraindef.h"
#include "coregraphics/mesh.h"
#include "io/stream.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainNode : public Models::StateNode
{
    DeclareClass(TerrainNode);
public:
    /// constructor
    TerrainNode();
    /// destructor
    virtual ~TerrainNode();

	/// apply state shared by all my ModelNodeInstances
	virtual bool ApplySharedState();
    virtual Resources::Resource::State GetResourceState() const;
	virtual void LoadResource();

	/// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;

	/// ��terrainnodeinstance�е���������Ⱦ
	TerrainChunkFVF* GetVertexData();
	void SetVertexOffsetInCache(SizeT offset);
	void AddToRender();
    void FreeVertexOffset();

	static void InitGlobalVBOs();

	void ParseData(const Ptr<IO::Stream>& stream, SizeT offset);

    void CreateTemp();
protected:
    const Util::String CreateBlendTexture(void* srcData, SizeT srcNum);


    Math::vector posBase;
    int x, z;
	DWORD areaId;

    Util::String shaderName;    // shd:terrain
    int animated[4];            // �Ĳ������Ƿ��ж���
	int nTextures;
	int layer;

    TerrainChunkFVF *dataBuf;	// size=9*9+8*8
	unsigned char* blendbuf;	// TexBlend0	size=64*64*4

    bool loaded;                  // ���ݼ������
    bool loadDetail;              // ϸ�����ݼ���

	// render
	CoreGraphics::PrimitiveGroup primGroup;
    Ptr<CoreGraphics::Mesh> mesh;
    SizeT vertexOffset;

	static bool coordCreated;
	static Math::float2 texCoord[mapbufsize];
	static Math::float2 alphaCoord[mapbufsize];
};

inline Resources::Resource::State 
TerrainNode::GetResourceState() const
{
    return loaded?Resources::Resource::Loaded:Resources::Resource::Initial;
}

//------------------------------------------------------------------------------
/**
*/
inline TerrainChunkFVF* 
TerrainNode::GetVertexData()
{
	return this->dataBuf;
}

inline void 
TerrainNode::SetVertexOffsetInCache(SizeT offset)
{
	this->primGroup.SetBaseVertex(offset);
	this->primGroup.SetNumVertices(8*8+9*9);
	this->primGroup.SetBaseIndex(0);
	this->primGroup.SetNumIndices(samplerstripsize);
    this->primGroup.SetPrimitiveTopology(CoreGraphics::PrimitiveTopology::TriangleList);
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
