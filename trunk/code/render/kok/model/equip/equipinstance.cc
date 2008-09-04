//------------------------------------------------------------------------------
//  terraininstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/equip/equipinstance.h"
#include "models/modelnode.h"
#include "models/modelnodeinstance.h"
#include "util/array.h"
#include "util/string.h"
#include "kok/model/equip/equipnode.h"
#include "kok/model/equip/equip.h"

namespace KOK
{
ImplementClass(KOK::EquipInstance, 'EPIE', Models::ModelInstance);

using namespace Models;
using namespace Graphics;
using namespace Util;
using namespace Math;

extern ID3DXMatrixStack *g_pStack;
//------------------------------------------------------------------------------
/**
*/
EquipInstance::EquipInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
EquipInstance::~EquipInstance()
{
}

void 
EquipInstance::OnAttachToModel(const Ptr<Model>& model)
{
	this->model = model;

	Ptr<ModelNodeInstance> tmp;
	const Array<Ptr<ModelNode> >& modelNodes = this->model->GetNodes();
	SizeT numModelNodes = modelNodes.Size();
	IndexT modelNodeIndex;
	for (modelNodeIndex = 0; modelNodeIndex < numModelNodes; modelNodeIndex++)
	{
		const Ptr<ModelNode>& modelNode = modelNodes[modelNodeIndex];

		// 不用创建特殊用处的子模型
		String name = modelNode->GetName().Value();
		if (name.CheckStringExist("m_shadow") ||		// 刀光用顶点数据
			name.CheckStringExist("e_flame") ||			// 
			name.CheckStringExist("b_00") ||			// 立方体，控制特效范围
			modelNode.downcast<EquipNode>()->GetMeshType() > 0)
			continue;

		Ptr<ModelNodeInstance> nodeInstance = modelNode->CreateNodeInstance();
		nodeInstance->OnAttachToModelInstance(this, modelNode, tmp);
		this->nodeInstances.Append(nodeInstance);
	}
}

//------------------------------------------------------------------------------
/**
    
*/
void
EquipInstance::Update()
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
EquipInstance::NotifyVisible(IndexT frameIndex)
{
	if (m_bForseHide)
		return;

	ModelInstance::NotifyVisible(frameIndex);
}

void 
EquipInstance::SetEquip( int iTextureID, bool bWeaponRibbonLerp )
{
	
}

void 
EquipInstance::TickAnimTime( float fElapsedTime )
{
	m_fAnimElapsedTime = fElapsedTime * 4800.0f;
	m_fCurrentAnimTime += m_fAnimElapsedTime;
}

//------------------------------------------------------------------------------
/**
	计算Animation
*/
void 
EquipInstance::ComputeAnimation()
{
	const Ptr<Equip>& equip = this->model.downcast<Equip>();

	if( equip->GetSkeletonHierarchyRoot() )
	{
		cAnimation* pAnimation = equip->GetAnimRoot();
		cAnimationActionInfoGroup* pActionInfoGroup = equip->GetActionInfoGroup();

		if( pAnimation && pActionInfoGroup && ( pActionInfoGroup->getAnimationActionInfoCount() > 0 ) )
		{
			float fAnimTime = 0.0f;
			cAnimationActionInfo* pActionInfo = pActionInfoGroup->getAnimationActionInfo( m_iActionIndex );
			if( pActionInfo )
			{
				float fStartTime = (float)pActionInfo->getAnimationActionStartTime();
				float fEndTime = (float)pActionInfo->getAnimationActionEndTime();
				if( m_fCurrentAnimTime > fEndTime )
				{
					fAnimTime = fmod( ( m_fCurrentAnimTime - fEndTime ), ( fEndTime - fStartTime ) );
					m_fCurrentAnimTime = fStartTime + fAnimTime;
				}
				else
				{
					fAnimTime = m_fCurrentAnimTime - fStartTime;
				}
			}

			g_pStack->Push();
			g_pStack->LoadIdentity();

			sComputeAnimParam param( m_iActionIndex, -1, fAnimTime, 0.0f, 0.0f );
			equip->GetSkeletonHierarchyRoot()->computeSkeletonHierarchyMatrix( pActionInfoGroup, param );

			g_pStack->Pop();
		}
	}
}

//------------------------------------------------------------------------------
/**
	set matrix of weapon for skinned character model by frame move.
*/
void 
EquipInstance::SetWorldTransform( const matrix44&  matTransform )
{
	// 如果skin有更新的话,就把matrix资料保存,没更新的时候拿来用
	if( m_pSkeletonHierarchy )
	{
		D3DXMATRIXA16* m = m_pSkeletonHierarchy->getSkeletonHierarchyMatrix();
		matrix44 matSkeleton;
		matSkeleton.set(float4(m->_11, m->_12, m->_13, m->_14),
						float4(m->_21, m->_22, m->_23, m->_24),
						float4(m->_31, m->_32, m->_33, m->_34),
						float4(m->_41, m->_42, m->_43, m->_44));
		this->transform = matrix44::multiply(matSkeleton, matTransform);
	}
	else
	{
		this->transform = matTransform;
	}
}

void 
EquipInstance::ForceAlpha( float falpha )
{
	
}

void 
EquipInstance::SetFakeReflect( bool bShow )
{
	
}

} // namespace Models
