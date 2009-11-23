//------------------------------------------------------------------------------
//  fightingfeature/fightingfeatureunit.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fightingfeature/fightingfeatureunit.h"
#include "properties/fightinginputproperty.h"
#include "properties/fightingactorgraphicsproperty.h"

namespace FightingFeature
{
ImplementClass(FightingFeatureUnit, 'FTFX' , Game::FeatureUnit);
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

	this->AttachManager(this->inputRuleManager.upcast<Game::Manager>());
	this->AttachManager(this->skillManager.upcast<Game::Manager>());
}

//------------------------------------------------------------------------------
/**
*/
void 
FightingFeatureUnit::OnDeactivate()
{
	this->RemoveManager(this->inputRuleManager.upcast<Game::Manager>());
	this->RemoveManager(this->skillManager.upcast<Game::Manager>());

	this->inputRuleManager = 0;
	this->skillManager = 0;
}

}; // namespace Game
