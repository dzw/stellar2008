#pragma once
#ifndef KOK_EQUIPNODE_H
#define KOK_EQUIPNODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::EquipNode

    (C) 2008 cTuo
*/    
#include "kok/model/kokshapenode.h"
#include "kok/model/animation/cSkeletonHierarchy.h"

//------------------------------------------------------------------------------
namespace KOK
{
class EquipNode : public KokShapeNode
{
    DeclareClass(EquipNode);
public:
    /// constructor
    EquipNode();
    /// destructor
    virtual ~EquipNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();
	/// called when resources should be loaded
    virtual void LoadResources();

	void LoadFromStream(const Ptr<IO::Stream>& stream, 
		int iVersion, bool bMirrorZ, bool bCompuiteDuplicateVertexCollection=false, bool bBlend=false);

	virtual void LoadTextures(const Util::String& path, int texId);
	/// �������ӵ�任
	Math::matrix44 GetTransform();
protected:
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance()const;
	virtual void UnloadResources();
	void CreateMesh();

	/// ��ģ�Ͷ�Ӧ�Ĺ���,����ÿ�β��ҷ�ʱ��
	cSkeletonHierarchy* skeleton;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
