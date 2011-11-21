//------------------------------------------------------------------------------
//  testviewerapplication.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tools/testviewer/testviewerapplication.h"
#include "frame/frameserver.h"
#include "coregraphics/debugview.h"
#include "input/inputserver.h"
#include "input/keyboard.h"

namespace Tools
{
using namespace Graphics;
using namespace Math;
using namespace Util;
using namespace Lighting;
using namespace Resources;
using namespace Timing;
#ifdef PHYSX
using namespace PhysX;
#endif
using namespace Input;

//------------------------------------------------------------------------------
/**
*/
TestViewerApplication::TestViewerApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TestViewerApplication::~TestViewerApplication()
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
TestViewerApplication::Open()
{
    n_assert(!this->IsOpen());
    if (ViewerApplication::Open())
    {
		this->animTable = Anim::AnimTable::Create();
		this->animTable->Open();

        // setup lights
        matrix44 lightTransform = matrix44::rotationx(n_deg2rad(-70.0f));
        this->globalLight = GlobalLightEntity::Create();
        this->globalLight->SetTransform(lightTransform);
        this->globalLight->SetColor(float4(1.0f, 1.0f, 1.5f, 1.0f));
        //this->globalLight->SetBackLightColor(float4(0.0f, 0.0f, 0.0f, 0.0f));
        this->globalLight->SetCastShadows(false);
        this->stage->AttachEntity(this->globalLight.upcast<GraphicsEntity>());

        /*lightTransform = matrix44::multiply(matrix44::scaling(75.0f, 75.0f, 100.0f), matrix44::lookatrh(point(20.0f, 20.0f, 20.0f), point::origin(), vector::upvec()));
        this->localLight0 = SpotLightEntity::Create();
        this->localLight0->SetTransform(lightTransform);
        this->localLight0->SetColor(float4(1.0f, 1.0f, 1.0f, 0.0f));        
        this->localLight0->SetCastShadows(false);
        this->stage->AttachEntity(this->localLight0.upcast<GraphicsEntity>());

        lightTransform = matrix44::multiply(matrix44::scaling(75.0f, 75.0f, 100.0f), matrix44::lookatrh(point(-20.0f, 20.0f, 20.0f), point::origin(), vector::upvec()));
        this->localLight1 = SpotLightEntity::Create();
        this->localLight1->SetTransform(lightTransform);
        this->localLight1->SetColor(float4(4.0f, 1.0f, 1.0f, 0.0f));
        this->localLight1->SetCastShadows(false);
        this->stage->AttachEntity(this->localLight1.upcast<GraphicsEntity>());*/
		
		this->view->SetFrameShader(Frame::FrameServer::Instance()->GetFrameShaderByName(ResourceId("DX9Sample")));

		ResourceId modelResId;
		if (this->args.HasArg("-n2"))
		{
			modelResId = this->args.GetString("-n2");
			String fileName = modelResId.Value();

			lightTransform = matrix44::scaling(0.5f, 0.5f, 0.5f);
			//if (fileName.CheckStringExist("characters"))
			{
				this->actor = ActorEntity::Create();
				this->actor->SetResourceId(modelResId);
				this->actor->SetTransform(lightTransform);
				this->stage->AttachEntity(this->actor.upcast<GraphicsEntity>());

				this->actor->SetBaseAnimation("attack");
			}
			//else
			//{
			//	this->head = ModelEntity::Create();
			//	//this->head->SetTransform(matrix44::translation(0.0f, 3.0f, 0.0f));
			//	this->head->SetResourceId(modelResId);
			//	this->head->SetTransform(lightTransform);
			//	this->stage->AttachEntity(this->head.upcast<GraphicsEntity>());
			//}
		}

        // setup models
        /*this->ground = ModelEntity::Create();
        this->ground->SetResourceId(ResourceId("mdl:examples/ground.n2"));
        this->stage->AttachEntity(ground.upcast<GraphicsEntity>());*/



		//lightTransform = matrix44::scaling(0.5f, 0.5f, 0.5f);
		//this->head = ModelEntity::Create();
  //      //this->head->SetTransform(matrix44::translation(0.0f, 3.0f, 0.0f));
  //      this->head->SetResourceId(ResourceId("mdl:characters/trollkroete.n2"));
		//this->head->SetTransform(lightTransform);
  //      this->stage->AttachEntity(this->head.upcast<GraphicsEntity>());



		// wow:World\\AZEROTH\\BootyBay\\PassiveDoodad\\FishingBox\\FishingBox.m2
		// wow:creature\\deer\\deer.m2  Character\\Bloodelf\\male\\bloodelfmale.m2
		this->tree = ModelEntity::Create();
		this->tree->SetResourceId(ResourceId("wow:Character\\Bloodelf\\male\\bloodelfmale.m2"));
		//this->tree->SetResourceId(ResourceId("mdl:characters/mensch_m.n2"));
		this->stage->AttachEntity(this->tree.upcast<GraphicsEntity>());
		
		this->worldManager = WOW::WorldManager::Create();
		this->worldManager->Open();
		this->worldManager->InitWorld("Azeroth");

#ifdef PHYSX		
		this->physxServer = PhysXServer::Create();
		this->physxServer->Open();
		//this->physxServer->Pause(true);
		//this->physxServer->CreateGroundPlane();
		this->physxServer->CreateMeshFromFile("msh:examples/ground_c_0.nvx2");
		this->physxServer->CreateBox(Math::vector(0.0f, 3.0f, 0.0f), Math::vector(0.5f, 0.5f, 0.5f));
		this->physxServer->CreateCapsule(Math::vector(1.0f, 3.0f, 0.0f), 0.55f, 0.75f);
		this->physxServer->CreateSphere(Math::vector(-1.0f, 3.0f, 0.0f), 0.65f);
		this->physxServer->AddUserDataToActor();
		this->view->ShowDebugPhysX();
#endif

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
TestViewerApplication::Close()
{
    this->globalLight = 0;
    this->localLight0 = 0;
    this->localLight1 = 0;
    this->ground = 0;
    this->head   = 0;
	this->tree	 = 0;
	this->actor  = 0;
	if (this->animTable.isvalid())
	{
		//this->animTable->Close();
		this->animTable = 0;
	}
	this->worldManager->Close();
	this->worldManager = 0;

#ifdef PHYSX
	if (this->physxServer.isvalid())
	{
		this->physxServer->Close();
		this->physxServer = 0;
	}
#endif

    ViewerApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
TestViewerApplication::OnUpdateFrame()
{
	Timing::Timer tt;
	tt.Reset();
	tt.Start();

	this->worldManager->Update();

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
	static bool b = false;
	if (this->actor.isvalid() && this->actor->IsValid() && !b)
	{
		b = true;
		this->actor->SetBaseAnimation("angriff_bogen_shoot");
	}

#ifdef PHYSX
	this->physxServer->Update((float)this->frameTime);
#endif

    ViewerApplication::OnUpdateFrame();
}

void
TestViewerApplication::OnProcessInput()
{
    InputServer* inputServer = InputServer::Instance();
    const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();

#ifdef PHYSX
    if (keyboard->KeyDown(Key::Add))
    {
        this->physxServer->CreateBox(Math::vector(0.0f, 5.0f, 0.0f), Math::vector(0.5f, 0.5f, 0.5f));
    }
#endif

	ViewerApplication::OnProcessInput();
}

} // namespace Tools
