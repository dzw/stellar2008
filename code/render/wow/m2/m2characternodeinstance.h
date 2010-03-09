#pragma once
#ifndef WOW_M2CHARACTERNODEINSTANCE_H
#define WOW_M2CHARACTERNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class WOW::M2CharacterNodeInstance
  
    (C) 2010 cTuo
*/
#include "models/nodes/transformnodeinstance.h"
#include "wow/m2/animmanager.h"

//------------------------------------------------------------------------------
namespace WOW
{
struct JointInstance
{
	Math::matrix44 mat;
	Math::matrix44 mrot;
};

class M2CharacterNodeInstance : public Models::TransformNodeInstance
{
    DeclareClass(M2CharacterNodeInstance);
public:
	

    /// constructor
    M2CharacterNodeInstance();
    /// destructor
    virtual ~M2CharacterNodeInstance();
    
    /// perform per-frame updates
    virtual void Update();    
	/// get joint data
	const Util::FixedArray<JointInstance> GetJointData()const;
	///
	void SetCurAnimID(int anim, int fadeout = 500);
	void SetAnimManager(AnimManager* m);
	const AnimManager* GetAnimManager()const;
	void SetSecondaryAnimID(int anim);
protected:
    /// called when attached to ModelInstance
	virtual void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void OnRemoveFromModelInstance();
    /// render node specific debug shape
    virtual void RenderDebug();
    
	Util::FixedArray<JointInstance> joints;

	/// µ±Ç°¶¯»­ID
	int curAnim;
	AnimManager* animManager;
};

//------------------------------------------------------------------------------
/**
*/
inline const Util::FixedArray<JointInstance> 
M2CharacterNodeInstance::GetJointData()const
{
	return this->joints;
}

inline void 
M2CharacterNodeInstance::SetCurAnimID(int anim, int fadeout)
{
	//if (animManager != 0)
	//	animManager->Set(0, anim, true);
	if (animManager != 0)
		animManager->CreateAction(anim, fadeout);
}

inline void
M2CharacterNodeInstance::SetSecondaryAnimID(int anim)
{
	if (animManager != 0)
		animManager->SetSecondary(anim);
}

inline void 
M2CharacterNodeInstance::SetAnimManager(AnimManager* m)
{
	animManager = m;
}

inline const AnimManager* 
M2CharacterNodeInstance::GetAnimManager()const
{
	return animManager;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

