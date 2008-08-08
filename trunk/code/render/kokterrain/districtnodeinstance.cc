//------------------------------------------------------------------------------
//  shapenodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/districtnodeinstance.h"
#include "coregraphics/renderdevice.h"
#include "models/modelnode.h"

namespace KOK
{
ImplementClass(KOK::DistrictNodeInstance, 'DTNI', Models::StateNodeInstance);

using namespace CoreGraphics;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
DistrictNodeInstance::DistrictNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
DistrictNodeInstance::~DistrictNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
DistrictNodeInstance::Render()
{
    StateNodeInstance::Render();
	RenderDevice::Instance()->Draw();
}    

//------------------------------------------------------------------------------
/**
*/
void
DistrictNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
    // just tell our model node that we are a visible instance
    this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

} // namespace Models
