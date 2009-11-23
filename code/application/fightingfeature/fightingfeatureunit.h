#ifndef FIGHTINGFEATURE_FIGHTINGFEATUREUNIT_H
#define FIGHTINGFEATURE_FIGHTINGFEATUREUNIT_H
//------------------------------------------------------------------------------
/**
    @class FightingFeature::FightingFeatureUnit
*/
#include "game/featureunit.h"
#include "managers/skillmanager.h"
#include "managers/inputrulemanager.h"

//------------------------------------------------------------------------------
namespace FightingFeature
{

class FightingFeatureUnit : public Game::FeatureUnit    
{
    DeclareClass(FightingFeatureUnit);
    DeclareSingleton(FightingFeatureUnit);   

public:

    /// constructor
    FightingFeatureUnit();
    /// destructor
    virtual ~FightingFeatureUnit();

	/// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();
protected:
    Ptr<InputRuleManager> inputRuleManager;
	Ptr<SkillManager> skillManager;
};

}; // namespace GraphicsFeatureUnit
//------------------------------------------------------------------------------
#endif