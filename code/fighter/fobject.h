#pragma once
#ifndef FIGHTER_FACTOR_H
#define FIGHTER_FACTOR_H
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
class FObject : public Core::RefCounted
{
	DeclareClass(FObject);
public:
    /// constructor
    FObject();
    /// destructor
    virtual ~FObject();
	
	virtual void Init();
	virtual void Clear();
private:
	Ptr<Graphics::ModelEntity> model;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
