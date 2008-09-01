#pragma once
#ifndef KOK_BEINGNODE_H
#define KOK_BEINGNODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::BeingNode

	�Ǹ񶯻���Ҫ���¼�������shader��Ķ����ʽ����������ģ��,nodeInstance���ڴӹ��������ȡ�ñ任����

	KokShapeNode::skinWeights�����˳����ǹ�������ģ�壬����Ҳ�Ǹ���������á�

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
