#ifndef PROPERTIES_FIGHTINGINPUTPROPERTY_H
#define PROPERTIES_FIGHTINGINPUTPROPERTY_H
//------------------------------------------------------------------------------
/**
    
    (C) 2009 cTuo
*/
#include "game/property.h"
#include "graphicsattr/graphicsattributes.h"
#include "properties/inputproperty.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Attr
{
    
};

//------------------------------------------------------------------------------
namespace FightingFeature
{
class FightingInputProperty : public GraphicsFeature::InputProperty
{	
    DeclareClass(FightingInputProperty);
public:
    /// constructor
    FightingInputProperty();
    /// destructor
    virtual ~FightingInputProperty();
    /// handle a single message
	virtual void OnBeginFrame();

protected:
	void ProcessKeyBuffer();

	Util::Array<DWORD> keyBuffer;
	Timing::Time lastTime;
};
RegisterClass(FightingInputProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif