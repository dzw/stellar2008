//------------------------------------------------------------------------------
//  ThingNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/Thingnode.h"
#include "kok/model/Thingnodeinstance.h"
#include "coregraphics/shaderserver.h"
#include "models/attributes.h"
#include "coregraphics/shaderstate.h"

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
using namespace Attr;

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
    KokShapeNode::LoadResources();
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
	if (!KokShapeNode::ApplySharedState())
		return false;

	ShaderServer* shdServer = ShaderServer::Instance();
	shdServer->SetFeatureBits(shdServer->FeatureStringToMask("Static"));

	return true;
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

	//// 创建mesh
	//Util::Array<CoreGraphics::VertexComponent> components;
	//components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	//components.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
	////components.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4));
	//components.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	//CreateMesh(components, sizeof(VertexFVF));

	if (this->m_dwNumMaterial > 0)
	{
		DWORD alphaBlendType = this->m_pMaterial[0].GetAlphaBlendType();
		if ((alphaBlendType > 0 && alphaBlendType <= 4))
		{
			this->SetType(ModelNodeType::Alpha);

			int srcBlend, destBlend, cullMode;
			switch(alphaBlendType)
			{
			case 1:
				srcBlend  = Blend_SrcAlpha;
				destBlend = Blend_One;		
				cullMode  = Cull_NoCull;
				break;
			case 2:
				srcBlend  = Blend_SrcAlpha;
				destBlend = Blend_InvSrcAlpha;
				cullMode  = Cull_NoCull;
				break;
			case 3:
				srcBlend = Blend_Zero;
				destBlend = Blend_SrcColor;
				cullMode = Cull_NoCull;
				break;
			case 4:
				srcBlend = Blend_Zero;
				destBlend = Blend_InvSrcColor;
				cullMode = Cull_NoCull;
				break;
			}
			this->SetInt(Attr::SrcAlphaBlend, srcBlend);
			this->SetInt(Attr::DestAlphaBlend, destBlend);
			this->SetInt(Attr::CullMode, cullMode);
		}
	}

	if (!this->shaderInstance.isvalid())
		this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(Resources::ResourceId("shd:kokstaticmodel"));
}

}