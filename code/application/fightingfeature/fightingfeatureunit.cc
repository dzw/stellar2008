//------------------------------------------------------------------------------
//  game/fightingfeature.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fightingfeature/fightingfeatureunit.h"
#include "fightingfeature\properties\fightingactorgraphicsproperty.h"
#include "fightingfeature\properties\fightinginputproperty.h"

namespace FightingFeature
{
ImplementClass(FightingFeatureUnit, 'FFUT' , Game::FeatureUnit);
ImplementSingleton(FightingFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
FightingFeatureUnit::FightingFeatureUnit()
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FightingFeatureUnit::~FightingFeatureUnit()
{
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
FightingFeatureUnit::OnActivate()
{
	FeatureUnit::OnActivate();

	this->inputRuleManager = InputRuleManager::Create();
	this->skillManager = SkillManager::Create();

    /// add this feature for render debug callback
    //this->AddRenderDebugCallback(this, "Graphics");
}

//------------------------------------------------------------------------------
/**
*/
void
FightingFeatureUnit::OnDeactivate()
{
	this->inputRuleManager = 0;
	this->skillManager = 0;
    FeatureUnit::OnDeactivate();
}

}; // namespace Game
