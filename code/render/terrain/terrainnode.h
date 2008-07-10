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
class TerrainNode : public Models::ModelNode
{
    DeclareClass(TerrainNode);
public:
    /// constructor
    TerrainNode();
    /// destructor
    virtual ~TerrainNode();

	/// apply state shared by all my ModelNodeInstances
	virtual void ApplySharedState();
	/// create a model node instance
	virtual Ptr<ModelNodeInstance> CreateNodeInstance() const;
	void SetMesh(const Ptr<Mesh>& mesh);

protected:
	Ptr<CoreGraphics::Mesh> mesh;
};

void 
TerrainNode::SetMesh(const Ptr<CoreGraphics::Mesh>& mesh)
{
	this->mesh = mesh;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
