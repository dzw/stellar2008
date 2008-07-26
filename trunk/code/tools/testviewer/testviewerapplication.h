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
#include "terrain/worldserver.h"

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

    Ptr<Graphics::ModelEntity> ground;
    Ptr<Graphics::ModelEntity> head;
	Ptr<Graphics::ModelEntity> tree;
    Ptr<Lighting::GlobalLightEntity> globalLight;
    Ptr<Lighting::SpotLightEntity> localLight0;
    Ptr<Lighting::SpotLightEntity> localLight1;

    Ptr<Terrain::WorldServer> worldServer;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
