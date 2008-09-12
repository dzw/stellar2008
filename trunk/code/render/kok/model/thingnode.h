#pragma once
#ifndef KOK_THINGNODE_H
#define KOK_THINGNODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::ThingNode

	处理摇动的地物，和特殊的效果

    (C) 2008 cTuo
*/    
#include "kok/model/kokshapenode.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ThingNode : public KokShapeNode
{
    DeclareClass(ThingNode);
public:
    /// constructor
    ThingNode();
    /// destructor
    virtual ~ThingNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();
	/// called when resources should be loaded
    virtual void LoadResources();

	void LoadFromStream(const Ptr<IO::Stream>& stream, 
		int iVersion, bool bMirrorZ, bool bCompuiteDuplicateVertexCollection=false, bool bBlend=false);
	
	int GetCullMode()const;
	int GetSrcBlend()const;
	int GetDestBlend()const;
protected:
	friend class ThingInstance;

	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance()const;
	virtual void UnloadResources();
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
