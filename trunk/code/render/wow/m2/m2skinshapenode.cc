//------------------------------------------------------------------------------
//  shapenode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2skinshapenode.h"
#include "wow/m2/m2skinshapenodeinstance.h"
#include "models/attributes.h"
#include "resources/resourcemanager.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shaderfeature.h"

namespace WOW
{
ImplementClass(WOW::M2SkinShapeNode, 'M2SN', Models::ShapeNode);

using namespace Util;
using namespace Attr;
using namespace Math;
using namespace Resources;
using namespace CoreGraphics;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
M2SkinShapeNode::M2SkinShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
M2SkinShapeNode::~M2SkinShapeNode()
{
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
M2SkinShapeNode::CreateNodeInstance() const
{
    Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) M2SkinShapeNodeInstance::Create();
    return newInst;
}

//------------------------------------------------------------------------------
/**
*/
bool
M2SkinShapeNode::ApplySharedState()
{     
    // set mesh
    ShapeNode::ApplySharedState();
   
    // set skinned shader feature
    ShaderServer* shdServer = ShaderServer::Instance();
    shdServer->SetFeatureBits(shdServer->FeatureStringToMask("Skinned"));

	return true;
}

//------------------------------------------------------------------------------
/**
    Do not load resources by default.
*/
void
M2SkinShapeNode::LoadResources()
{
    // nothing done
    ShapeNode::LoadResources();
}

} // namespace Models