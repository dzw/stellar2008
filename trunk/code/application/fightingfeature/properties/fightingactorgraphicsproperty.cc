//------------------------------------------------------------------------------
//  properties/fightingactorgraphicsproperty.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/fightingactorgraphicsproperty.h"
#include "managers/skillmanager.h"
#include "msg/fightinginputresult.h"
#include "msg/movedirection.h"
#include "msg/movestop.h"

namespace Attr
{
}

namespace FightingFeature
{
ImplementClass(FightingFeature::FightingActorGraphicsProperty, 'FAGP', GraphicsFeature::ActorGraphicsProperty);

using namespace BaseGameFeature;
using namespace Util;
using namespace Math;
using namespace Graphics;
using namespace Nebula2;
using namespace GraphicsFeature;

//------------------------------------------------------------------------------
/**
*/
FightingActorGraphicsProperty::FightingActorGraphicsProperty()
{
}

//------------------------------------------------------------------------------
/**
*/
FightingActorGraphicsProperty::~FightingActorGraphicsProperty()
{
}

//------------------------------------------------------------------------------
/**
*/
void
FightingActorGraphicsProperty::OnActivate()
{  
	ActorGraphicsProperty::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
FightingActorGraphicsProperty::OnDeactivate()
{
	ActorGraphicsProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
FightingActorGraphicsProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(FightingInputResultMsg::Id);
	ActorGraphicsProperty::SetupAcceptedMessages();
}


//------------------------------------------------------------------------------
/**
*/
void
FightingActorGraphicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg);
	if (msg->CheckId(FightingInputResultMsg::Id))
	{
		DWORD val = msg.downcast<FightingInputResultMsg>()->GetValue();
		DWORD firstKey = msg.downcast<FightingInputResultMsg>()->GetFirstKey();
		ProcessInputResult(val, firstKey);
	}
	else if (msg->CheckId(MoveDirection::Id))
	{
		return;
	}
	else if (msg->CheckId(MoveStop::Id))
	{
		return;
	}
       
	ActorGraphicsProperty::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
	处理输入
*/
void
FightingActorGraphicsProperty::ProcessInputResult(DWORD val, DWORD firstKey)
{
	DWORD keyValue = val;
	//// 受击处理
	//bool isAttached = 调用受击处理函数;
	//if(isAttached) //如果要处理受击
	//{
	//	return;
	//}

	//If (在眩晕状态)
	//	return;

	//// 连击处理
	//If (当前动作刚结束 && 当前逝去的时间 < 当前动作Delay)
	//{
	//	If (检测与当前动作是否能形成连击)
	//	{
	//		keyValue =连击动作值;
	//	}
	//}
	SkillManager* skManager = SkillManager::Instance();
	SkillInfo skInfo;
	if (skManager->HasSkillValue(keyValue))
	{
		skInfo = skManager->GetSkillValue(keyValue);
	}

	/*当前动作与新动作（skInfo）进行插值，在这里只做付值操作，每帧更新的时候才真正插值。
		curAction = 当前动作;
	nextAction = skInfo;*/

	Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
	Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);

}


}; // namespace GraphicsFeature

