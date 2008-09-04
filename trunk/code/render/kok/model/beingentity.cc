//------------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/beingentity.h"
//#include "kok/model/managedbeing.h"
#include "resources/resourcemanager.h"
#include "models/visresolver.h"
#include "models/modelinstance.h"
#include "kok/model/animation/cSkeletonSerializerFactory.h"
#include "math/quaternion.h"
#include "math/vector.h"

namespace KOK
{
ImplementClass(KOK::BeingEntity, 'BEET', Graphics::ModelEntity);

using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Util;

//char* BeingEntity::s_aszMPName[mptCOUNT] =
//{
//	"m_body",  // mptBody = 0, //����model
//	"m_hair",  // mptHair,     //����model
//	"m_head",  // mptHead,     //ͷ��model
//	"m_handA", // mptHand,     //��������
//	"m_handB", // mptArm,      //����С��
//	"m_pants", // mptPants,    //�°���model
//	"m_footA", // mptFoot,     //���ҽ���
//	"m_footB", // mptLeg,      //����С��
//	"m_cloak", //	mptCloak,    //����
//};

//------------------------------------------------------------------------------
/**
*/
BeingEntity::BeingEntity():
	skeletonSerializer(0),
	fakeReflectTexture(0)
{
	this->SetType(ModelType);

	for (SizeT i = 0; i < mptCOUNT; i++)
	{
		this->bodyPart[i] = 0;
		this->bodyInstance[i] = 0;
	}
	this->changeList.Clear();

	for (SizeT i = 0; i < eptCOUNT; i++)
	{
		this->equipPart[i] = 0;
		this->equipInstance[i] = 0;
	}
	this->changeEquipList.Clear();
	
}

//------------------------------------------------------------------------------
/**
*/
BeingEntity::~BeingEntity()
{
}

//------------------------------------------------------------------------------
/**
*/
void
BeingEntity::OnActivate()
{
    n_assert(!this->IsActive());
    GraphicsEntity::OnActivate();

    // note: we will remain invalid until at least our model has loaded
    this->SetValid(true);
    //this->managedModel = ResourceManager::Instance()->CreateManagedResource(Thing::RTTI, this->resId).downcast<ManagedThing>();

	String fakeTex = "mtext:FakeReflect.dds";
	this->fakeReflectTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, fakeTex).downcast<ManagedTexture>();
}

//------------------------------------------------------------------------------
/**
*/
void
BeingEntity::OnDeactivate()
{
    n_assert(this->IsActive());

	for (SizeT i = 0; i < mptCOUNT; i++)
	{
		if (this->bodyInstance[i].isvalid())
		{
			this->bodyInstance[i]->Discard();
			this->bodyInstance[i] = 0;
		}
	}

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
BeingEntity::OnUpdate()
{
	for (SizeT i = 0; i < mptCOUNT; i++)
	{
		if (this->bodyInstance[i].isvalid())
		{
			this->bodyInstance[i]->SetTime(this->GetTime());
			this->bodyInstance[i]->Update();
		}
	}

	UpdatePart();
	UpdateEquipPart();

	UpdateAnimation(this->GetTime() / 1000.0f);
	this->skeletonUpdate = true;
	//UpdateSkeleton();

	UpdateTransform();

	//ModelEntity::OnUpdate();
	GraphicsEntity::OnUpdate();
}

//------------------------------------------------------------------------------
/**
	������Ҫ�ı�Ĳ�����Ϣ��û�仯�Ĳ�����,�����ɫ���ʱ��󲿷����������Ҫ����
*/
void
BeingEntity::UpdatePart()
{
	for (SizeT i = 0; i < this->changeList.Size(); )
	{
		if (this->changeList[i].isvalid() && this->changeList[i]->GetState() == Resource::Loaded)
		{
			this->changeList[i]->Update();
			ValidateBodyInstance(this->changeList[i]);
			
			this->changeList.EraseIndex(i);
		}
		else
			i++;
	}
}

void
BeingEntity::UpdateEquipPart()
{
	for (SizeT i = 0; i < this->changeEquipList.Size(); )
	{
		if (this->changeEquipList[i].isvalid() && this->changeEquipList[i]->GetState() == Resource::Loaded)
		{
			ValidateEquipInstance(this->changeEquipList[i]);
			this->changeEquipList.EraseIndex(i);
		}
		else
			i++;
	}
}

void
BeingEntity::ValidateEquipInstance(const Ptr<ManagedEquip>& managed)
{
	EquipLinkerType partType = managed->GetPartType();
	// �ͷ�����ʹ�õ�
	if (this->equipPart[partType].isvalid())
	{
		ResourceManager::Instance()->DiscardManagedResource(this->equipPart[partType].upcast<ManagedResource>());
		this->equipPart[partType] = 0;
	}
	if (this->equipInstance[partType].isvalid())
	{
		this->equipInstance[partType]->Discard();
		this->equipInstance[partType] = 0;
	}

	this->equipPart[partType] = managed;

	const Ptr<Equip> model = managed->GetEquip();
	n_assert(model->IsLoaded());

	bbox b = this->GetLocalBoundingBox();
	b.extend(model->GetBoundingBox());
	this->SetLocalBoundingBox(b);

	this->equipInstance[partType] = model->CreateInstance();
	this->equipInstance[partType]->SetTransform(this->GetTransform());
	this->equipInstance[partType]->SetModelEntity(this);

	this->equipInstance[partType]->SetEquip();
	if (this->skeletonSerializer != NULL)
	{
		cSkeletonHierarchy* skeleton = this->skeletonSerializer->getWeaponSkeletonHierarchy((unsigned int)partType);
		if (skeleton != NULL)
		{
			this->equipInstance[partType]->setEquipSkeletonHierarchy(skeleton);

//#ifdef _CHECK_NOUSE_ANIMATION
//			SkeletonHierarchyUsabilityMap::iterator iter = m_SkeletonHierarchyUnUsedMap.find( pSkeletonHierarchy );
//			if( iter != m_SkeletonHierarchyUnUsedMap.end() )
//			{
//				m_SkeletonHierarchyUnUsedMap.erase( iter );
//			}
//#endif
		}
	}
}

//------------------------------------------------------------------------------
/**
	��������Ҫÿ֡�����£�ֻ�����õ�ʱ��Ÿ���
*/
void
BeingEntity::UpdateSkeleton()
{
	// ÿֻ֡��Ҫ����һ�Σ���Ϊ�����㼶���ܷ���model�У�����ֻ�����������־�赵
	// �����㼶���·ǳ���ʱ�䣬���Ծ����ٵĸ���
	if (!this->skeletonUpdate)
		return;
	this->skeletonUpdate = false;

	m_sComputeAnimParam.m_iActionIndex1 = m_CurnAction.m_iIndex;
	m_sComputeAnimParam.m_iActionIndex2 = m_FadeoutAction.m_iIndex;
	m_sComputeAnimParam.m_fAnimTime1 = m_CurnAction.m_fAnimTime;
	m_sComputeAnimParam.m_fAnimTime2 = m_FadeoutAction.m_fAnimTime;
	m_sComputeAnimParam.m_fLerpValue = m_fFadeoutTimeRemain / m_fFadeoutTime;

	// ���㶯��
#ifdef _CHECK_NOUSE_ANIMATION
	// 070425 cyhsieh �޸�Mount Matrix�ļ���
	this->skeletonSerializer->computeSkeletonHierarchyMatrix(m_sComputeAnimParam, pRootTransform, &m_SkeletonHierarchyUnUsedMap );
#else
	// �޸�Mount Matrix�ļ���
	this->skeletonSerializer->computeSkeletonHierarchyMatrix( m_sComputeAnimParam, NULL );
#endif
}

void
BeingEntity::UpdateAnimation(float fElapsedTime)
{
	// ����ʱ����� m_fPlaySpeed (������������) ��Ӱ��...
	// �ڸ��ֲ�ͬ�Ķ������в�ͬ��PlaySpeed, 
	// m_fPlaySpeed ���ڱ�Ҫ��ʱ���趨�ɲ�һ��������
	m_CurnAction.m_fAnimTime  += (fElapsedTime * 4800 * m_CurnAction.m_fPlaySpeed);
	if (m_CurnAction.m_fAnimTime > m_CurnAction.m_fAnimTimeLength)
	{
		// ���������һ��Frame
		m_CurnAction.m_fAnimTime = 0;//m_CurnAction.m_fAnimTimeLength;
	}
	// �ж������ڵ���...
	if (m_fFadeoutTimeRemain > 0.0f)
	{
		// �ݼ�����ʱ��
		m_fFadeoutTimeRemain -= fElapsedTime;
		// �������!!!
		if (m_fFadeoutTimeRemain <= 0.0f)
		{
			m_FadeoutAction.m_iIndex = -1;
			m_fFadeoutTimeRemain = 0.0f;
			m_fFadeoutTime       = 1.0f;
		}
		else 
		{
			// �����Ķ������ټ�ʱ��... 
			m_FadeoutAction.m_fAnimTime += (fElapsedTime * 4800 * m_FadeoutAction.m_fPlaySpeed);
			if (m_FadeoutAction.m_fAnimTime > m_FadeoutAction.m_fAnimTimeLength)
			{
				// ���������һ��Frame
				// Bug Fix:�����Ķ���Ӧ�������Ķ���
				// ��Ϊm_fAnimTime�� fmod m_fAnimTimeLength�������ͻ�ص���0��frame�Ķ���
				// !!!To do... ��û�бȽϺõķ���?
				//m_FadeoutAction.m_fAnimTime = m_FadeoutAction.m_fAnimTimeLength;
				m_FadeoutAction.m_fAnimTime = m_FadeoutAction.m_fAnimTimeLength;
			} 
		} 
	} 

	// װ����Ч
	/*if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && pEquipPart->getEquipVisible() )
			{
				pEquipPart->TickAnimTime( fElapsedTime );
			}
		}
	}*/
}

//------------------------------------------------------------------------------
/**
	����ʵ����һ�����͵Ĳ���ֻ����һ��ʵ������װ��ʱ���ɾ��ԭ����ʵ����
*/
void
BeingEntity::ValidateBodyInstance(const Ptr<ManagedBeing>& managed)
{
	int partType = managed->GetPartType();

	if (this->bodyPart[partType].isvalid())
	{
		ResourceManager::Instance()->DiscardManagedResource(this->bodyPart[partType].upcast<ManagedResource>());
		this->bodyPart[partType] = 0;
	}
	if (this->bodyInstance[partType].isvalid())
	{
		this->bodyInstance[partType]->Discard();
		this->bodyInstance[partType] = 0;
	}

	const Ptr<Being> model = managed->GetBeing();
	n_assert(model->IsLoaded());

	bbox b = this->GetLocalBoundingBox();
	b.extend(model->GetBoundingBox());
	this->SetLocalBoundingBox(b);

	this->bodyInstance[partType] = model->CreateInstance();
	this->bodyInstance[partType]->SetTransform(this->GetTransform());
	this->bodyInstance[partType]->SetModelEntity(this);

	// �滻������ʾ
	this->bodyPart[managed->GetPartType()] = managed;
}

//------------------------------------------------------------------------------
/**
	part�������ͣ������������HumanPartType\MountPartType
*/
void 
BeingEntity::SetPart(int part, const Ptr<ManagedBeing>& model)
{
	if (part < mptCOUNT && model.isvalid() && model->GetState() == Resource::Loaded)
		this->bodyPart[part] = model;
}

//------------------------------------------------------------------------------
/**
	���á����ز���
*/
void 
BeingEntity::SetPart(int partType, /*
						  SkinType skinType, int occupationType, 
						  int modelId,*/const String& modelName, int textureId)
{
	if (partType >= mptCOUNT || partType < 0)
		return;

	//String resId;
	//resId.Format("%s%1d%1d%03d", s_aszMPName[partType], skinType, occupationType, modelId);
	Ptr<ManagedBeing> partModel = ResourceManager::Instance()->CreateManagedResource(Being::RTTI, modelName).downcast<ManagedBeing>();
	partModel->SetTextureId(textureId);
	partModel->SetPartType(partType);
	this->changeList.Append(partModel);
	//this->bodyPart[partType] = partModel;	// ���������ʾ
}

//------------------------------------------------------------------------------
/**
	��װ�����������
	1��ģ�͸ı�
	2��ģ�Ͳ�����ͼ�ı�
	3��ģ�Ͳ�����ͼ����

	ע�⣺��װֻ�еȼ�������滻ԭ���ģ�
*/
void
BeingEntity::ChangePart(int partType, const String& modelName, int textureId)
{
	if (partType >= mptCOUNT || partType < 0)
		return;

	Ptr<ManagedBeing> partModel = ResourceManager::Instance()->LookupManagedResource(modelName).downcast<ManagedBeing>();
	if (!partModel.isvalid())
	{
		// ģ�͡���ͼ���䣬�൱�ڴ����µ�
		ResourceManager::Instance()->CreateManagedResource(Being::RTTI, modelName).downcast<ManagedBeing>();
		partModel->SetTextureId(textureId);
		partModel->SetPartType(partType);
		this->changeList.Append(partModel);
	}
	else
	{
		// ģ�͸ı�
		if (partModel != this->bodyPart[partType])
		{
			this->bodyPart[partType] = partModel;
		}

		// ��ͼ�ı�
		if (partModel->GetTextureId() != textureId)
		{
			partModel->SetTextureId(textureId);
			partModel->LoadTexture();
		}
	}
	
}

//------------------------------------------------------------------------------
/**
	���в�����ʵ������Ҫ�ɼ������Զ�Ҫ���뵽�ɼ��б�
*/
void 
BeingEntity::AttachVisibleInstance()
{
	VisResolver* visResolver = VisResolver::Instance();

	for (SizeT i = 0; i < mptCOUNT; i++)
	{
		if (this->bodyInstance[i].isvalid())
			visResolver->AttachVisibleModelInstance(this->bodyInstance[i]);
	}
}

//------------------------------------------------------------------------------
/**
	����������Ϣ
*/
void
BeingEntity::CreateSkeleton(BeingType1& beType, const String& skeletonName, const String& path)
{
	if (this->skeletonSerializer == NULL)
	{
		this->skeletonSerializer = cSkeletonSerializerFactory::Instance()->createSkeletonSerializer(beType, (char*)skeletonName.AsCharPtr(), path.AsCharPtr());
		if(this->skeletonSerializer  == 0)
			n_assert(0);
	}

	/*for (SizeT i = 0; i < mptCOUNT; i++)
	{
		if (bodyPart[i].isvalid())
		{
			const Ptr<Being>& being = bodyPart[i]->GetBeing();
			for (SizeT n = 0; n < being->GetNodes)
			{
			}
		}
	}*/

	// ʵ��Ӱ��
	//if( m_bPrincipal )    // ��Ҳ���ʵ��Ӱ��
	//{
	//	m_pModel->createSkinnedCharacterShadowData(s_pD3DDevice);
	//}

	// ������ҵ�ģʽ, ��Ҫ����װ����λ???
	//if (BeingType == BEING_TYPE_PLAY) m_pModel->updateSkinnedCharacterEquips();
}

 
//------------------------------------------------------------------------------
/**
*/
//#ifdef _CHECK_NOUSE_ANIMATION
//void 
//BeingEntity::getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, const Ptr<BeingNode>& node )
//{
//	if( this->skeletonSerializer == NULL || !node.isvalid() )
//		return;
//
//	cSkinWeights* pMeshSkinWeights = node->GetSkinWeights( k );
//	DWORD dwSkinWeightsSize = node->GetSkinWeightNum();
//	if( dwSkinWeightsSize > 0 )
//	{
//		// ��SkinWeight
//		for( DWORD k = 0; k < dwSkinWeightsSize; k++ )
//		{
//			cSkeletonHierarchy* pSkeletonHierarchy = this->skeletonSerializer->findSkeletonHierarchy( pMeshSkinWeights[k].getSkinWeightsSkeletonName() );
//			if( pSkeletonHierarchy )
//			{
//				SkeletonHierarchyUsabilityMap::iterator iter = SkeletonHierarchyUsedMap.find( pSkeletonHierarchy );
//				if( iter == SkeletonHierarchyUsedMap.end() )
//				{
//					SkeletonHierarchyUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
//				}
//			}
//		}
//	}
//	else
//	{
//		// û��SkinWeight
//		//cSkeletonHierarchy* pSkeletonHierarchy = m_epFrameWork->FindFrameFromMap( pSubMeshSerial->getSubMeshName() );
//		cSkeletonHierarchy* pSkeletonHierarchy = this->skeletonSerializer->findSkeletonHierarchy( node->GetName().AsCharPtr() );
//		if( pSkeletonHierarchy )
//		{
//			SkeletonHierarchyUsabilityMap::iterator iter = SkeletonHierarchyUsedMap.find( pSkeletonHierarchy );
//			if( iter == SkeletonHierarchyUsedMap.end() )
//			{
//				SkeletonHierarchyUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
//			}
//		}
//	}
//}
//#endif
//
////------------------------------------------------------------------------------
///**
//*/
//#ifdef _CHECK_NOUSE_ANIMATION
//void 
//BeingEntity::getUnusedSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy, SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, bool& bParentUse )
//{
//	bool bSelfUse = false;
//	bool bNextUse = false;
//
//	if( pSkeletonHierarchy->getChild() )
//	{
//		getUnusedSkeletonHierarchy( pSkeletonHierarchy->getChild(), SkeletonHierarchyUsedMap, bSelfUse );
//	}
//
//	if( pSkeletonHierarchy->getSibling() )
//	{
//		getUnusedSkeletonHierarchy( pSkeletonHierarchy->getSibling(), SkeletonHierarchyUsedMap, bNextUse );
//	}
//
//	if( bSelfUse )
//	{
//		bParentUse = true;
//	}
//	else
//	{
//		if( bNextUse )
//		{
//			bParentUse = true;
//		}
//
//		SkeletonHierarchyUsabilityMap::iterator iter = SkeletonHierarchyUsedMap.find( pSkeletonHierarchy );
//		if( iter != SkeletonHierarchyUsedMap.end() )
//		{
//			bSelfUse = true;
//			bParentUse = true;
//		}
//		else
//		{
//			if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "k_weapon" ) != NULL )
//			{
//				bParentUse = true;
//				m_SkeletonHierarchyUnUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
//			}
//			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "k_magic" ) != NULL )
//			{
//				bParentUse = true;
//			}
//			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "Link_Ride01" ) != NULL )
//			{
//				bParentUse = true;
//			}
//			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "Link_Ride02" ) != NULL )
//			{
//				bParentUse = true;
//			}
//			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "Link_Ride03" ) != NULL )
//			{
//				bParentUse = true;
//			}
//			else
//			{
//				m_SkeletonHierarchyUnUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
//			}
//		}
//	}
//}
//#endif

//-----------------------------------------------------------------------------
// Name: createAnimationAction()
// Desc: create a new animation action by index with fading out prevoius animation action 
//       and set new aniamtion action operation , play speed, fade out time. 
//-----------------------------------------------------------------------------
int 
BeingEntity::CreateAnimationAction( int iActionIndex, float fPlaySpeed, 
								   AnimationActionOperation animationActionOperation, float fFadeoutTime )
{
	if (iActionIndex < 0) return -1; // ���Ϸ��Ķ�������, �����ж������
	if (this->skeletonSerializer == NULL)    return -1; // û��this->skeletonSerializer, �޷����ж������
	if (this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount() == 0)  return -1; // û�ж�����, �޷����ж������

	// ���̼�
	this->skeletonSerializer->prepareAnimationKeys( iActionIndex );

	// ����ǰһ������.
	if (fFadeoutTime > 0.0f)
	{
		// ���̼�
		this->skeletonSerializer->prepareAnimationKeys( m_CurnAction.m_iIndex );

		m_FadeoutAction.m_iIndex          = m_CurnAction.m_iIndex;
		m_FadeoutAction.m_fAnimTime       = m_CurnAction.m_fAnimTime;   	// ��ͷ��ʼ����
		m_FadeoutAction.m_fAnimTimeLength = m_CurnAction.m_fAnimTimeLength; // ��¼����ʱ���ܳ�
		m_FadeoutAction.m_fPlaySpeed      = m_CurnAction.m_fPlaySpeed;
		m_fFadeoutTimeRemain              = fFadeoutTime;
		m_fFadeoutTime                    = fFadeoutTime;
	} 
	else 
	{ // û�ж�����Ҫ����
		m_fFadeoutTime = 1.0f;
		m_fFadeoutTimeRemain = 0.0f;
		m_FadeoutAction.m_iIndex = -1;
	}

	// �����ĸΪ 0, ֱ���� m_fFadeoutTime ���� 1
	if (m_fFadeoutTime < 0.0f) m_fFadeoutTime = 1.0f;

	cAnimationActionInfo* pAnimationActionInfo = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo( iActionIndex );

	if (pAnimationActionInfo == NULL) return -1;
	// �¶�������ʼʱ�� 
	DWORD dwStartTime = pAnimationActionInfo->getAnimationActionStartTime();
	// �¶����Ľ���ʱ�� 
	DWORD dwEndTime   = pAnimationActionInfo->getAnimationActionEndTime();
	// ����ʱ���ܳ�(200 unit per frame)
	m_CurnAction.m_iIndex = iActionIndex; // ��¼�µĶ�������
	// ÿ��Frame 200����λ, 24��Frame��4800����λ, ��0��ʼ����, ���һ��ӦΪ 4600, ���Լ�ȥ 200
	m_CurnAction.m_fAnimTimeLength = float(dwEndTime - dwStartTime) - ((animationActionOperation & aoIgnoreCountBackwards1ndKeyframe) ? 200 : 0); // ��¼����ʱ���ܳ�
	// ������ر�ָ��, ���������һ��, ��Ȼ�ʹӵ�һ�ſ�ʼ����
	m_CurnAction.m_fAnimTime = (animationActionOperation & aoCountBackwards2ndKeyframe) ? m_CurnAction.m_fAnimTimeLength : 0; // ������ʼλ��

	m_CurnAction.m_fPlaySpeed = fPlaySpeed;

	return iActionIndex;
}


//------------------------------------------------------------------------------
/**
	create a new animation action by index into blending current animation action and set it's playing speed , fading out time. 
*/
int 
BeingEntity::CreateAnimationActionIntoBlending( int iActionIndex, float fPlaySpeed, float fFadeoutTime )
{
	if (iActionIndex < 0) return -1; // ���Ϸ��Ķ�������, �����ж������
	if (this->skeletonSerializer == NULL)    return -1; // û��FrameWork, �޷����ж������
	if (this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount() == 0)  return -1; // û�ж�����, �޷����ж������

	// 060502 cyhsieh Act Loading ���̼�
	this->skeletonSerializer->prepareAnimationKeys( iActionIndex );

	cAnimationActionInfo* pAnimationActionInfo = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo( iActionIndex );
	if (pAnimationActionInfo == NULL) return -1;

	// �¶�������ʼʱ��
	DWORD dwStartTime = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo(iActionIndex)->getAnimationActionStartTime();
	// �¶����Ľ���ʱ�� 
	DWORD dwEndTime   = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo(iActionIndex)->getAnimationActionEndTime();

	m_FadeoutAction.m_iIndex          = iActionIndex;
	m_FadeoutAction.m_fAnimTime       = 0;         // ��ͷ��ʼ����
	m_FadeoutAction.m_fAnimTimeLength = float(dwEndTime - dwStartTime); // ��¼����ʱ���ܳ�
	m_FadeoutAction.m_fPlaySpeed      = fPlaySpeed;
	m_fFadeoutTimeRemain              = fFadeoutTime;
	m_fFadeoutTime                    = fFadeoutTime;

	return 0;
}

//------------------------------------------------------------------------------
/**
	����װ��
*/
void 
BeingEntity::SetEquipPart(const String& modelName,
						  EquipLinkerType equipPartType, 
						  int texId)
{
	if (name.Length() <= 0)
		return;

	if (equipPartType >= eptCOUNT)
		return;

	EquipLinkerType linkId;
	if (equipPartType == eptFishingRod)
		linkId = eptWeaponR;
	else
		linkId = equipPartType;

	
	Ptr<ManagedEquip> partModel = ResourceManager::Instance()->CreateManagedResource(Equip::RTTI, modelName).downcast<ManagedEquip>();
	partModel->SetPartType(linkId);
	partModel->SetTextureId(texId);
	this->changeEquipList.Append(partModel);
}

void 
BeingEntity::RemoveEquipPart(EquipLinkerType equipPartType)
{
	if (equipPartType >= eptCOUNT)
		return;

	if (this->equipPart[equipPartType].isvalid())
	{
		ResourceManager::Instance()->DiscardManagedResource(this->equipPart[equipPartType].upcast<ManagedResource>());
		this->equipPart[equipPartType] = 0;
	}
	if (this->equipInstance[equipPartType].isvalid())
	{
		this->equipInstance[equipPartType]->Discard();
		this->equipInstance[equipPartType] = 0;
	}
}

void 
BeingEntity::SetEquipActionIndex(int actionIndex)
{
	
}


void 
BeingEntity::SetDirection(float dir)
{
	Math::quaternion rotate;
	rotate.set(0.0f, 1.0f, 0.0f, (dir + 360) % 360) * (N_PI / 180.0f));
	this->tform.setrotate(rotate);
}

void 
BeingEntity::SetPosition(float posX, float posY, float posZ)
{
	this->tform.setposition(Math::vector(posX, posY, posZ));
}

void 
BeingEntity::SetScale(float scaleX, float scaleY, float scaleZ)
{
	this->tform.setscale(Math::vector(scaleX, scaleY, scaleZ));
}

void
BeingEntity::UpdateTransform()
{
	if (this->tform.isdirty())
	{
		this->transform = this->tform.getmatrix();
		for (SizeT i = 0; i < mptCOUNT; i++)
		{
			if (this->bodyPart[i].isvalid())
				this->bodyPart[i]->SetTransform(this->transform);
		}

		for (SizeT i = 0; i < eptCOUNT; i++)
		{
			if (this->equipPart[i].isvalid())
			{
				this->equipPart[i]->SetWorldTransform(this->transform);
			}
		}
	}
}

} // namespace Graphics
