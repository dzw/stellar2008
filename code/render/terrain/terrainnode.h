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
	virtual void ApplySharedState();
    virtual Resources::Resource::State GetResourceState() const;
	virtual void LoadResource();

	/// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;
	void SetMesh(const Ptr<CoreGraphics::Mesh>& mesh);

	/// ��terrainnodeinstance�е���������Ⱦ
	void Render();
	TerrainChunkFVF* GetVertexData();
	void SetVertexOffsetInCache(SizeT offset);
	void AddToRender();

	static void InitGlobalVBOs();

	/// set resource data
    void SetResource(const Ptr<IO::Stream>& s, SizeT offset);
	void ParseData();
protected:
	const String& CreateBlendTexture(void* srcData, SizeT srcNum);

    

    Math::vector posBase;
    int x, z;
	DWORD areaId;

    Util::String shaderName;    // shd:terrain
    int animated[4];            // �Ĳ������Ƿ��ж���
	int nTextures;

    TerrainChunkFVF *dataBuf;	// size=9*9+8*8

	unsigned char* blendbuf;	// TexBlend0	size=64*64*4
	
    bool loaded;                  // ���ݼ������

	// render
	Ptr<CoreGraphics::Mesh> mesh;
	IndexT primGroupIndex;
	CoreGraphics::PrimitiveGroup primGroup;
	/// ָ��chunk���ݴ�ŵĵ�ַ
	SizeT headerOffset;
	Ptr<IO::Stream> stream;
	
	static bool coordCreated;
	static Math::float2 texCoord[mapbufsize];
	static Math::float2 alphaCoord[mapbufsize];
};

Resources::Resource::State 
TerrainNode::GetResourceState() const
{
    return loaded?Resources::Resource::Loaded:Resources::Resource::Initial;
}

void 
TerrainNode::SetMesh(const Ptr<CoreGraphics::Mesh>& mesh)
{
	this->mesh = mesh;
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
}

} // namespace Models
//------------------------------------------------------------------------------
#endif