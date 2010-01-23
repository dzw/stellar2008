//------------------------------------------------------------------------------
//  shapenodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2characternodeinstance.h"
//#include "coregraphics/renderdevice.h"
//#include "models/nodes/characternode.h"
//#include "models/modelinstance.h"
//#include "nebula2/anim/managedanimation.h"
//#include "timing/timer.h"
//#include "coregraphics/shaperenderer.h"
#include "wow/m2/m2characternode.h"
#include "models/modelinstance.h"

namespace WOW
{
ImplementClass(WOW::M2CharacterNodeInstance, 'M2CI', Models::TransformNodeInstance);

using namespace CoreGraphics;
using namespace Math;
using namespace Models;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
M2CharacterNodeInstance::M2CharacterNodeInstance():
curAnim(54)
{
}

//------------------------------------------------------------------------------
/**
*/
M2CharacterNodeInstance::~M2CharacterNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
M2CharacterNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
    TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);
    n_assert(node->IsA(M2CharacterNode::RTTI));

	SizeT jointSize = node.downcast<M2CharacterNode>()->GetJoints().Size();
	this->joints.SetSize(jointSize);
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNodeInstance::OnRemoveFromModelInstance()
{

    TransformNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNodeInstance::Update()
{    
    TransformNodeInstance::Update();

    const Ptr<M2CharacterNode>& charNode = GetModelNode().downcast<M2CharacterNode>();
    if (charNode.isvalid())
    {
		static int lastTime=0;
		int ddt = 0;
		ddt = (timeGetTime() - lastTime);// * animSpeed;
		lastTime = timeGetTime();

		//float tt = modelInstance->GetTime();	// 要换成整型timeGetTime
		charNode->UpdataBones(curAnim, ddt);

		const FixedArray<M2CharJoint>& j = charNode->GetJoints();
		for (SizeT i = 0; i < joints.Size(); i++)
		{
			this->joints[i].mat = matrix44::transpose(j[i].GetMatrix());
			this->joints[i].mrot = matrix44::transpose(j[i].GetRotateMatrix());
		}
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
M2CharacterNodeInstance::RenderDebug()
{
}

} // namespace Models
