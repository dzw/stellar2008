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
enum ObjectType
{
	ObjectType_Hero = 0,
	ObjectType_Npc,
	ObjectType_Player,
	ObjectType_Monster,
	ObjectType_Num,
};
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
	virtual void Update();

	BYTE GetObjectType()const;
	
private:
	Ptr<Graphics::ModelEntity> model;
	BYTE type;
};

inline BYTE 
FObject::GetObjectType()const
{
	return this->type;
}

} // namespace Test
//------------------------------------------------------------------------------
#endif
