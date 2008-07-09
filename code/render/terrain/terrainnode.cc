//------------------------------------------------------------------------------
//  modelnode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainnode.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainNode, 'TNNE', Models::ModelNode);


//------------------------------------------------------------------------------
/**
*/
TerrainNode::TerrainNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TerrainNode::~TerrainNode()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainNode::ApplySharedState()
{
	// 提交缓冲中的顶点数据,在instance中使用这些数据渲染(参考ShapeNode::ApplySharedState)
	const Ptr<Mesh>& mesh = this->cache->GetMesh();
	mesh->ApplyPrimitives(this->primGroupIndex);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
TerrainNode::CreateNodeInstance() const
{
	Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) TerrainNodeInstance::Create();
	return newInst;
}

} // namespace Models