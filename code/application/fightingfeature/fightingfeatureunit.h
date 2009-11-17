#ifndef FIGHTINGFEATURE_FIGHTINGFEATUREUNIT_H
#define FIGHTINGFEATURE_FIGHTINGFEATUREUNIT_H
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::GraphicsFeatureUnitUnit
    
    The GraphicsFeatures provides everything for rendering graphicentities
    from different views in different stages.

    For the default use it creates one default view and one default stage which are
    used in the graphic and camera properties.    
    The BaseGameFeatureUnit uses this features to build up a default graphic world.

    The GraphicsFeatureUnit also offers visualization of debug shapes. Just
    attach your feature which should be considered in debug rendering.
    It also allows switching thru all debug renderings of each feature.

    Additonal to the rendering the graphicsfeature creates and triggers 
    the input server.

    (C) 2007 Radon Labs GmbH
*/
#include "game/featureunit.h"
#include "coregraphics/renderdevice.h"
#include "managers/inputrulemanager.h"
#include "managers/skillmanager.h"

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