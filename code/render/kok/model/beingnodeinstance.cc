//------------------------------------------------------------------------------
//  BeingNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/beingnodeinstance.h"
#include "kok/model/beingnode.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/shaderserver.h"
#include "math/matrix44.h"

#include "kok/model/animation/cSkeletonSerializer.h"
#include "kok/model/animation/cskinweights.h"
#include "kok/model/beingentity.h"
#include "kok/model/kokshapenode.h"

#include "resources/resourcemanager.h"
#include "coregraphics/renderdevice.h"

namespace KOK
{
ImplementClass(KOK::BeingNodeInstance, 'BEIN', KOK::KokShapeNodeInstance);

using namespace Models;
using namespace CoreGraphics;
using namespace Math;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
BeingNodeInstance::BeingNodeInstance():
	jointPaletteVar(0),
	fakeReflectMap(0),
	fakeReflectLightMap(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BeingNodeInstance::~BeingNodeInstance()
{
	this->jointPaletteVar = 0;
	this->fakeReflectMap = 0;
	this->fakeReflectLightMap = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
BeingNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(node->IsA(BeingNode::RTTI));

	KokShapeNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	CreateMaterial();
}

//------------------------------------------------------------------------------
/**
*/
void
BeingNodeInstance::Render()
{
	static const int maxJointPaletteSize = 72;
	static matrix44 jointArray[maxJointPaletteSize];

	const Ptr<BeingEntity>& beingEntity = this->GetModelInstance()->GetModelEntity().downcast<BeingEntity>();
	if (!beingEntity.isvalid())
		return;

	// 当需要画的时候才更新骨骼，而且只更新一次，因为部件是以model为单位的，所以只能调用一次更新
	beingEntity->UpdateSkeleton();
	

	cSkeletonSerializer* skeletonSerializer = beingEntity->GetSkeleton();
	const Ptr<BeingNode>& node = this->modelNode.downcast<BeingNode>();
	cSkinWeights* skins = node->GetSkinWeights();
	DWORD skinNum = node->GetSkinWeightNum();
	D3DXMATRIXA16 tmpMat;
	for (SizeT i = 0; i < skinNum; i++)
	{
		D3DXMATRIXA16* mat = skins[i].getSkinWeightsSkeletonMatrix();
		if (skeletonSerializer != NULL)
		{
			cSkeletonHierarchy* sk = skeletonSerializer->findSkeletonHierarchy((char*)skins[i].getSkinWeightsSkeletonName());
			if (sk != NULL)
			{
				tmpMat = (*mat) * (*sk->getSkeletonHierarchyMatrix());
			}
			else
				tmpMat = *mat;
		}
		else
		{
			tmpMat = *mat;
		}
		jointArray[i].set(float4(tmpMat._11, tmpMat._12, tmpMat._13, tmpMat._14 ),
						float4(tmpMat._21, tmpMat._22, tmpMat._23, tmpMat._24 ),
						float4(tmpMat._31, tmpMat._32, tmpMat._33, tmpMat._34 ),
						float4(tmpMat._41, tmpMat._42, tmpMat._43, tmpMat._44 ));
	}

	// test
	/*for (int i = 0; i < 72; i++)
	{
		jointArray[i] = matrix44::identity();
	}*/

	// transfer the joint palette to the current shader
	n_assert(jointPaletteVar.isvalid());
	jointPaletteVar->SetMatrixArray(jointArray, skinNum);    



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
	//const Ptr<ManagedTexture>& fakeReflect = ResourceManager::Instance()->LookupManagedResource(ResourceId("FakeReflectTexture")).downcast<ManagedTexture>();
	const Ptr<ManagedTexture>& fakeReflect = beingEntity->GetFakeReflectTexture();
	if (fakeReflect.isvalid() && fakeReflect->GetState() == Resource::Loaded)
		this->fakeReflectLightMap->SetTexture(fakeReflect->GetTexture());
	else
		return;

	// 假反光效果
	shaderServer->SetFeatureBits(shaderServer->FeatureStringToMask("Reflect"));
	shaderInstance->SelectActiveVariation(shaderServer->GetFeatureBits());
	numPasses = shaderInstance->Begin();
	n_assert(1 == numPasses);
	shaderInstance->BeginPass(0);

	for (IndexT batchIndex = 0; batchIndex < node->GetAttribuateTableSize(); batchIndex++)
	{
		RenderBatch(batchIndex, true);
	}

	shaderInstance->EndPass();
	shaderInstance->End();

	//this->modelNode.downcast<BeingNode>()->Render();
}    

//------------------------------------------------------------------------------
/**
*/
void
BeingNodeInstance::RenderBatch(IndexT index, bool reflectRender)
{
	const Ptr<BeingNode>& node = this->modelNode.downcast<BeingNode>();
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
BeingNodeInstance::ApplyState()
{
	KokShapeNodeInstance::ApplyState();

	TransformDevice* transformDevice = TransformDevice::Instance();
	transformDevice->ApplyModelTransforms();
}

//------------------------------------------------------------------------------
/**
*/
void
BeingNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
BeingNodeInstance::Update()
{
	KokShapeNodeInstance::Update();
}

void
BeingNodeInstance::CreateMaterial()
{
	this->fakeReflectMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("FakeReflect"));
	this->fakeReflectLightMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("FakeReflectLight"));
	this->jointPaletteVar = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("JointPalette"));
}

}
