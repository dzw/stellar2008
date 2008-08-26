//------------------------------------------------------------------------------
//  KokShapeNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/kokshapenodeinstance.h"
#include "kok/model/kokshapenode.h"
#include "coregraphics/transformdevice.h"

namespace KOK
{
ImplementClass(KOK::KokShapeNodeInstance, 'KSNI', Models::TransformNodeInstance);

using namespace Models;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
KokShapeNodeInstance::KokShapeNodeInstance()	
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
KokShapeNodeInstance::~KokShapeNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNodeInstance::Render()
{
	this->modelNode.downcast<KokShapeNode>()->Render();
}    

void
KokShapeNodeInstance::ApplyState()
{
	TransformNodeInstance::ApplyState();

	TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
KokShapeNodeInstance::Update()
{
	TransformNodeInstance::Update();
}

}
