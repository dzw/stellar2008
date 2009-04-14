//------------------------------------------------------------------------------
//  testviewerapplication.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tools/testviewer/testviewerapplication.h"
#include "frame/frameserver.h"
#include "coregraphics/debugview.h"

namespace Tools
{
using namespace Graphics;
using namespace Math;
using namespace Util;
using namespace Lighting;
using namespace Resources;
using namespace Timing;

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

        // setup models
        /*this->ground = ModelEntity::Create();
        this->ground->SetResourceId(ResourceId("mdl:examples/ground.n2"));
        this->stage->AttachEntity(ground.upcast<GraphicsEntity>());*/

		lightTransform = matrix44::scaling(0.5f, 0.5f, 0.5f);
		this->head = ModelEntity::Create();
        //this->head->SetTransform(matrix44::translation(0.0f, 3.0f, 0.0f));
        this->head->SetResourceId(ResourceId("mdl:characters/trollkroete.n2"));
		this->head->SetTransform(lightTransform);
        this->stage->AttachEntity(this->head.upcast<GraphicsEntity>());

		// wow:World\\AZEROTH\\BootyBay\\PassiveDoodad\\FishingBox\\FishingBox.m2
		// wow:creature\\deer\\deer.m2
		//this->tree = ModelEntity::Create();
		//this->tree->SetResourceId(ResourceId("wow:creature\\deer\\deer.m2"));
		//this->tree->SetResourceId(ResourceId("mdl:characters/mensch_m.n2"));
		//this->stage->AttachEntity(this->tree.upcast<GraphicsEntity>());
		
		/*this->worldManager = WOW::WorldManager::Create();
		this->worldManager->Open();
		this->worldManager->InitWorld("Azeroth");*/

		
		
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

	//this->worldManager->Close();
	//this->worldManager = 0;

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
    ViewerApplication::OnUpdateFrame();
}

} // namespace Tools
