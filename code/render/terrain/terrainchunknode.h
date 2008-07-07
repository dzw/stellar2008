#pragma once
#ifndef TERRAIN_TERRAINCHUNKNODE_H
#define TERRAIN_TERRAINCHUNKNODE_H
//------------------------------------------------------------------------------
/**
	一个地形块数据，引用一个动态顶点缓冲，保存所有需要渲染的地形块顶点数据

    cTuo
*/
#include "models/nodes/transformnode.h"
#include "resources/managedmesh.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shadervariable.h"
#include "resources/managedtexture.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainChunkNode : public TransformNode
{
    DeclareClass(TerrainChunkNode);
public:
    /// constructor
    TerrainChunkNode();
    /// destructor
    virtual ~TerrainChunkNode();

	///
	void Update();
	///
	const Ptr<Resources::ManagedMesh>& GetManagedMesh()const;
protected:
    /// create a model node instance
    virtual Ptr<ModelNodeInstance> CreateNodeInstance() const;
    /// called to save state back to attributes
    virtual void SaveToAttrs(Attr::AttributeContainer& attrs);
	///
	virtual void LoadResources();
	///
	virtual void UnloadResources();
	///
	virtual void ApplySharedState();

	Ptr<Resources::ManagedMesh> managedMesh;
	IndexT primGroupIndex;

    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
    /// 前四个代表地形纹理，第五个是混合纹理
    Ptr<CoreGraphics::ShaderVariable> diffMap[5];
	Ptr<Resources::ManagedTexture> tex[5];
};

inline const Ptr<Resources::ManagedMesh>& 
TerrainChunkNode::GetManagedMesh()const
{
	return this->managedMesh;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    