#ifndef PROPERTIES_FIGHTINGACTORGRAPHICSPROPERTY_H
#define PROPERTIES_FIGHTINGACTORGRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    
    (C) 2009 cTuo
*/
#include "properties/actorgraphicsproperty.h"
#include "math\pfeedbackloop.h"
#include "math\angularpfeedbackloop.h"

namespace Attr
{
    
}

//------------------------------------------------------------------------------
namespace FightingFeature
{
class FightingActorGraphicsProperty : public GraphicsFeature::ActorGraphicsProperty
{
	DeclareClass(FightingActorGraphicsProperty);
public:
    /// constructor
    FightingActorGraphicsProperty();
    /// destructor
    virtual ~FightingActorGraphicsProperty();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
	/// called after movement has happened
	//virtual void OnMoveAfter();
	/// called on begin of frame
	virtual void OnMoveAfter();

	/// setup callbacks for this property, call by entity in OnActivate()
	virtual void SetupCallbacks();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
	void ProcessInputResult(DWORD val, DWORD firstKey);
	Math::vector TransformDirection(const Math::vector& dir);
	void Walk(const Math::vector& dir, const Util::String& animName);

	Math::PFeedbackLoop<Math::vector> smoothedPosition;
	Math::AngularPFeedbackLoop smoothedHeading;
	Math::vector lookatDirection;

	DWORD preSkill;
};
RegisterClass(FightingActorGraphicsProperty);

}; // namespace Properties
//------------------------------------------------------------------------------
#endif