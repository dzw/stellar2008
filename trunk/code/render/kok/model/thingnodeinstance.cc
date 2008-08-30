//------------------------------------------------------------------------------
//  ThingNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/thingnodeinstance.h"
#include "kok/model/thingnode.h"
#include "coregraphics/transformdevice.h"

namespace KOK
{
ImplementClass(KOK::ThingNodeInstance, 'THIN', KOK::KokShapeNodeInstance);

using namespace Models;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ThingNodeInstance::ThingNodeInstance()	
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ThingNodeInstance::~ThingNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
void
ThingNodeInstance::Render()
{
	this->modelNode.downcast<ThingNode>()->Render();
}    

void
ThingNodeInstance::ApplyState()
{
	KokShapeNodeInstance::ApplyState();

	TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();
}

//------------------------------------------------------------------------------
/**
*/
void
ThingNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
ThingNodeInstance::Update()
{
	KokShapeNodeInstance::Update();
}

}
