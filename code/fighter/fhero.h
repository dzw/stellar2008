#pragma once
#ifndef FIGHTER_FHERO_H
#define FIGHTER_FHERO_H
//------------------------------------------------------------------------------
/**
	֡���͵�ʱ�򿨵���������·����֡������

	���ǣ�Ψһ

	�����ι�ϵ:
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

//------------------------------------------------------------------------------
namespace Fighter
{
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
	void SetSpeed(float f);
	virtual void SetCurrentAnimation(int id);
	void Walk(const Math::vector& dir, const Util::String& animName);
	void Run(const Math::vector& dir, const Util::String& animName);
protected:
	Ptr<Graphics::CameraEntity> camera;
	Math::PFeedbackLoop<Math::vector> smoothedPosition;
	Math::AngularPFeedbackLoop smoothedHeading;
	Math::vector lookatDirection;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
