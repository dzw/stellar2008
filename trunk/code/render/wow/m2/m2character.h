#pragma once
#ifndef WOW_M2CHARACTER_H
#define WOW_M2CHARACTER_H
//------------------------------------------------------------------------------
/**
*/
#include "core/refcounted.h"
#include "wow/m2/m2charskeleton.h"
#include "foundation/timing/time.h"
#include "wow/m2/animation/animmanage.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2Character : public Core::RefCounted
{
    DeclareClass(M2Character);
public:
    /// constructor
    M2Character();
    /// destructor
    virtual ~M2Character();

	M2CharSkeleton& GetSkeleton();
	void EvaluateSkeleton(Timing::Time time, bool enforceEvaluation = false);
	void Set(const Ptr<M2Character>& other);

	void SetAniManager(unsigned char* ptr, int num);
	AnimManager* GetAniManager();
	ModelAnimation* GetModelAnimation();
private:
	M2CharSkeleton charSkeleton;
	//Ptr<nAnimClipScheduler> clipScheduler;
	AnimManager *animManager;
	ModelAnimation* anims;
};

//------------------------------------------------------------------------------
/**
*/
inline AnimManager* 
M2Character::GetAniManager()
{
	return this->animManager;
}

//------------------------------------------------------------------------------
/**
*/
inline ModelAnimation* 
M2Character::GetModelAnimation()
{
	return this->anims;
}

//------------------------------------------------------------------------------
/**
*/
inline M2CharSkeleton&
M2Character::GetSkeleton()
{
	return this->charSkeleton;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
M2Character::Set(const Ptr<M2Character>& other)
{
	charSkeleton = other->GetSkeleton();
	animManager = other->GetAniManager();
	anims = other->GetModelAnimation();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    