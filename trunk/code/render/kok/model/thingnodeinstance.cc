//------------------------------------------------------------------------------
//  ThingNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/thingnodeinstance.h"
#include "kok/model/thingnode.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/renderdevice.h"
#include "models/attributes.h"

namespace KOK
{
ImplementClass(KOK::ThingNodeInstance, 'THIN', KOK::KokShapeNodeInstance);

using namespace Models;
using namespace CoreGraphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
ThingNodeInstance::ThingNodeInstance():
	cullMode(0),
	srcBlend(0),
	destBlend(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ThingNodeInstance::~ThingNodeInstance()
{
	this->cullMode = 0;
	this->srcBlend = 0;
	this->destBlend = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ThingNodeInstance::Render()
{
	//this->modelNode.downcast<ThingNode>()->Render();

	const Ptr<ThingNode>& node = this->modelNode.downcast<ThingNode>();

	if (node->GetType() == ModelNodeType::Alpha)
	{
		this->cullMode->SetInt(node->GetInt(Attr::CullMode));
		this->srcBlend->SetInt(node->GetInt(Attr::SrcAlphaBlend));
		this->destBlend->SetInt(node->GetInt(Attr::DestAlphaBlend));
	}

	for (IndexT batchIndex = 0; batchIndex < node->GetAttribuateTableSize(); batchIndex++)
	{
		RenderBatch(batchIndex);
	}
}    

void
ThingNodeInstance::ApplyState()
{
	KokShapeNodeInstance::ApplyState();

	//TransformDevice* transformDevice = TransformDevice::Instance();
	//transformDevice->ApplyModelTransforms();
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

//------------------------------------------------------------------------------
/**
*/
void
ThingNodeInstance::RenderBatch(IndexT index)
{
	const Ptr<ThingNode>& node = this->modelNode.downcast<ThingNode>();
	const AttributeRange& batch = node->GetAttributeTable()[index];
	const cMaterial& material = node->GetMaterial()[index];
	const Ptr<Texture>& tex = this->GetTexture(index);

	if (batch.FaceCount <= 0 || batch.VertexCount <= 0 || !tex.isvalid())
		return;

	this->diffuseColor->SetVector(float4(
		material.m_D3DMaterial.Diffuse.r,
		material.m_D3DMaterial.Diffuse.g,
		material.m_D3DMaterial.Diffuse.b,
		material.m_D3DMaterial.Diffuse.a));
	this->ambientColor->SetVector(float4(
		material.m_D3DMaterial.Ambient.r,
		material.m_D3DMaterial.Ambient.g,
		material.m_D3DMaterial.Ambient.b,
		material.m_D3DMaterial.Ambient.a));
	this->specularColor->SetVector(float4(
		material.m_D3DMaterial.Specular.r,
		material.m_D3DMaterial.Specular.g,
		material.m_D3DMaterial.Specular.b,
		material.m_D3DMaterial.Specular.a));
	this->emissiveColor->SetVector(float4(
		material.m_D3DMaterial.Emissive.r,
		material.m_D3DMaterial.Emissive.g,
		material.m_D3DMaterial.Emissive.b,
		material.m_D3DMaterial.Emissive.a));
	this->diffMap->SetTexture(tex);


	this->shaderInstance->Commit();

	node->GetMesh()->ApplyPrimitives(index);
	RenderDevice::Instance()->Draw();
}

//------------------------------------------------------------------------------
/**
	有些模型只是拿顶点数据用的，不需要显示，所以不要在NODE中把所有的子模型都创建
	MESH，在这里为需要显示的子模型创建MESH（THING中只有m_repeat和m_hd两种类型是
	需要显示的，而前者更是将多个子模型放在一起，只显示其中的一个！）
*/
void 
ThingNodeInstance::CreateMesh()
{
	const Ptr<ThingNode>& node = this->modelNode.downcast<ThingNode>();
	if (node->GetMesh().isvalid())
		return;

	// 创建mesh
	Util::Array<CoreGraphics::VertexComponent> components;
	components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	components.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
	//components.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4));
	components.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	node->CreateMesh(components, sizeof(VertexFVF));
}

void 
ThingNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst,
										   const Ptr<ModelNode>& node, 
										   const Ptr<ModelNodeInstance>& parentNodeInst)
{
	KokShapeNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	if (node->GetType() == ModelNodeType::Alpha)
	{
		this->cullMode = shaderInstance->GetVariableByName(ShaderVariable::Name("CullMode"));
		this->srcBlend = shaderInstance->GetVariableByName(ShaderVariable::Name("AlphaSrcBlend"));
		this->destBlend = shaderInstance->GetVariableByName(ShaderVariable::Name("AlphaDestBlend"));
	}
}

}
