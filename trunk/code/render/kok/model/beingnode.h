#pragma once
#ifndef KOK_BEINGNODE_H
#define KOK_BEINGNODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::BeingNode

	骨格动画需要重新计算生成shader版的顶点格式，产生矩阵模板,nodeInstance需在从骨骼框架中取得变换矩阵！

	KokShapeNode::skinWeights保存的顺序就是骨骼矩阵模板，顶点也是根据这个设置。

    (C) 2008 cTuo
*/    
#include "kok/model/kokshapenode.h"

//------------------------------------------------------------------------------
namespace KOK
{
class BeingNode : public KokShapeNode
{
    DeclareClass(BeingNode);
public:
    /// constructor
    BeingNode();
    /// destructor
    virtual ~BeingNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();
	/// called when resources should be loaded
    virtual void LoadResources();

	void LoadFromStream(const Ptr<IO::Stream>& stream, 
		int iVersion, bool bMirrorZ, bool bCompuiteDuplicateVertexCollection=false, bool bBlend=false);
	void CreateVertexBuffer(const Ptr<IO::Stream>& stream, bool bMirrorZ);

	virtual void LoadTextures(const Util::String& path, int texId);
protected:
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance()const;
	virtual void UnloadResources();
	void CreateMesh();
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
