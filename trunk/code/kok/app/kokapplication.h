#pragma once
#ifndef KOK_KOKAPPLICATION_H
#define KOK_KOKAPPLICATION_H
//------------------------------------------------------------------------------
/**
    @class KOK::KOKApplication
    (C) 2008 cTuo
*/
#include "apprender/viewerapplication.h"
//#include "kok/terrain/terrainserver.h"
#include "kok/terrain/terrainentity.h"
#include "kok/model/thingentity.h"
#include "kok/model/beingentity.h"
#include "renderutil/dxutcamerautil.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "graphics/cameraentity.h"

//------------------------------------------------------------------------------
namespace KOK
{
class KOKApplication : public App::RenderApplication
{
public:
    /// constructor
    KOKApplication();
    /// destructor
    virtual ~KOKApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

	virtual void OnRenderFrame();
private:
    /// render current frame
    virtual void OnUpdateFrame();
	virtual void OnProcessInput();
	/// load lpq file
	void LoadLPQFile();

	Ptr<Graphics::Stage> stage;
	Ptr<Graphics::View> view;
	Ptr<Graphics::CameraEntity> cameraEntity;

	//Ptr<TerrainServer> terrainServer;
	Ptr<TerrainEntity> terrainEntity;
	Ptr<ThingEntity> thingEntity;
	Ptr<BeingEntity> beingEntity;
	RenderUtil::DXUTCameraUtil dxutCameraUtil;

	
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
