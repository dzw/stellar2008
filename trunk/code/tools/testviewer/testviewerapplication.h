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
#include "kok/model/thingentity.h"
#include "kok/model/beingentity.h"
#include "kok/model/animation/cSkeletonSerializerFactory.h"

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
	void LoadLPQFile();
    /// render current frame
    virtual void OnUpdateFrame();

	void CreateSkeleton(const Ptr<KOK::BeingEntity>& entity, const Util::String& skeletonName);

	Ptr<KOK::ThingEntity> thingEntity;
	Ptr<KOK::BeingEntity> beingEntity;
	Ptr<KOK::cSkeletonSerializerFactory> skeletonFactory;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
