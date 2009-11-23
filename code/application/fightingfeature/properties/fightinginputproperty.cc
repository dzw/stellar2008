//------------------------------------------------------------------------------
//  properties/fightinginputproperty.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/fightinginputproperty.h"
#include "basegametiming/inputtimesource.h"
#include "managers/focusmanager.h"
#include "managers/inputrulemanager.h"
#include "render/input/inputserver.h"
#include "render/input/keyboard.h"
#include "render/input/mouse.h"
#include "msg/camerareset.h"
#include "msg/cameraorbit.h"
#include "msg/movestop.h"
#include "msg/fightinginputresult.h"

namespace Attr
{
    
};

namespace FightingFeature
{
ImplementClass(FightingFeature::FightingInputProperty, 'FINP', GraphicsFeature::InputProperty);

using namespace Math;
using namespace Game;
using namespace BaseGameFeature;
using namespace Input;
using namespace GraphicsFeature;

//------------------------------------------------------------------------------
/**
*/
FightingInputProperty::FightingInputProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FightingInputProperty::~FightingInputProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Handle general input.
*/
void
FightingInputProperty::OnBeginFrame()
{
#if (1)//__DEBUG_ACTIVE__
    // only do something if we have the input focus
    if (FocusManager::Instance()->GetInputFocusEntity() == this->entity)
    {
        InputServer* inputServer = InputServer::Instance();
        
    #if !__WII__        
        if (inputServer->GetDefaultKeyboard()->KeyDown(Key::Space))
        {
            // send camera reset msg
            Ptr<CameraReset> camReset = CameraReset::Create();
            this->entity->SendSync(camReset.upcast<Messaging::Message>());
        }                
        if (inputServer->GetDefaultMouse()->ButtonPressed(MouseButton::RightButton))
        {
            float2 mouseMove = inputServer->GetDefaultMouse()->GetMovement();     

            // create CameraOrbit message
            Ptr<CameraOrbit> msg = CameraOrbit::Create();
            msg->SetHorizontalRotation(mouseMove.x());
            msg->SetVerticalRotation(mouseMove.y());
            this->entity->SendSync(msg.upcast<Messaging::Message>());            
        }
        if (inputServer->GetDefaultMouse()->WheelForward())
        {
            this->OnCameraZoomIn();
        }
        if (inputServer->GetDefaultMouse()->WheelBackward())
        {
            this->OnCameraZoomOut();
        }

		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::S))
		{
			n_printf("S.");
			this->keyBuffer.Append(DownValue);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::W))
		{
			n_printf("W.");
			this->keyBuffer.Append(UpValue);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::A))
		{
			n_printf("A.");
			this->keyBuffer.Append(LeftValue);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::D))
		{
			n_printf("D.");
			this->keyBuffer.Append(RightValue);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::J))	// 攻击
		{
			n_printf("J.");
			this->keyBuffer.Append(AttactValue);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::K))	// 跳跃
		{
			n_printf("K.");
			this->keyBuffer.Append(JumpValue);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::L))	// 防守
		{
			n_printf("L.");
			this->keyBuffer.Append(DefenseValue);
		}

		if (this->keyBuffer.Size() > 0)
		{
			n_printf("key input.\n");
			Timing::Time curTime = InputTimeSource::Instance()->GetTime();
			if (curTime - lastTime > 0.05)
			{
				ProcessKeyBuffer();
				lastTime = curTime;
			}
		}
		else
		{
			Ptr<MoveStop> msg = MoveStop::Create();
			this->entity->SendSync(msg.upcast<Messaging::Message>());
		}
        
     #else   
     
        const Ptr<GamePad>& gamepad = inputServer->GetDefaultGamePad(0);
        if (gamepad->ButtonDown(GamePad::StartButton))
        {
            // send camera reset msg
            Ptr<CameraReset> camReset = CameraReset::Create();
            this->entity->SendSync(camReset.upcast<Messaging::Message>());
        }                
        if (gamepad->GetAxisValue(GamePad::LeftThumbXAxis) != 0
        ||  gamepad->GetAxisValue(GamePad::LeftThumbYAxis) != 0)
        {
            // create CameraOrbit message
            Ptr<CameraOrbit> msg = CameraOrbit::Create();
            msg->SetHorizontalRotation(gamepad->GetAxisValue(GamePad::LeftThumbXAxis) * 0.1f);
            msg->SetVerticalRotation(gamepad->GetAxisValue(GamePad::LeftThumbYAxis) * 0.1f);
            this->entity->SendSync(msg.upcast<Messaging::Message>());            
        }
        if (gamepad->GetAxisValue(GamePad::LeftTriggerAxis) != 0)
        {
            this->OnCameraZoomIn();
        }
        if (gamepad->GetAxisValue(GamePad::RightTriggerAxis) != 0)
        {
            this->OnCameraZoomOut();
        }
        if (gamepad->GetAxisValue(GamePad::RightThumbXAxis) != 0
            ||  gamepad->GetAxisValue(GamePad::RightThumbYAxis) != 0)
        {            
            Ptr<MoveDirection> msg = MoveDirection::Create();
            msg->SetDirection(vector(gamepad->GetAxisValue(GamePad::RightThumbXAxis),0,gamepad->GetAxisValue(GamePad::RightThumbYAxis)* -1.0f));
            msg->SetCameraRelative(true);
            this->entity->SendSync(msg.upcast<Messaging::Message>());                
        }       
        else
        {
            Ptr<MoveStop> msg = MoveStop::Create();
            this->entity->SendSync(msg.upcast<Messaging::Message>());
        }
    #endif
    }
#endif
}

//------------------------------------------------------------------------------
/**
	处理输入键,并发送结果
*/
void
FightingInputProperty::ProcessKeyBuffer()
{
	if (this->keyBuffer.Size() == 0)
	{
		return;
	}
n_printf("ProcessKeyBuffer.\n");
	DWORD val = InputRuleManager::Instance()->CheckInvalid(this->keyBuffer);
	DWORD firstKey = this->keyBuffer[0];

	Ptr<FightingInputResultMsg> msg = FightingInputResultMsg::Create();
	msg->SetValue(val);
	msg->SetFirstKey(firstKey);
	this->entity->SendSync(msg.upcast<Messaging::Message>());

	this->keyBuffer.Clear();
}

}; // namespace GraphicsFeature
