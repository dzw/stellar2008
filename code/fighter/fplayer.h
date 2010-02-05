#pragma once
#ifndef FIGHTER_FPLAYER_H
#define FIGHTER_FPLAYER_H
//------------------------------------------------------------------------------
/**
	对象层次关系:
	object
	  npc 
		monster
		player
		  hero
*/
#include "fnpc.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FPlayer : public FNpc
{
	DeclareClass(FPlayer);
public:
    /// constructor
    FPlayer();
    /// destructor
    virtual ~FPlayer();
	
	virtual void Init();
	virtual void Clear();
private:
	
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
