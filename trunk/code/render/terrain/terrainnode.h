#pragma once
#ifndef TERRAIN_TERRAINNODE_H
#define TERRAIN_TERRAINNODE_H
//------------------------------------------------------------------------------
/**
	������Ⱦ��������Ļ��棨���㡢�����ȣ���ÿ��TerrainNodeInstance����һ�����ο�
	������Ⱦ

	������Ⱦ��Ҫ��������
*/
#include "models/modelnode.h"

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

	/// create a model node instance
	virtual Ptr<ModelNodeInstance> CreateNodeInstance() const;
	void SetMesh(const Ptr<Mesh>& mesh);

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

    TerrainChunkFVF dataBuf[8*8+9*9];	// size=9*9+8*8
	int dataSize;

	unsigned char* blendbuf;	// TexBlend0	size=64*64*4
	int bufSize;
	bool isEmpty;				// ��ʱ��û�л��������Ҫ��һ�ſ�������棬����shader���������־����ͬһ�ſ�������Ҫ�ظ�����

    bool isOk;                  // ���ݼ������

	// render
	CoreGraphics::PrimitiveGroup primGroup;
};

Resources::Resource::State 
TerrainNode::GetResourceState() const
{
    return isOk?Resources::Resource::Loaded:Resources::Resource::Initial;
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
