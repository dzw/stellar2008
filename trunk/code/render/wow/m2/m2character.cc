//------------------------------------------------------------------------------
//  m2character.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2character.h"


namespace WOW
{
ImplementClass(WOW::M2Character, 'M2CR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
M2Character::M2Character() :
animManager(0),
anims(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
M2Character::~M2Character()
{
	if (anims != NULL)
	{
		n_delete_array(anims);
		anims = NULL;
	}
	if (animManager != NULL)
	{
		n_delete(animManager);
		animManager = NULL;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
M2Character::EvaluateSkeleton(Timing::Time time, bool enforceEvaluation)
{
	int t = 0;
	if (animManager != NULL)
	{
		int ddt = 0;
		static Timing::Time lastTime = 0;

		//Timing::Time curTime = BaseGameFeature::SystemTimeSource::Instance()->GetTime();
		
		ddt = (time - lastTime)*1000;
		lastTime = time;

		animManager->Tick(ddt);
		t = animManager->GetFrame();
		
	}

	charSkeleton.Evaluate(0, t/*(int)time*/);
}

//------------------------------------------------------------------------------
/**
*/
void 
M2Character::SetAniManager(unsigned char* ptr, int num)
{
	anims = n_new_array(ModelAnimation, num);
	memcpy(anims, ptr, num * sizeof(ModelAnimation));
	animManager = n_new(AnimManager(anims));
}

} // namespace Models
