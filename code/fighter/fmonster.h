#pragma once
#ifndef FIGHTER_FMONSTER_H
#define FIGHTER_FMONSTER_H
//------------------------------------------------------------------------------
/**
	�����ι�ϵ:
	object
	  npc 
		monster
		player
		  hero
*/


//------------------------------------------------------------------------------
namespace Fighter
{
class FMonster : public FNpc
{
	DeclareClass(FMonster);
public:
    /// constructor
    FMonster();
    /// destructor
    virtual ~FMonster();
	
	virtual void Init();
	virtual void Clear();
private:
	
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
