#pragma once
#ifndef FIGHTER_FHERO_H
#define FIGHTER_FHERO_H
//------------------------------------------------------------------------------
/**
	帧数低的时候卡的历害，走路会跳帧！！！

	主角，唯一

	对象层次关系:
	object
	  npc 
		monster
		player
		  hero
*/
#include "fplayer.h"
#include "graphics/cameraentity.h"
#include "math/pfeedbackloop.h"
#include "math/angularpfeedbackloop.h"
#include "util/queue.h"

//------------------------------------------------------------------------------
namespace Fighter
{
/// wow中用的ID，临时的，可用名字代替
enum AnimID
{
	AID_Idle		= 50,
	AID_Walk		= 54,
	AID_Run			= 53,
	AID_RunAttack	= 4,
	AID_JumpStart	= 61,
	AID_Jump		= 64,
	AID_JumpEnd		= 63, 
	AID_Attack		= 21,
	AID_Defend		= 20,
};

class FHero : public FPlayer
{
	DeclareClass(FHero);
public:
    /// constructor
    FHero();
    /// destructor
    virtual ~FHero();
	
	virtual void Init();
	virtual void Clear();
	virtual void Update();
	void SetPosition(const Math::vector& dir, float dist=1.0f);
	void SetDirection(const Math::vector& dir);
	void SetSpeed(float f);
	virtual void SetCurrentAnimation(int id, DWORD fadeout=500);
	void SetAttachAnimation(int id);
	void SetSecondAnimation(int id);

	void Walk(const Math::vector& dir, const Util::String& animName);
	void Run(const Math::vector& dir, const Util::String& animName);
	void Jump(const Math::vector& dir, const Util::String& animName, bool isRuning);
	void Defend(const Math::vector& dir, const Util::String& animName);
	void Attack(const Math::vector& dir, const Util::String& animName);
	void Move(float dist);
protected:
	void NextAnim(BYTE action);
	
	Ptr<Graphics::CameraEntity> camera;
	Math::PFeedbackLoop<Math::vector> smoothedPosition;
	Math::AngularPFeedbackLoop smoothedHeading;
	Math::vector lookatDirection;
	int curAnim;
	Math::vector curDir;
	//float curSpeed;
	bool stopMove,
		 stopTurn;

	// param for debug 
	float dbgSpeed,
		  dbgMove;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
