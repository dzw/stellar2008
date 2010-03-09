//------------------------------------------------------------------------------
//  TransformNode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2characternode.h"
#include "wow/m2/m2characternodeinstance.h"

namespace WOW
{
ImplementClass(WOW::M2CharacterNode, 'M2CH', Models::TransformNode);

using namespace Util;
using namespace Attr;
using namespace Math;
using namespace Resources;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
M2CharacterNode::M2CharacterNode()
{
}

//------------------------------------------------------------------------------
/**
*/
M2CharacterNode::~M2CharacterNode()
{
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
M2CharacterNode::CreateNodeInstance() const
{    
	Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) M2CharacterNodeInstance::Create();
	newInst.downcast<M2CharacterNodeInstance>()->SetAnimManager(animManager);
	return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNode::LoadResources()
{
	// call parent class, creates mesh
	TransformNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNode::UnloadResources()
{
	// call parent class
	TransformNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void 
M2CharacterNode::CalcBones()
{
	AnimParam& animParam = animManager->GetAnimParam();
	int anim1		= animParam.actionIndex1;
	int animTime1	= animParam.animTime1;
	int anim2		= animParam.actionIndex2;
	int animTime2	= animParam.animTime2;
	float lerpValue = animParam.lerpValue;

	// Reset all bones to 'false' which means they haven't been animated yet.
	for (IndexT i=0; i<bones.Size(); i++) 
	{
		bones[i].calc = false;
	}

	// 是character模型才走这里
	// Character specific bone animation calculations.
	if (1/*charModelDetails.isChar*/) 
	{	
		// Animate the "core" rotations and transformations for the rest of the model to adopt into their transformations
		if (keyBoneLookup[BONE_ROOT] > -1)	
		{
			for (int i=0; i<=keyBoneLookup[BONE_ROOT]; i++) 
			{
				bones[i].Evaluate(&bones[0], anim1, animTime1, anim2, animTime2, lerpValue);
			}
		}

		// Find the close hands animation id
		int closeFistID = 0;
		
		// use animLookups to speedup
		if (nAnimationLookup >= ANIMATION_HANDSCLOSED && animLookups[ANIMATION_HANDSCLOSED] > 0) // closed fist
			closeFistID = animLookups[ANIMATION_HANDSCLOSED];

		// Animate key skeletal bones except the fingers which we do later.
		// -----
		int a, t;

		// if we have a "secondary animation" selected,  animate upper body using that.
		if (animManager->GetSecondaryID() > -1) {
			a = animManager->GetSecondaryID();
			t = animManager->GetSecondaryFrame();
		} else {
			a = anim1;
			t = animTime1;
		}

		for (size_t i=0; i<5; i++) { // only goto 5, otherwise it affects the hip/waist rotation for the lower-body.
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].Evaluate(&bones[0], a, t, anim2, animTime2, lerpValue);
		}

		if (animManager->GetMouthID() > -1) {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].Evaluate(&bones[0], animManager->GetMouthID(), animManager->GetMouthFrame(), anim2, animTime2, lerpValue);
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].Evaluate(&bones[0], animManager->GetMouthID(), animManager->GetMouthFrame(), anim2, animTime2, lerpValue);
		} else {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].Evaluate(&bones[0], a, t, anim2, animTime2, lerpValue);
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].Evaluate(&bones[0], a, t, anim2, animTime2, lerpValue);
		}

		// still not sure what 18-26 bone lookups are but I think its more for things like wrist, etc which are not as visually obvious.
		for (size_t i=18; i<BONE_MAX; i++) {
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].Evaluate(&bones[0], a, t, anim2, animTime2, lerpValue);
		}
		// =====

		
		
		if (0/*charModelDetails.closeRHand*/) {
			a = closeFistID;
			t = anims[closeFistID].timeStart+1;
		} else {
			a = anim1;
			t = animTime1;
		}

		for (unsigned int i=0; i<5; i++) {
			if (keyBoneLookup[BONE_RFINGER1 + i] > -1) 
				bones[keyBoneLookup[BONE_RFINGER1 + i]].Evaluate(&bones[0], a, t, anim2, animTime2, lerpValue);
		}

		if (0/*charModelDetails.closeLHand*/) {
			a = closeFistID;
			t = anims[closeFistID].timeStart+1;
		} else {
			a = anim1;
			t = animTime1;
		}

		for (unsigned int i=0; i<5; i++) {
			if (keyBoneLookup[BONE_LFINGER1 + i] > -1)
				bones[keyBoneLookup[BONE_LFINGER1 + i]].Evaluate(&bones[0], a, t, anim2, animTime2, lerpValue);
		}
	} else {
		for (int i=0; i<keyBoneLookup[BONE_ROOT]; i++) {
			bones[i].Evaluate(&bones[0], anim1, animTime1, anim2, animTime2, lerpValue);
		}

		// The following line fixes 'mounts' in that the character doesn't get rotated, but it also screws up the rotation for the entire model :(
		//bones[18].calcMatrix(bones, anim, time, false);

	}

	// Animate everything thats left with the 'default' animation
	for (IndexT i=0; i<bones.Size(); i++) {
		bones[i].Evaluate(&bones[0], anim1, animTime1, anim2, animTime2, lerpValue);
	}
}

void 
M2CharacterNode::UpdataBones(int time)
{
	animManager->Tick(time);
	//CalcBones(animManager->GetAnim(), animManager->GetFrame());
	CalcBones();
}

} // namespace Models