//------------------------------------------------------------------------------
//  testviewerapplication.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/fmain.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "graphics/simplestagebuilder.h"
#include "graphics/view.h"
#include "math/matrix44.h"
#include "math/quaternion.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/gamepad.h"
#include "frame/frameserver.h"
#include "graphics/quadtreestagebuilder.h"
#include "coregraphics/fontserver.h"
#include "math/rectangle.h"
#include "attr/attributecontainer.h"

#include "debug/perform/profiler.h"

namespace Fighter
{
using namespace Util;
using namespace CoreGraphics;
using namespace Resources;
using namespace Graphics;
using namespace Lighting;
using namespace Math;
using namespace Input;
using namespace Frame;

//------------------------------------------------------------------------------
/**
*/
FighterApplication::FighterApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FighterApplication::~FighterApplication()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
FighterApplication::Open()
{
    n_assert(!this->IsOpen());
    if (RenderApplication::Open())
    {
		GraphicsServer* gfxServer = GraphicsServer::Instance();

		// setup the camera util object
		this->mayaCameraUtil.Setup(point(0.0f, 0.0f, 0.0f), point(0.0, 0.0, 29.38), vector(0.0f, 1.0f, 0.0f));

		// setup a stage
		this->stage = gfxServer->CreateStage(StringAtom("DefaultStage"), SimpleStageBuilder::Create());

		// attach a camera to the stage
		/*this->cameraEntity = CameraEntity::Create();
		cameraEntity->SetTransform(this->mayaCameraUtil.GetCameraTransform());
		this->stage->AttachEntity(cameraEntity.upcast<GraphicsEntity>());*/
		
		// setup a default view
		this->view = gfxServer->CreateView(View::RTTI, StringAtom("DefaultView"), true);
		this->view->SetStage(this->stage);        
		this->view->SetFrameShader(FrameServer::Instance()->GetFrameShaderByName(ResourceId(DEFAULT_FRAMESHADER_NAME)));
		//this->view->SetCameraEntity(cameraEntity);

        // setup lights
        //matrix44 lightTransform = matrix44::rotationx(n_deg2rad(-70.0f));
        //this->globalLight = GlobalLightEntity::Create();
        //this->globalLight->SetTransform(lightTransform);
        //this->globalLight->SetColor(float4(1.0f, 1.0f, 1.5f, 1.0f));
        ////this->globalLight->SetBackLightColor(float4(0.0f, 0.0f, 0.0f, 0.0f));
        //this->globalLight->SetCastShadows(false);
        //this->stage->AttachEntity(this->globalLight.upcast<GraphicsEntity>());
		
		this->view->SetFrameShader(Frame::FrameServer::Instance()->GetFrameShaderByName(ResourceId("DX9Sample")));

		this->cameraManager = FCameraManager::Create();
		this->cameraManager->Open(this->view);

		this->worldManager = FWorldManager::Create();
		this->worldManager->Open();

		this->objectManager = FObjectManager::Create();
		this->objectManager->Open();

		//ResourceId modelResId;
		//if (this->args.HasArg("-n2"))
		//{
		//	modelResId = this->args.GetString("-n2");

		//	lightTransform = matrix44::scaling(0.5f, 0.5f, 0.5f);
		//	this->head = ModelEntity::Create();
		//	//this->head->SetTransform(matrix44::translation(0.0f, 3.0f, 0.0f));
		//	this->head->SetResourceId(modelResId);
		//	this->head->SetTransform(lightTransform);
		//	this->stage->AttachEntity(this->head.upcast<GraphicsEntity>());
		//}

		/*this->modelEntity = ModelEntity::Create();
		this->modelEntity->SetResourceId(ResourceId("wow:Character\\Bloodelf\\male\\bloodelfmale.m2"));
		this->stage->AttachEntity(this->modelEntity.upcast<GraphicsEntity>());*/

		this->terrainEntity = ModelEntity::Create();
		this->terrainEntity->SetResourceId(ResourceId("mdl:examples/ground.n2"));
		this->stage->AttachEntity(this->terrainEntity.upcast<GraphicsEntity>());

		/*this->modelEntity = ModelEntity::Create();
		this->modelEntity->SetResourceId(ResourceId("mdl:characters/mensch_m.n2"));
		this->stage->AttachEntity(this->modelEntity.upcast<GraphicsEntity>());*/

		//this->chaseCaneraUtil.SetEntity(this->modelEntity.upcast<GraphicsEntity>());

		

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
FighterApplication::Close()
{
    /*this->globalLight = 0;
    this->localLight0 = 0;
    this->localLight1 = 0;
    this->ground = 0;
    this->head   = 0;
	this->tree	 = 0;*/

	if (this->cameraManager.isvalid())
	{
		this->cameraManager->Close();
		this->cameraManager = 0;
	}
	if (this->worldManager.isvalid())
	{
		this->worldManager->Close();
		this->worldManager = 0;
	}
	if (this->objectManager.isvalid())
	{
		this->objectManager->Close();
		this->objectManager = 0;
	}

	GraphicsServer* gfxServer = GraphicsServer::Instance();
	gfxServer->DiscardView(this->view);
	this->view = 0;
	gfxServer->DiscardStage(this->stage);
	this->stage = 0;
	this->modelEntity = 0;
	this->terrainEntity = 0;
	this->lightEntity = 0;
	this->cameraEntity = 0;

	

    RenderApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
FighterApplication::OnProcessInput()
{
	// @todo: include frame time!
	// update the camera from input
	InputServer* inputServer = InputServer::Instance();
	const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
	const Ptr<Mouse>& mouse = inputServer->GetDefaultMouse();
	//const Ptr<GamePad>& gamePad = inputServer->GetDefaultGamePad(0);

	//if (gamePad->IsConnected() && gamePad->ButtonPressed(GamePad::LeftShoulderButton))
	//{
	//    // rotate model if right shoulder button is pressed
	//    float yaw = gamePad->GetAxisValue(GamePad::LeftThumbXAxis) * 0.1f;
	//    float pitch = gamePad->GetAxisValue(GamePad::LeftThumbYAxis) * 0.1f;
	//    matrix44 rotMatrix = matrix44::rotationyawpitchroll(yaw, pitch, 0.0f);
	//    matrix44 m = matrix44::multiply(this->modelEntity->GetTransform(), rotMatrix);
	//    this->modelEntity->SetTransform(m);
	//}
	//else
	{
		const Math::float2& v = mouse->GetMovement();
		/*if (mouse->ButtonPressed(MouseButton::RightButton))
		{
			this->chaseCaneraUtil.SetOrbit(v.x(), v.y(), this->frameTime);
		}
		if (mouse->WheelForward())
		{
			this->chaseCaneraUtil.SetDistance(-1.0f);
		}
		if (mouse->WheelBackward())
		{
			this->chaseCaneraUtil.SetDistance(1.0f);
		}

		this->chaseCaneraUtil.Update(this->time, true);
		this->cameraEntity->SetTransform(this->chaseCaneraUtil.GetCameraTransform());*/

		// standard input handling: manipulate camera
		this->mayaCameraUtil.SetOrbitButton(mouse->ButtonPressed(MouseButton::LeftButton));
		this->mayaCameraUtil.SetPanButton(mouse->ButtonPressed(MouseButton::MiddleButton));
		this->mayaCameraUtil.SetZoomButton(mouse->ButtonPressed(MouseButton::RightButton));
		this->mayaCameraUtil.SetZoomInButton(mouse->WheelForward());
		this->mayaCameraUtil.SetZoomOutButton(mouse->WheelBackward());
		this->mayaCameraUtil.SetMouseMovement(mouse->GetMovement());

		// process gamepad input
		float zoomIn = 0.0f;
		float zoomOut = 0.0f;
		float2 panning(0.0f, 0.0f);
		float2 orbiting(0.0f, 0.0f);
		vector kbDir = vector::nullvec();
		float moveScaler = 5.0f;
		float speedUp = 1.0f;
		/*if (gamePad->IsConnected())
		{
		if (gamePad->ButtonDown(GamePad::AButton))
		{
		this->mayaCameraUtil.Reset();
		this->modelEntity->SetTransform(matrix44::identity());
		}
		if (gamePad->ButtonDown(GamePad::StartButton) ||
		gamePad->ButtonDown(GamePad::BackButton))
		{
		this->SetQuitRequested(true);
		}
		float frameTime = (float) this->GetFrameTime();
		zoomIn       += gamePad->GetAxisValue(GamePad::RightTriggerAxis) * frameTime;
		zoomOut      += gamePad->GetAxisValue(GamePad::LeftTriggerAxis) * frameTime;
		panning.x()  += gamePad->GetAxisValue(GamePad::RightThumbXAxis) * frameTime;
		panning.y()  += gamePad->GetAxisValue(GamePad::RightThumbYAxis) * frameTime;
		orbiting.x() += gamePad->GetAxisValue(GamePad::LeftThumbXAxis) * frameTime;
		orbiting.y() += gamePad->GetAxisValue(GamePad::LeftThumbYAxis) * frameTime;
		}*/

		// process keyboard input
		if (keyboard->KeyDown(Key::Escape))
		{
			this->SetQuitRequested(true);
		}
		if (keyboard->KeyDown(Key::Space))
		{
			this->mayaCameraUtil.Reset();
		}
		if (keyboard->KeyPressed(Key::Left))
		{
			panning.x() -= 0.1f;
		}
		if (keyboard->KeyPressed(Key::Right))
		{
			panning.x() += 0.1f;
		}
		if (keyboard->KeyPressed(Key::Up))
		{
			panning.y() -= 0.1f;
		}
		if (keyboard->KeyPressed(Key::Down))
		{
			panning.y() += 0.1f;
		}

		this->mayaCameraUtil.SetPanning(panning);
		this->mayaCameraUtil.SetOrbiting(orbiting);
		this->mayaCameraUtil.SetZoomIn(zoomIn);
		this->mayaCameraUtil.SetZoomOut(zoomOut);
		this->mayaCameraUtil.Update();
		this->cameraEntity->SetTransform(this->mayaCameraUtil.GetCameraTransform());


		if (keyboard->KeyPressed(Key::Control) && keyboard->KeyPressed(Key::F5))
		{
			ShaderServer::Instance()->Update();
		}

		if (keyboard->KeyPressed(Key::Control) && keyboard->KeyPressed(Key::F6))
		{
			this->view->ShowDebugInfo();
		}

		if (keyboard->KeyPressed(Key::Control) && keyboard->KeyPressed(Key::F7))
		{
			gProfiler->enable(true);
			gProfiler->dumpToConsole();
		}

	}
}

//------------------------------------------------------------------------------
/**
*/
void
FighterApplication::OnRenderFrame()
{
	GraphicsServer::Instance()->OnFrame(this->timer.GetTime());

	// if we are running in windowed mode, be multitasking friendly
	// by giving up our time slice
	//if (!DisplayDevice::Instance()->IsFullscreen())
	//{
	//    //Timing::Sleep(0.0);
	//}
}

//------------------------------------------------------------------------------
/**
*/
void 
FighterApplication::RenderDebugString()
{
	// ÏÔÊ¾×ø±ê
	rectangle<float> rect(0, 0, 1, 1);
	vector pos;// = this->freeCameraUtil.GetPosition();
	char szPos[128];
	//sprintf(szPos, "%f, %f, %f", pos.x(), pos.y(), pos.z());
	//FontServer::Instance()->DrawText(szPos, float4(1, 0, 0, 1), rect, 0, true);

	Array<Ptr<GraphicsEntity>> modelentites = this->stage->GetEntities();
	pos = this->cameraEntity->GetTransform().getrow3();
	sprintf(szPos, "Entities: %d\n %f, %f, %f", modelentites.Size(), pos.x(), pos.y(), pos.z());
	FontServer::Instance()->DrawText(szPos, float4(1, 0, 0, 1), rect, 0, true);
	rect.top += 0.021;

}

//------------------------------------------------------------------------------
/**
*/
void
FighterApplication::OnUpdateFrame()
{
	Timing::Timer tt;
	tt.Reset();
	tt.Start();

	//this->worldManager->Update();

	tt.Stop();
	float t = (float)tt.GetTime();
	String output;
	output.AppendFloat(t);
	CoreGraphics::DebugView::Instance()->AddDebugString("OnUpdateFrame: ", output);
/*
    point lookatPos = this->head->GetTransform().getrow3();

    // animate lights
    float curTime = (float) this->GetTime();
    point pos(20.0f * n_sin(curTime), 
              20.0f + 15.0f * n_sin(curTime * 0.5f), 
              20.0f + 7.5f * n_sin(curTime * 0.33f));
    matrix44 lightTransform = matrix44::multiply(matrix44::scaling(75.0f, 75.0f, 100.0f), matrix44::lookatrh(pos, lookatPos, vector::upvec()));
    this->localLight0->SetTransform(lightTransform);

    pos.set(-20.0f * n_sin(curTime), 20.0f - 10.0f * n_sin(curTime * 0.7f), 20.0f);
    lightTransform = matrix44::multiply(matrix44::scaling(75.0f, 75.0f, 100.0f), matrix44::lookatrh(pos, lookatPos, vector::upvec()));
    this->localLight1->SetTransform(lightTransform);
*/
    RenderApplication::OnUpdateFrame();
}

} // namespace Tools
