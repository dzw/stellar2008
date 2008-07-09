#pragma once
#ifndef TERRAIN_TERRAINNODE_H
#define TERRAIN_TERRAINNODE_H
//------------------------------------------------------------------------------
/**
	������Ⱦ��������Ļ��棨���㡢�����ȣ���ÿ��TerrainNodeInstance����һ�����ο�
	������Ⱦ
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

protected:
	Ptr<TerrainChunkCache> cache;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
