#pragma once
#ifndef APP_VIEWERAPPLICATION_H
#define APP_VIEWERAPPLICATION_H
//------------------------------------------------------------------------------
/**
    @class App::ViewerApplication

    Nebula3's default viewer application.

    (C) 2007 Radon Labs GmbH
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

//------------------------------------------------------------------------------
namespace App
{
class ViewerApplication : public RenderApplication
{
public:
    /// constructor
    ViewerApplication();
    /// destructor
    virtual ~ViewerApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

protected:
    /// process input (called before rendering)
    virtual void OnProcessInput();
    /// render current frame
    virtual void OnRenderFrame();
	/// render font
	void RenderDebugString();
	/// pick mouse
	void Pick(const Math::float2& cursor, Math::vector* rayOrig, Math::vector* rayDir)const;

    Resources::ResourceId resId;
    Ptr<Frame::FrameShader> frameShader;
    Ptr<Graphics::Stage> stage;
    Ptr<Graphics::View> view;
    Ptr<Graphics::CameraEntity> cameraEntity;
    Ptr<Graphics::ModelEntity> modelEntity;
    Ptr<Lighting::SpotLightEntity> lightEntity;
    RenderUtil::MayaCameraUtil mayaCameraUtil;
	RenderUtil::DXUTCameraUtil dxutCameraUtil;
	//RenderUtil::QuaternionCameraUtil quaternionCameraUtil;

	RenderUtil::FreeCameraUtil freeCameraUtil;

	Util::Array<Util::String> pickEntities;
	bool isDebugRender;
};

} // namespace App
//------------------------------------------------------------------------------
#endif
