//------------------------------------------------------------------------------
//  ThingNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/Thingnode.h"
#include "kok/model/Thingnodeinstance.h"

namespace KOK
{
ImplementClass(KOK::ThingNode, 'THNE', KOK::KokShapeNode);

using namespace IO;
using namespace Util;
using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Memory;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ThingNode::ThingNode()
{
}

//------------------------------------------------------------------------------
/**
*/
ThingNode::~ThingNode()
{
	
}

//------------------------------------------------------------------------------
/**
*/
Resource::State
ThingNode::GetResourceState() const
{
	return Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
void
ThingNode::LoadResources()
{
    // call parent class
    TransformNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
ThingNode::UnloadResources()
{    
    KokShapeNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
ThingNode::CreateNodeInstance()const
{
	Ptr<ModelNodeInstance> instance = (ModelNodeInstance*)ThingNodeInstance::Create();
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
bool
ThingNode::ApplySharedState()
{
	return KokShapeNode::ApplySharedState();
}

//------------------------------------------------------------------------------
/**
*/
void
ThingNode::LoadFromStream(const Ptr<Stream>& stream, 
							 int iVersion, 
							 bool bMirrorZ, 
							 bool bCompuiteDuplicateVertexCollection,
							 bool bBlend)
{
	KokShapeNode::LoadFromStream(stream, iVersion, bMirrorZ, bCompuiteDuplicateVertexCollection, bBlend);

	// 这里重新计算摇动所需的顶点数据

	// 创建mesh
	Util::Array<CoreGraphics::VertexComponent> components;
	components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	components.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
	//components.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4));
	components.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	CreateMesh(components, sizeof(VertexFVF));
}

}