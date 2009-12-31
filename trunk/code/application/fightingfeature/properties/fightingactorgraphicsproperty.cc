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
#include "basegametiming/gametimesource.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphics/cameraentity.h"
#include "managers/inputrulemanager.h"
#include "math/polar.h"

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
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
FightingActorGraphicsProperty::FightingActorGraphicsProperty():
preSkill(SL_Num)
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
	// initialize feedback loops for motion smoothing
	Time time = GameTimeSource::Instance()->GetTime();
	matrix44 entityMatrix = this->GetEntity()->GetMatrix44(Attr::Transform);
	this->smoothedPosition.Reset(time, 0.001f, -25.0f, entityMatrix.getpos_component());

	polar headingAngle(entityMatrix.getz_component());
    this->smoothedHeading.Reset(time, 0.001f, -25.0f, headingAngle.rho);

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
	else
	{
		ActorGraphicsProperty::HandleMessage(msg);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
FightingActorGraphicsProperty::SetupCallbacks()
{
	this->entity->RegisterPropertyCallback(this, MoveAfter);
}

//------------------------------------------------------------------------------
/**
    The OnMoveAfter() method transfers the current physics entity transform to 
    the game entity.
*/
void
FightingActorGraphicsProperty::OnMoveAfter()
{
    if (1)
    {
        // if current state and goal are already close to each other,
        // we don't send a position update
        // NOTE: this check must be made before the feedback look update!
        vector posError = this->smoothedPosition.GetState() - this->smoothedPosition.GetGoal();
        
        // always evaluate the feedback loops to keep them in valid time delta
        Time time = GameTimeSource::Instance()->GetTime();
        this->smoothedPosition.Update(time);
		this->smoothedHeading.Update(time);

        // only send update transform if anything changed
        if ((posError.length() > 0.001f))
        {
            // construct the new entity matrix
			//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
			//mat.translate(this->smoothedPosition.GetState());

			matrix44 matrix = matrix44::rotationy(this->smoothedHeading.GetState());
            matrix.translate(this->smoothedPosition.GetState());

			/*float4 pos = this->smoothedPosition.GetState();
			pos.w() = 1.0f;
			
			entityMatrix.setrow3(pos);*/
			this->GetEntity()->SetMatrix44(Attr::Transform, matrix);
			UpdateTransform(matrix);
        }
		else
		{
			Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			Entity->SetBaseAnimation("idle_01", 0.2f, 0.0f, true, true, 0.2f);
			this->preSkill = 0;
		}

    }
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

	switch(skInfo.keyValue)
	{
	case SL_WalkUp:		// 上
		{
			Walk(vector(0.0f, 0.0f, 1.0f), skInfo.animName);
			break;
		}
	case SL_WalkDown:		// 下
		{
			Walk(vector(0.0f, 0.0f, -1.0f), skInfo.animName);
			break;
		}
	case SL_WalkLeft:		// 左
		{
			Walk(vector(-1.0f, 0.0f, 0.0f), skInfo.animName);
			break;
		}
	case SL_WalkRight:		// 右
		{
			// 在跑的状态
			if (this->preSkill == SL_RunRight)
			{
				SkillInfo sk = skManager->GetSkillValue(this->preSkill);
				Walk(vector(1.0f, 0.0f, 0.0f), sk.animName);
			}
			else
			{
				Walk(vector(1.0f, 0.0f, 0.0f), skInfo.animName);
			}
			break;
		}
	case SL_Attack:		// 攻(A)
		{
			break;
		}
	case SL_Jump:		// 跳(J)
		{
			break;
		}
	case SL_Defend:		// 防(D)
		{
			Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}

	case SL_RunRight:		// 奔跑右
		{
			Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}
	case SL_RunLeft:		// 奔跑左
		{
			break;
		}
	case SL_RunDown:		
		{
			break;
		}
	case SL_RunUp:
		{
			break;
		}
	}
}

void
FightingActorGraphicsProperty::Walk(const Math::vector& dir, const Util::String& animName)
{
	Math::vector direction = dir;
	direction = this->TransformDirection(dir);
	direction.y() = 0.0f;
	direction = vector::normalize(direction);

	matrix44 entityMatrix = this->GetEntity()->GetMatrix44(Attr::Transform);
	//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
	//mat.translate(dir);
	this->smoothedPosition.SetGoal(entityMatrix.getpos_component()+direction);
	//this->graphicsEntities[0]->SetTransform(mat);

	this->lookatDirection = direction;
	matrix44 fixedTransform = matrix44::lookatlh(entityMatrix.getpos_component(), entityMatrix.getpos_component() + this->lookatDirection, vector::upvec());
	polar headingAngles(fixedTransform.getz_component());
	this->smoothedHeading.SetGoal(headingAngles.rho);

	Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
	Entity->SetBaseAnimation(animName, 0.2f, 0.0f, true, true, 0.2f);
}

Math::vector
FightingActorGraphicsProperty::TransformDirection(const Math::vector& dir)
{
	const Ptr<Graphics::View>& curView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
	Graphics::CameraEntity* camera = curView->GetCameraEntity();
	n_assert(camera);
	matrix44 camTransform = camera->GetTransform();
	camTransform.setpos_component(float4(0.0f, 0.0f, 0.0f, 1.0f));
	return vector::transform(dir, camTransform);
}

}; // namespace GraphicsFeature

