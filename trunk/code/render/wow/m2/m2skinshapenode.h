#pragma once
#ifndef WOW_M2SKINSHAPENODE_H
#define WOW_M2SKINSHAPENODE_H
//------------------------------------------------------------------------------
/**
	每个m2skinshapenode只有一个批次，批次编号可通过Attr::MeshGroupIndex获得
*/    
#include "models/nodes/shapenode.h"
#include "wow/m2/modelheaders.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2SkinShapeNode : public Models::ShapeNode
{
    DeclareClass(M2SkinShapeNode);
public:
    /// constructor
    M2SkinShapeNode();
    /// destructor
    virtual ~M2SkinShapeNode();

    /// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;    
    /// apply state shared by all my SkinShapeNodeInstances
    virtual bool ApplySharedState();

    int GetNumJoints() const;
	void SetJointArray(Util::Array<int> j);
	const Util::Array<int>& GetJointArray()const;

	void SetGeoset(const ModelGeoset& g);
	const ModelGeoset& GetGeoset()const;
protected:   
    /// called when resources should be loaded
    virtual void LoadResources();

private: 
    Util::Array<int> jointArray;
	ModelGeoset geoset;
};

//------------------------------------------------------------------------------
/**
    Get number of fragments.
*/
inline int
M2SkinShapeNode::GetNumJoints() const
{
    return this->jointArray.Size();
}

inline void 
M2SkinShapeNode::SetJointArray(Util::Array<int> j)
{
	jointArray.Clear();
	jointArray = j;
}

inline const Util::Array<int>& 
M2SkinShapeNode::GetJointArray()const
{
	return jointArray;
}

inline void 
M2SkinShapeNode::SetGeoset(const ModelGeoset& g)
{
	Memory::Copy(&g, &geoset, sizeof(ModelGeoset));
}

inline const ModelGeoset& 
M2SkinShapeNode::GetGeoset()const
{
	return geoset;
}

}
//------------------------------------------------------------------------------
#endif
