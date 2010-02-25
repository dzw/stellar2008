#pragma once
#ifndef MODELS_M2CHARACTERNODE_H
#define MODELS_M2CHARACTERNODE_H
//------------------------------------------------------------------------------
/**
    @class Models::M2CharacterNode
*/    
#include "models/nodes/transformnode.h"
#include "wow/m2/m2charjoint.h"
#include "wow/m2/animmanager.h"
#include "wow/m2/modelheaders.h"
#include "wow/m2/enums.h"

namespace Models
{
class M2ModelReader;
}

//------------------------------------------------------------------------------
namespace WOW
{
class M2CharacterNode : public Models::TransformNode
{
    DeclareClass(M2CharacterNode);
public:
    /// constructor
    M2CharacterNode();
    /// destructor
    virtual ~M2CharacterNode();

    /// called when resources should be loaded
    virtual void LoadResources();
    /// called when resources should be unloaded
    virtual void UnloadResources();

	/// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;

	const Util::FixedArray<M2CharJoint>& GetJoints()const;
	///
	void UpdataBones(int time);
protected:
	friend class Models::M2ModelReader;

	/// update bones
	void CalcBones(int anim, int time);

	Util::FixedArray<M2CharJoint> bones;
	ModelAnimation* anims;
	AnimManager* animManager;
	int nAnimationLookup;
	int16 keyBoneLookup[BONE_MAX];
	Util::FixedArray<int> animLookups;
};

inline const Util::FixedArray<M2CharJoint>& 
M2CharacterNode::GetJoints()const
{
	return bones;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
