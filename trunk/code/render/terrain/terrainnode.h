#pragma once
#ifndef TERRAIN_TERRAINNODE_H
#define TERRAIN_TERRAINNODE_H
//------------------------------------------------------------------------------
/**
	保存渲染地形所需的缓存（顶点、索引等），每个TerrainNodeInstance代表一个地形块
	用于渲染

	设置渲染需要的总数据
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

    TerrainChunkFVF dataBuf[8*8+9*9];	// size=9*9+8*8
	int dataSize;

	unsigned char* blendbuf;	// TexBlend0	size=64*64*4
	int bufSize;
	bool isEmpty;				// 有时候没有混合纹理，需要用一张空纹理代替，避免shader出错，这个标志共享同一张空纹理，不要重复建立

    bool isOk;                  // 数据加载完成

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
