//------------------------------------------------------------------------------
//  msg/inputresult.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "msg/fightinginputresult.h"

namespace FightingFeature
{
ImplementClass(FightingFeature::FightingInputResultMsg, 'MGFI', Messaging::Message);
ImplementMsgId(FightingInputResultMsg);

//------------------------------------------------------------------------------
/**
*/
FightingInputResultMsg::FightingInputResultMsg()
{
    // empty
}

};
