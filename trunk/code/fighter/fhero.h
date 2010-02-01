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
#include "core/refcounted.h"
#include "core/types.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FHero : public Core::RefCounted
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
	
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
