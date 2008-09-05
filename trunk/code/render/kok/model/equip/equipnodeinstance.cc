//------------------------------------------------------------------------------
//  EquipNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/equip/equipnodeinstance.h"
#include "kok/model/equip/equipnode.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/shaderserver.h"
#include "math/matrix44.h"

#include "kok/model/kokshapenode.h"

#include "resources/resourcemanager.h"
#include "coregraphics/renderdevice.h"
#include "models/modelinstance.h"

namespace KOK
{
ImplementClass(KOK::EquipNodeInstance, 'EQIN', KOK::KokShapeNodeInstance);

using namespace Models;
using namespace CoreGraphics;
using namespace Math;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
EquipNodeInstance::EquipNodeInstance()	
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EquipNodeInstance::~EquipNodeInstance()
{
	this->fakeReflectMap = 0;
	this->fakeReflectLightMap = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
EquipNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(node->IsA(EquipNode::RTTI));

	KokShapeNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	CreateMaterial();
}

//------------------------------------------------------------------------------
/**
*/
void
EquipNodeInstance::Render()
{
	const Ptr<EquipNode>& node = this->modelNode.downcast<EquipNode>();

	this->modelTransform = matrix44::multiply(node->GetTransform(), this->modelInstance->GetTransform());
	TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();




	ShaderServer* shaderServer = ShaderServer::Instance();
	shaderServer->SetFeatureBits(shaderServer->FeatureStringToMask("Skin"));
	shaderInstance->SelectActiveVariation(shaderServer->GetFeatureBits());
	SizeT numPasses = shaderInstance->Begin();
	n_assert(1 == numPasses);
	shaderInstance->BeginPass(0);

	// 先渲染没有假反光的batch
	for (IndexT batchIndex = 0; batchIndex < node->GetAttribuateTableSize(); batchIndex++)
	{
		RenderBatch(batchIndex, false);
	}

	shaderInstance->EndPass();
	shaderInstance->End();



	// 渲染带假反光的batch
	// 假反光光照贴图
	//const Ptr<ManagedTexture>& fakeReflect = beingEntity->GetFakeReflectTexture();
	//if (fakeReflect.isvalid() && fakeReflect->GetState() == Resource::Loaded)
	//	this->fakeReflectLightMap->SetTexture(fakeReflect->GetTexture());
	//else
	//	return;

	//// 假反光效果
	//shaderServer->SetFeatureBits(shaderServer->FeatureStringToMask("Reflect"));
	//shaderInstance->SelectActiveVariation(shaderServer->GetFeatureBits());
	//numPasses = shaderInstance->Begin();
	//n_assert(1 == numPasses);
	//shaderInstance->BeginPass(0);

	//for (IndexT batchIndex = 0; batchIndex < node->GetAttribuateTableSize(); batchIndex++)
	//{
	//	RenderBatch(batchIndex, true);
	//}

	//shaderInstance->EndPass();
	//shaderInstance->End();
}    

//------------------------------------------------------------------------------
/**
*/
void
EquipNodeInstance::RenderBatch(IndexT index, bool reflectRender)
{
	const Ptr<EquipNode>& node = this->modelNode.downcast<EquipNode>();
	const AttributeRange& batch = node->GetAttributeTable()[index];
	const cMaterial& material = node->GetMaterial()[index];

	if (batch.FaceCount <= 0 || batch.VertexCount <= 0 || !material.GetTexture().isvalid())
		return;

	const Ptr<Texture>& fakeReflect = material.GetFakeReflectTexture();
	if (reflectRender) 
	{
		// 渲染假反光batch
		if (fakeReflect.isvalid())
			this->fakeReflectMap->SetTexture(fakeReflect);
		else
			return;
	}
	else
	{
		if (fakeReflect.isvalid())
			return;
	}
	

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
	this->diffMap->SetTexture(material.GetTexture());

	this->shaderInstance->Commit();

	node->GetMesh()->ApplyPrimitives(index);
	RenderDevice::Instance()->Draw();
}

void
EquipNodeInstance::ApplyState()
{
	KokShapeNodeInstance::ApplyState();

	/*TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();*/
}

//------------------------------------------------------------------------------
/**
*/
void
EquipNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
EquipNodeInstance::Update()
{
	KokShapeNodeInstance::Update();
}

void
EquipNodeInstance::CreateMaterial()
{
	this->fakeReflectMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("FakeReflect"));
	this->fakeReflectLightMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("FakeReflectLight"));
}

}
