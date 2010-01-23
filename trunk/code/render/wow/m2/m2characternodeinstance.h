#pragma once
#ifndef WOW_M2CHARACTERNODEINSTANCE_H
#define WOW_M2CHARACTERNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class Models::M2CharacterNodeInstance
  
    The M2CharacterNodeInstance actually renders a skinned shape, and holds
    all the necessary per-instance state to do so.

    It knows the selected visible skins an active textures for
    actual rendering.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/nodes/transformnodeinstance.h"

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
	void SetCurAnimID(int anim);
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
M2CharacterNodeInstance::SetCurAnimID(int anim)
{
	curAnim = anim;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

