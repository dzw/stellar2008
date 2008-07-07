//------------------------------------------------------------------------------
//  TransformNode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2characternode.h"
#include "wow/m2/m2characternodeinstance.h"


namespace WOW
{
ImplementClass(WOW::M2CharacterNode, 'M2CH', Models::CharacterNode);

using namespace Util;
using namespace Attr;
using namespace Math;
using namespace Resources;
using namespace CoreGraphics;
using namespace Char;
using namespace Anim;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
M2CharacterNode::M2CharacterNode()
{
	this->character = M2Character::Create();
}

//------------------------------------------------------------------------------
/**
*/
M2CharacterNode::~M2CharacterNode()
{
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
M2CharacterNode::CreateNodeInstance() const
{    
	Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) M2CharacterNodeInstance::Create();
	return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNode::LoadResources()
{
	// call parent class, creates mesh
	TransformNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNode::UnloadResources()
{
	this->character = 0;

	// call parent class
	TransformNode::UnloadResources();
}

} // namespace Models