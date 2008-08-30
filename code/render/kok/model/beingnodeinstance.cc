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

namespace KOK
{
ImplementClass(KOK::BeingNodeInstance, 'BEIN', KOK::KokShapeNodeInstance);

using namespace Models;
using namespace CoreGraphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
BeingNodeInstance::BeingNodeInstance():
	jointPaletteVar(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BeingNodeInstance::~BeingNodeInstance()
{
	this->jointPaletteVar = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
BeingNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(node->IsA(BeingNode::RTTI));

	KokShapeNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	// create palette shader variable instance
	ShaderVariable::Semantic paletteSemantic = ShaderVariable::Semantic("JointPalette");
	this->jointPaletteVar = CreateShaderVariableInstance(paletteSemantic);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ShaderVariableInstance>
BeingNodeInstance::CreateShaderVariableInstance(const ShaderVariable::Semantic& sem)
{
	const Ptr<ShaderInstance>& shaderInstance = this->modelNode.downcast<KokShapeNode>()->GetShaderInstance();
	n_assert(shaderInstance.isvalid());

	// create new shader variable instance
	n_assert(shaderInstance->HasVariableBySemantic(sem));
	const Ptr<ShaderVariable>& var = shaderInstance->GetVariableBySemantic(sem);
	Ptr<ShaderVariableInstance> varInst = var->CreateInstance();
	return varInst;
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
	const Ptr<KokShapeNode>& node = this->modelNode.downcast<KokShapeNode>();
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
	jointPaletteVar->Apply();

	// commit shader variable changes
	CoreGraphics::ShaderServer* shdServer = CoreGraphics::ShaderServer::Instance();
	shdServer->GetActiveShaderInstance()->Commit();


	this->modelNode.downcast<BeingNode>()->Render();
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

}
