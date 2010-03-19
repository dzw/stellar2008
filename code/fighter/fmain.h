#pragma once
#ifndef TOOLS_TESTVIEWER_H
#define TOOLS_TESTVIEWER_H
//------------------------------------------------------------------------------
/**
    @class Tools::TestViewer
    
    Nebula3 test viewer app.

    (C) 2006 Radon Labs GmbH
*/
#include "apprender/renderapplication.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "graphics/cameraentity.h"
#include "lighting/spotlightentity.h"
#include "graphics/modelentity.h"
#include "renderutil/mayacamerautil.h"
#include "renderutil/dxutcamerautil.h"
//#include "renderutil/quaternioncamerautil.h"
#include "renderutil/freecamerautil.h"
//#include "renderutil/chasecamerautil.h"

#include "fighter/FWorldManager.h"
#include "fighter/fcameramanager.h"
#include "fighter/fobjectmanager.h"
#include "fighter/finputmanager.h"
#include "fighter/fskillmanager.h"
#include "addons\nebula2\nebula2wrapper.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FighterApplication : public App::RenderApplication
{
public:
    /// constructor
    FighterApplication();
    /// destructor
    virtual ~FighterApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

private:
    /// render current frame
    virtual void OnUpdateFrame();
	/// process input (called before rendering)
	virtual void OnProcessInput();
	/// render current frame
	virtual void OnRenderFrame();
	/// render font
	void RenderDebugString();

    /*Ptr<Graphics::ModelEntity> ground;
    Ptr<Graphics::ModelEntity> head;
	Ptr<Graphics::ModelEntity> tree;
    Ptr<Lighting::GlobalLightEntity> globalLight;
    Ptr<Lighting::SpotLightEntity> localLight0;
    Ptr<Lighting::SpotLightEntity> localLight1;*/

	Resources::ResourceId resId;
	Ptr<Frame::FrameShader> frameShader;
	Ptr<Graphics::Stage> stage;
	Ptr<Graphics::View> view;
	Ptr<Graphics::CameraEntity> cameraEntity;
	Ptr<Graphics::ModelEntity> terrainEntity;
	Ptr<Graphics::ModelEntity> modelEntity;
	Ptr<Lighting::SpotLightEntity> lightEntity;
	RenderUtil::MayaCameraUtil mayaCameraUtil;
	RenderUtil::DXUTCameraUtil dxutCameraUtil;
	RenderUtil::FreeCameraUtil freeCameraUtil;
//	RenderUtil::ChaseCameraUtil chaseCaneraUtil;
	bool isDebugRender;

	Ptr<Fighter::FCameraManager> cameraManager;
	Ptr<Fighter::FWorldManager> worldManager;
	Ptr<Fighter::FObjectManager> objectManager;
	Ptr<Fighter::FInputManager> inputManager;
	Ptr<Fighter::FSkillManager> skillManager;
	Ptr<Anim::AnimTable> animTable;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
