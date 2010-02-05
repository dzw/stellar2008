#pragma once
#ifndef FIGHTER_FHERO_H
#define FIGHTER_FHERO_H
//------------------------------------------------------------------------------
/**
	对象层次关系:
	object
	  npc 
		monster
		player
		  hero
*/
#include "fplayer.h"

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
private:
	Ptr<CameraEntity> camera;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
