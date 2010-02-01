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
#include "core/refcounted.h"
#include "core/types.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FNpc : public Core::RefCounted
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
