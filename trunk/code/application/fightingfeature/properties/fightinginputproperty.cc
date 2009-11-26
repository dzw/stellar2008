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

		Timing::Time curTime = InputTimeSource::Instance()->GetTime();
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::S))
		{
			n_printf("Down S.");
			InputKey k(DownValue, curTime);
			this->keyBuffer.Append(k);
		}
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::W))
		{
			n_printf("Down W.");
			InputKey k(UpValue, curTime);
			this->keyBuffer.Append(k);
		}
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::A))
		{
			n_printf("Down A.");
			InputKey k(LeftValue, curTime);
			this->keyBuffer.Append(k);
		}
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::D))
		{
			n_printf("Down D.");
			InputKey k(RightValue, curTime);
			this->keyBuffer.Append(k);
		}
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::J))	// π•ª˜
		{
			n_printf("Down J.");
			InputKey k(AttactValue, curTime);
			this->keyBuffer.Append(k);
		}
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::K))	// Ã¯‘æ
		{
			n_printf("Down K.");
			InputKey k(JumpValue, curTime);
			this->keyBuffer.Append(k);
		}
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::L))	// ∑¿ ÿ
		{
			n_printf("Down L.");
			InputKey k(DefenseValue, curTime);
			this->keyBuffer.Append(k);
		}

		static Timing::Time tt = 0.2;
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::F2))	// Ã¯‘æ
		{
			tt += 0.05;
		}
		if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::F3))	// ∑¿ ÿ
		{
			tt -= 0.05;
			if (tt < 0.001) tt = 0.0;
		}

		/*if (this->keyBuffer.Size() == 0)
		{
			if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::S))
			{
				n_printf("S.");
				this->keyBuffer.Clear();
				this->keyBuffer.Append(DownValue);
			}
			if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::W))
			{
				n_printf("W.");
				this->keyBuffer.Clear();
				this->keyBuffer.Append(UpValue);
			}
			if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::A))
			{
				n_printf("A.");
				this->keyBuffer.Clear();
				this->keyBuffer.Append(LeftValue);
			}
			if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::D))
			{
				n_printf("D.");
				this->keyBuffer.Clear();
				this->keyBuffer.Append(RightValue);
			}
		}*/

		if (this->keyBuffer.Size() > 0)
		{
			Util::Array<DWORD> keys;
			for (SizeT i = 0; i < this->keyBuffer.Size(); i++)
			{
				if (curTime - this->keyBuffer[i].t > tt)
					keys.Append(this->keyBuffer[i].val);
			}
			if (keys.Size() > 0)
			{
				n_printf("%f\n", curTime - lastTime);
				ProcessKeyBuffer(keys);
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
	¥¶¿Ì ‰»Îº¸,≤¢∑¢ÀÕΩ·π˚
*/
void
FightingInputProperty::ProcessKeyBuffer(const Util::Array<DWORD>& keys)
{
	if (keys.Size() == 0)
	{
		return;
	}

	DWORD val = InputRuleManager::Instance()->CheckInvalid(keys);
	DWORD firstKey = keys[0];

	Ptr<FightingInputResultMsg> msg = FightingInputResultMsg::Create();
	msg->SetValue(val);
	msg->SetFirstKey(firstKey);
	this->entity->SendSync(msg.upcast<Messaging::Message>());

	this->keyBuffer.Clear();
}

}; // namespace GraphicsFeature
