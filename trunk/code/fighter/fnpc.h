#pragma once
#ifndef FIGHTER_FNPC_H
#define FIGHTER_FNPC_H
//------------------------------------------------------------------------------
/**
	对象层次关系:
	object
	  npc 
		monster
		player
		  hero
*/
#include "fobject.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FNpc : public FObject
{
	DeclareClass(FNpc);
public:
    /// constructor
    FNpc();
    /// destructor
    virtual ~FNpc();
	
	virtual void Init();
	virtual void Clear();
private:
	
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
