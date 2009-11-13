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
	��������
*/
void
FightingActorGraphicsProperty::ProcessInputResult(DWORD val, DWORD firstKey)
{
	DWORD keyValue = val;
	//// �ܻ�����
	//bool isAttached = �����ܻ�������;
	//if(isAttached) //���Ҫ�����ܻ�
	//{
	//	return;
	//}

	//If (��ѣ��״̬)
	//	return;

	//// ��������
	//If (��ǰ�����ս��� && ��ǰ��ȥ��ʱ�� < ��ǰ����Delay)
	//{
	//	If (����뵱ǰ�����Ƿ����γ�����)
	//	{
	//		keyValue =��������ֵ;
	//	}
	//}
	SkillManager* skManager = SkillManager::Instance();
	SkillInfo skInfo;
	if (skManager->HasSkillValue(keyValue))
	{
		skInfo = skManager->GetSkillValue(keyValue);
	}

	/*��ǰ�������¶�����skInfo�����в�ֵ��������ֻ����ֵ������ÿ֡���µ�ʱ���������ֵ��
		curAction = ��ǰ����;
	nextAction = skInfo;*/

	Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
	Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);

}


}; // namespace GraphicsFeature

