#pragma once
#ifndef TOOLS_TESTVIEWER_H
#define TOOLS_TESTVIEWER_H
//------------------------------------------------------------------------------
/**
    @class Tools::TestViewer
    
    Nebula3 test viewer app.

    (C) 2006 Radon Labs GmbH
*/
#include "apprender/viewerapplication.h"
#include "lighting/spotlightentity.h"
#include "graphics/modelentity.h"
#include "wow/world/worldmanager.h"
#include "graphics/actorentity.h"
#include "addons\nebula2\nebula2wrapper.h"
#ifdef PHYSX
#include "addons/physx/physxserver.h"
#endif
//------------------------------------------------------------------------------
namespace Tools
{
class TestViewerApplication : public App::ViewerApplication
{
public:
    /// constructor
    TestViewerApplication();
    /// destructor
    virtual ~TestViewerApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

private:
    /// render current frame
    virtual void OnUpdateFrame();
	virtual void OnProcessInput();

    Ptr<Graphics::ModelEntity> ground;
    Ptr<Graphics::ModelEntity> head;
	Ptr<Graphics::ModelEntity> tree;
	Ptr<Graphics::ActorEntity> actor;
    Ptr<Lighting::GlobalLightEntity> globalLight;
    Ptr<Lighting::SpotLightEntity> localLight0;
    Ptr<Lighting::SpotLightEntity> localLight1;
	Ptr<Anim::AnimTable> animTable;
	Ptr<WOW::WorldManager> worldManager;
#ifdef PHYSX
	Ptr<PhysX::PhysXServer> physxServer;
#endif
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
