#pragma once
#ifndef MODELS_CHUNKNODE_H
#define MODELS_CHUNKNODE_H
//------------------------------------------------------------------------------
/**
    cTuo
*/
#include "models/nodes/transformnode.h"
#include "resources/managedmesh.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shadervariable.h"
#include "resources/managedtexture.h"

//------------------------------------------------------------------------------
namespace Models
{
class ChunkNode : public TransformNode
{
    DeclareClass(ChunkNode);
public:
    /// constructor
    ChunkNode();
    /// destructor
    virtual ~ChunkNode();

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
	virtual bool ApplySharedState();

	Ptr<Resources::ManagedMesh> managedMesh;
	IndexT primGroupIndex;

    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
    /// 前四个代表地形纹理，第五个是混合纹理
    Ptr<CoreGraphics::ShaderVariable> diffMap[5];
	Ptr<Resources::ManagedTexture> tex[5];
};

inline const Ptr<Resources::ManagedMesh>& 
ChunkNode::GetManagedMesh()const
{
	return this->managedMesh;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    