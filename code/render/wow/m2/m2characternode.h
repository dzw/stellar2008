#pragma once
#ifndef MODELS_M2CHARACTERNODE_H
#define MODELS_M2CHARACTERNODE_H
//------------------------------------------------------------------------------
/**
    @class Models::M2CharacterNode
*/    
#include "models/nodes/characternode.h"
#include "wow/m2/m2character.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2CharacterNode : public Models::CharacterNode
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
	
	/// begin adding joints
	void BeginJoints(int numJoints);
	/// add a joint to the skeleton
	void SetJoint(int index, unsigned char *f, ModelBoneDef &b, int *global);
	/// finish adding joints
	void EndJoints();
	/// get number of joints in skeleton
	int GetNumJoints();
	/// get joint attributes
	void GetJoint(int index, int& parentJointIndex, Math::vector& poseTranslate, Math::quaternion& poseRotate, Math::vector& poseScale, Util::String& name);


	/// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;

	const Ptr<M2Character>& GetCharacter() const;

	void SetAniManager(unsigned char* ptr, int num);
private:
	Ptr<M2Character> character;
	
};

//------------------------------------------------------------------------------
/**  
*/
inline void 
M2CharacterNode::SetAniManager(unsigned char* ptr, int num)
{
	this->character->SetAniManager(ptr, num);
}

//------------------------------------------------------------------------------
/**  
*/
inline const Ptr<M2Character>& 
M2CharacterNode::GetCharacter() const
{
	return this->character;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
M2CharacterNode::BeginJoints(int numJoints)
{
	this->character->GetSkeleton().BeginJoints(numJoints);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
M2CharacterNode::SetJoint(int index, unsigned char *f, ModelBoneDef &b, int *global)
{
	this->character->GetSkeleton().SetJoint(index, f, b, global);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
M2CharacterNode::EndJoints()
{

}

//------------------------------------------------------------------------------
/**
*/
inline int 
M2CharacterNode::GetNumJoints()
{
	return this->character->GetSkeleton().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
M2CharacterNode::GetJoint(int index, int& parentJointIndex, Math::vector& poseTranslate, Math::quaternion& poseRotate, Math::vector& poseScale, Util::String& name)
{

}

} // namespace Models
//------------------------------------------------------------------------------
#endif
