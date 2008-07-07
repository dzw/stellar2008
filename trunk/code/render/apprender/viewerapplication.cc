//------------------------------------------------------------------------------
//  viewerapplication.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "apprender/viewerapplication.h"
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

namespace App
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
ViewerApplication::ViewerApplication():
	isDebugRender(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ViewerApplication::~ViewerApplication()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
*/
bool
ViewerApplication::Open()
{
    n_assert(!this->IsOpen());
    if (RenderApplication::Open())
    {
        GraphicsServer* gfxServer = GraphicsServer::Instance();

        // setup the camera util object
        this->mayaCameraUtil.Setup(point(10.0f, 10.0f, 10.0f), point(0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f));
		this->dxutCameraUtil.Setup(vector(1.0f, 1.0f, 1.0f)/*vector(18468.0f, 177.0f, 14292.66f)*/);
		//this->quaternionCameraUtil.Setup(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -100.0f));
		this->freeCameraUtil.Setup(vector(10.0f, 1.0f, 10.0f));/*16000.0f, 114.0f, 27200.0f));19343.0f, 237.0f, 14136.66f));*/
		//this->freeCameraUtil.MoveForward(28266.66649f);
		//this->freeCameraUtil.MoveRight(12799.99992f);

        // setup a stage
        this->stage = gfxServer->CreateStage(StringAtom("DefaultStage"), SimpleStageBuilder::Create());
		/*Ptr<QuadtreeStageBuilder> quadTreeStageBuilder = QuadtreeStageBuilder::Create();    
		bbox levelBox;
		Attr::AttributeContainer attr;
		levelBox.set(point(17500.0f, 2000.0f, 17500.0f), point(17500.0f, 2000.0f, 17500.0f));
		attr.SetInt(Attr::QuadTreeStageBuilderDepth, 9);
		attr.SetFloat4(Attr::QuadTreeStageBuilderCenter, levelBox.center());
		attr.SetFloat4(Attr::QuadTreeStageBuilderExtents, levelBox.extents());
		quadTreeStageBuilder->SetAttributes(attr);
		this->stage = gfxServer->CreateStage(StringAtom("DefaultStage"), quadTreeStageBuilder.upcast<StageBuilder>());*/

        // attach a camera to the stage
        this->cameraEntity = CameraEntity::Create();
        //cameraEntity->SetTransform(this->mayaCameraUtil.GetCameraTransform());
		//cameraEntity->SetTransform(this->dxutCameraUtil.GetCameraTransform());
		//cameraEntity->SetTransform(this->quaternionCameraUtil.GetCameraTransform());
		cameraEntity->SetTransform(this->mayaCameraUtil.GetCameraTransform());
        this->stage->AttachEntity(cameraEntity.upcast<GraphicsEntity>());

        // setup a default view
        this->view = gfxServer->CreateView(View::RTTI, StringAtom("DefaultView"), true);
        this->view->SetStage(this->stage);        
        this->view->SetFrameShader(FrameServer::Instance()->GetFrameShaderByName(ResourceId(DEFAULT_FRAMESHADER_NAME)));
        this->view->SetCameraEntity(cameraEntity);

        // setup stage content
        ResourceId modelResId;
        if (this->args.HasArg("-model"))
        {
            modelResId = this->args.GetString("-model");

            // attach a light entity
            matrix44 lightTransform = matrix44::multiply(matrix44::scaling(100.0f, 100.0f, 100.0f), matrix44::lookatrh(point(20.0f, 20.0f, 20.0f), point::origin(), vector::upvec()));
            this->lightEntity = SpotLightEntity::Create();
            this->lightEntity->SetCastShadows(true);
            this->lightEntity->SetTransform(lightTransform);
            this->lightEntity->SetColor(float4(4.0f, 2.0f, 1.0f, 1.0f));        
            this->stage->AttachEntity(this->lightEntity.upcast<GraphicsEntity>());

            // attach a model entity
            this->modelEntity = ModelEntity::Create();
            this->modelEntity->SetResourceId(modelResId);
            this->stage->AttachEntity(this->modelEntity.upcast<GraphicsEntity>());      
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ViewerApplication::Close()
{
    n_assert(this->IsOpen());
    
    GraphicsServer* gfxServer = GraphicsServer::Instance();
    gfxServer->DiscardView(this->view);
    this->view = 0;
    gfxServer->DiscardStage(this->stage);
    this->stage = 0;
    this->modelEntity = 0;
    this->lightEntity = 0;
    this->cameraEntity = 0;

    RenderApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ViewerApplication::OnProcessInput()
{
    // @todo: include frame time!
    // update the camera from input
    InputServer* inputServer = InputServer::Instance();
    const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
    const Ptr<Mouse>& mouse = inputServer->GetDefaultMouse();
    const Ptr<GamePad>& gamePad = inputServer->GetDefaultGamePad(0);

    if (gamePad->IsConnected() && gamePad->ButtonPressed(GamePad::LeftShoulderButton))
    {
        // rotate model if right shoulder button is pressed
        float yaw = gamePad->GetAxisValue(GamePad::LeftThumbXAxis) * 0.1f;
        float pitch = gamePad->GetAxisValue(GamePad::LeftThumbYAxis) * 0.1f;
        matrix44 rotMatrix = matrix44::rotationyawpitchroll(yaw, pitch, 0.0f);
        matrix44 m = matrix44::multiply(this->modelEntity->GetTransform(), rotMatrix);
        this->modelEntity->SetTransform(m);
    }
    else
    {
        // standard input handling: manipulate camera
        this->mayaCameraUtil.SetOrbitButton(mouse->ButtonPressed(MouseButton::LeftButton));
        this->mayaCameraUtil.SetPanButton(mouse->ButtonPressed(MouseButton::MiddleButton));
        this->mayaCameraUtil.SetZoomButton(mouse->ButtonPressed(MouseButton::RightButton));
        this->mayaCameraUtil.SetZoomInButton(mouse->WheelForward());
        this->mayaCameraUtil.SetZoomOutButton(mouse->WheelBackward());
        this->mayaCameraUtil.SetMouseMovement(mouse->GetMovement());
        
        this->dxutCameraUtil.SetMouseMovement(mouse->GetMovement());
		if (mouse->WheelBackward() || mouse->WheelForward())
			this->dxutCameraUtil.MoveVertical(mouse->WheelBackward());
		this->dxutCameraUtil.SetLeftButton(mouse->ButtonPressed(MouseButton::LeftButton));
		/*this->quaternionCameraUtil.SetMouseMovement(mouse->GetMovement());
		this->quaternionCameraUtil.SetLeftButton(mouse->ButtonPressed(MouseButton::LeftButton));*/

		this->freeCameraUtil.SetLeftButton(mouse->ButtonPressed(MouseButton::LeftButton));
		this->freeCameraUtil.SetMouseMovement(mouse->GetMovement());
        // process gamepad input
        float zoomIn = 0.0f;
        float zoomOut = 0.0f;
        float2 panning(0.0f, 0.0f);
        float2 orbiting(0.0f, 0.0f);
		vector kbDir = vector::nullvec();
		float moveScaler = 5.0f;
		float speedUp = 1.0f;
        if (gamePad->IsConnected())
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
        }

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

		if (keyboard->KeyPressed(Key::W))
		{
			kbDir.z() += 0.1f;

			this->freeCameraUtil.MoveForward(-moveScaler);
		}
		if (keyboard->KeyPressed(Key::S))
		{
			kbDir.z() -= 0.1f;

			this->freeCameraUtil.MoveForward(moveScaler);
		}
		if (keyboard->KeyPressed(Key::D))
		{
			kbDir.x() -= 0.1f;

			this->freeCameraUtil.MoveRight(moveScaler);
		}
		if (keyboard->KeyPressed(Key::A))
		{
			kbDir.x() += 0.1f;

			this->freeCameraUtil.MoveRight(-moveScaler);
		}
		if (keyboard->KeyPressed(Key::Q))
		{
			this->freeCameraUtil.Yaw(0.01);
		}
		if (keyboard->KeyPressed(Key::E))
		{
			this->freeCameraUtil.Yaw(-0.01);
		}
		/*if (keyboard->KeyPressed(Key::Up))
		{
			this->freeCameraUtil.MoveUp(moveScaler);
		}
		if (keyboard->KeyPressed(Key::Down))
		{
			this->freeCameraUtil.MoveUp(-moveScaler);
		}*/
		if (keyboard->KeyPressed(Key::P))
		{
			speedUp += 1.0f;
			this->freeCameraUtil.SetSpeedUp(speedUp);
			this->dxutCameraUtil.SetSpeedUp(speedUp);
		}
		if (keyboard->KeyPressed(Key::O))
		{
			speedUp -= 1.0f;
			this->freeCameraUtil.SetSpeedUp(speedUp);
			this->dxutCameraUtil.SetSpeedUp(speedUp);
		}

		this->freeCameraUtil.Update();

		this->dxutCameraUtil.SetMovement(kbDir);
		this->dxutCameraUtil.Update();

		/*this->quaternionCameraUtil.SetMovement(kbDir);
		this->quaternionCameraUtil.Update();*/
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
			isDebugRender = !isDebugRender;
		}

		if (keyboard->KeyPressed(Key::Control) && keyboard->KeyPressed(Key::F7))
		{
			gProfiler->enable(true);
			gProfiler->dumpToConsole();
		}

		if (mouse->ButtonPressed(MouseButton::RightButton))
		{
			//Array<plane> planes;
			//point out;
			vector rayOrig, rayDir/*, rayEnd, vout*/;
			//line rayLine;
			Pick(mouse->GetPixelPosition(), &rayOrig, &rayDir);
			//bbox boxPos(rayOrig, rayDir*10000.0f);

			//rayEnd = rayOrig + rayDir*100000.0f;

			//rayDir += rayOrig;
			//rayLine.set(rayOrig, rayDir);

			pickEntities.Clear();

			matrix44 viewProj = matrix44::multiply(this->cameraEntity->GetViewTransform(), this->cameraEntity->GetProjTransform());
			Array<Ptr<GraphicsEntity>> models = this->stage->GetEntitiesByType(GraphicsEntity::ModelType);
			
			for (IndexT i = 0; i < models.Size(); i++)
			{
				const bbox b = models[i]->GetGlobalBoundingBox();
				//if (b.clipstatus(boxPos) != ClipStatus::Outside)
				//if (b.contains(rayOrig))
				//if (b.intersects(rayOrig, rayDir))

				/*b.get_clipplanes(viewProj, planes);
				for (IndexT j = 0; j < planes.Size(); j++)
				{
					if (planes[j].intersectline(rayOrig, rayEnd, out))
					{
						pickEntities.Append(models[i].downcast<ModelEntity>()->GetResourceId().Value());
					}
				}*/

				/*if (b.intersects(rayOrig, rayDir))
				{
					pickEntities.Append(models[i].downcast<ModelEntity>()->GetResourceId().Value());
				}*/
			}
		}

    }
}

//------------------------------------------------------------------------------
/**
*/
void
ViewerApplication::OnRenderFrame()
{
    GraphicsServer::Instance()->OnFrame(this->timer.GetTime());
	if (isDebugRender)
		this->view->RenderDebug();

    // if we are running in windowed mode, be multitasking friendly
    // by giving up our time slice
    if (!DisplayDevice::Instance()->IsFullscreen())
    {
        //Timing::Sleep(0.0);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ViewerApplication::RenderDebugString()
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

	//for (SizeT i = 0; i < modelentites.Size(); i++)
	//{
	//	/*if (i > 10)
	//		break;*/

	//	pos = modelentites[i]->GetTransform().row3();
	//	sprintf(szPos, "%d: %f, %f, %f", i, pos.x(), pos.y(), pos.z());

	//	rect.top += 0.021;
	//	rect.bottom += 0.021;

	//	FontServer::Instance()->DrawText(szPos, float4(1, 0, 0, 1), rect, 0, true);
	//}

	for (SizeT i = 0; i < pickEntities.Size(); i++)
	{
		sprintf(szPos, "%s", pickEntities[i].AsCharPtr());

		rect.top += 0.021;
		rect.bottom += 0.021;

		FontServer::Instance()->DrawText(szPos, float4(1, 0, 0, 1), rect, 0, true);
	}
}
//------------------------------------------------------------------------------
/**
*/
void
ViewerApplication::Pick(const float2& cursor, vector* rayOrig, vector* rayDir) const
{
	DisplayMode mode = DisplayDevice::Instance()->GetDisplayMode();
	const matrix44 proj = this->cameraEntity->GetProjTransform();
	vector v;
	
	POINT cursor32;
	GetCursorPos(&cursor32);
	//ScreenToClient(DisplayDevice::Instance()->GetHwnd(), &cursor32);

	// Compute the vector of the Pick ray in screen space
	//v.x() =  ( ( ( 2.0f * cursor32.x ) / mode.GetWidth()  ) - 1 ) / proj.row0().x();
	//v.y() = -( ( ( 2.0f * cursor32.y ) / mode.GetHeight() ) - 1 ) / proj.row1().y();
	//v.z() = 1.0f;

	////// Get the inverse view matrix
	//const matrix44 view = this->cameraEntity->GetViewTransform();
	//matrix44 invView = matrix44::inverse(view);

	//*rayDir = v;
	//*rayOrig = vector::nullvec();

	//// transform the ray's origin, w = 1.
	//D3DXVec3TransformCoord(
	//	(D3DXVECTOR3*)rayOrig,
	//	(CONST D3DXVECTOR3*)rayOrig,
	//	(D3DXMATRIX*)&invView);

	//// transform the ray's direction, w = 0.
	//D3DXVec3TransformNormal(
	//	(D3DXVECTOR3*)rayDir,
	//	(CONST D3DXVECTOR3*)rayDir,
	//	(D3DXMATRIX*)&invView);

	//// normalize the direction
	//D3DXVec3Normalize((D3DXVECTOR3*)rayDir, (CONST D3DXVECTOR3*)rayDir);

	// Transform the screen space Pick ray into 3D space
	/*vector r0, r1, r2, r3;
	r0 = invView.row0();
	r1 = invView.row1();
	r2 = invView.row2();
	r3 = invView.row3();
	rayDir->x() = v.x()*r0.x() + v.y()*r1.x() + v.z()*r2.x();
	rayDir->y() = v.x()*r0.y() + v.y()*r1.y() + v.z()*r2.y();
	rayDir->z() = v.x()*r0.z() + v.y()*r1.z() + v.z()*r2.z();
	rayOrig->x() = r3.x();
	rayOrig->y() = r3.y();
	rayOrig->z() = r3.z();
	*rayDir = vector::normalize(*rayDir);*/

	//*rayOrig += *rayDir * 1.0f;

	matrix44 clipRender = matrix44::identity();
	clipRender.getrow0().x() =  (float)(mode.GetWidth())  / 2.0f;
	clipRender.getrow3().x() =  (float)(mode.GetWidth())  / 2.0f;
	clipRender.getrow1().y() = -(float)(mode.GetHeight()) / 2.0f;
	clipRender.getrow3().y() =  (float)(mode.GetHeight()) / 2.0f;

	matrix44 screenTransform = matrix44::multiply(this->cameraEntity->GetViewTransform(),
												  this->cameraEntity->GetProjTransform());
	screenTransform = matrix44::multiply(screenTransform, clipRender);
	screenTransform = matrix44::inverse(screenTransform);
	vector v0((float)cursor32.x, (float)cursor32.y, -1.0f);
	vector v1((float)cursor32.x, (float)cursor32.y, 1.0f);
	//D3DXVec3TransformCoord(((D3DXVECTOR3*)rayOrig, (CONST D3DXVECTOR3*)&v0, (D3DXMATRIX*)&screenTransform);
	//D3DXVec3TransformCoord((D3DXVECTOR3*)&v1, (CONST D3DXVECTOR3*)&v1, (D3DXMATRIX*)&screenTransform);
	*rayOrig = vector::transformcoord(v0, screenTransform);
	v1 = vector::transformcoord(v1, screenTransform);
	*rayDir = v1-*rayOrig;
	*rayDir = vector::normalize(*rayDir);
}

}
