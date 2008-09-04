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
//	"m_body",  // mptBody = 0, //身体model
//	"m_hair",  // mptHair,     //发型model
//	"m_head",  // mptHead,     //头部model
//	"m_handA", // mptHand,     //左右手掌
//	"m_handB", // mptArm,      //左右小臂
//	"m_pants", // mptPants,    //下半身model
//	"m_footA", // mptFoot,     //左右脚掌
//	"m_footB", // mptLeg,      //左右小腿
//	"m_cloak", //	mptCloak,    //披风
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
	更新需要改变的部件信息，没变化的不更新,如果角色多的时候大部份情况都不需要更新
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
	// 释放正在使用的
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
	骨骼不需要每帧都更新，只有在用的时候才更新
*/
void
BeingEntity::UpdateSkeleton()
{
	// 每帧只需要更新一次，因为骨骼层级不能放在model中，所以只能在这里设标志阻档
	// 骨骼层级更新非常费时间，所以尽量少的更新
	if (!this->skeletonUpdate)
		return;
	this->skeletonUpdate = false;

	m_sComputeAnimParam.m_iActionIndex1 = m_CurnAction.m_iIndex;
	m_sComputeAnimParam.m_iActionIndex2 = m_FadeoutAction.m_iIndex;
	m_sComputeAnimParam.m_fAnimTime1 = m_CurnAction.m_fAnimTime;
	m_sComputeAnimParam.m_fAnimTime2 = m_FadeoutAction.m_fAnimTime;
	m_sComputeAnimParam.m_fLerpValue = m_fFadeoutTimeRemain / m_fFadeoutTime;

	// 计算动作
#ifdef _CHECK_NOUSE_ANIMATION
	// 070425 cyhsieh 修改Mount Matrix的计算
	this->skeletonSerializer->computeSkeletonHierarchyMatrix(m_sComputeAnimParam, pRootTransform, &m_SkeletonHierarchyUnUsedMap );
#else
	// 修改Mount Matrix的计算
	this->skeletonSerializer->computeSkeletonHierarchyMatrix( m_sComputeAnimParam, NULL );
#endif
}

void
BeingEntity::UpdateAnimation(float fElapsedTime)
{
	// 动作时间会受 m_fPlaySpeed (动作播放速率) 所影响...
	// 在各种不同的动作下有不同的PlaySpeed, 
	// m_fPlaySpeed 会在必要的时候被设定成不一样的速率
	m_CurnAction.m_fAnimTime  += (fElapsedTime * 4800 * m_CurnAction.m_fPlaySpeed);
	if (m_CurnAction.m_fAnimTime > m_CurnAction.m_fAnimTimeLength)
	{
		// 锁定在最后一张Frame
		m_CurnAction.m_fAnimTime = 0;//m_CurnAction.m_fAnimTimeLength;
	}
	// 有动作正在淡出...
	if (m_fFadeoutTimeRemain > 0.0f)
	{
		// 递减淡出时间
		m_fFadeoutTimeRemain -= fElapsedTime;
		// 淡出完毕!!!
		if (m_fFadeoutTimeRemain <= 0.0f)
		{
			m_FadeoutAction.m_iIndex = -1;
			m_fFadeoutTimeRemain = 0.0f;
			m_fFadeoutTime       = 1.0f;
		}
		else 
		{
			// 淡出的动作不再加时间... 
			m_FadeoutAction.m_fAnimTime += (fElapsedTime * 4800 * m_FadeoutAction.m_fPlaySpeed);
			if (m_FadeoutAction.m_fAnimTime > m_FadeoutAction.m_fAnimTimeLength)
			{
				// 锁定在最后一张Frame
				// Bug Fix:淡出的动作应该是最后的动作
				// 因为m_fAnimTime会 fmod m_fAnimTimeLength，这样就会回到第0个frame的动作
				// !!!To do... 有没有比较好的方法?
				//m_FadeoutAction.m_fAnimTime = m_FadeoutAction.m_fAnimTimeLength;
				m_FadeoutAction.m_fAnimTime = m_FadeoutAction.m_fAnimTimeLength;
			} 
		} 
	} 

	// 装备特效
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
	创建实例，一种类型的部件只存在一个实例，换装的时候会删除原来的实例。
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

	// 替换部件显示
	this->bodyPart[managed->GetPartType()] = managed;
}

//------------------------------------------------------------------------------
/**
	part部件类型，有两种情况：HumanPartType\MountPartType
*/
void 
BeingEntity::SetPart(int part, const Ptr<ManagedBeing>& model)
{
	if (part < mptCOUNT && model.isvalid() && model->GetState() == Resource::Loaded)
		this->bodyPart[part] = model;
}

//------------------------------------------------------------------------------
/**
	设置、加载部件
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
	//this->bodyPart[partType] = partModel;	// 加载完后显示
}

//------------------------------------------------------------------------------
/**
	换装，三种情况：
	1、模型改变
	2、模型不变贴图改变
	3、模型不变贴图不变

	注意：换装只有等加载完才替换原来的！
*/
void
BeingEntity::ChangePart(int partType, const String& modelName, int textureId)
{
	if (partType >= mptCOUNT || partType < 0)
		return;

	Ptr<ManagedBeing> partModel = ResourceManager::Instance()->LookupManagedResource(modelName).downcast<ManagedBeing>();
	if (!partModel.isvalid())
	{
		// 模型、贴图都变，相当于创建新的
		ResourceManager::Instance()->CreateManagedResource(Being::RTTI, modelName).downcast<ManagedBeing>();
		partModel->SetTextureId(textureId);
		partModel->SetPartType(partType);
		this->changeList.Append(partModel);
	}
	else
	{
		// 模型改变
		if (partModel != this->bodyPart[partType])
		{
			this->bodyPart[partType] = partModel;
		}

		// 贴图改变
		if (partModel->GetTextureId() != textureId)
		{
			partModel->SetTextureId(textureId);
			partModel->LoadTexture();
		}
	}
	
}

//------------------------------------------------------------------------------
/**
	所有部件的实例都需要可见，所以都要加入到可见列表
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
	创建骨骼信息
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

	// 实体影子
	//if( m_bPrincipal )    // 玩家才有实体影子
	//{
	//	m_pModel->createSkinnedCharacterShadowData(s_pD3DDevice);
	//}

	// 这是玩家的模式, 需要更新装备栏位???
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
//		// 有SkinWeight
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
//		// 没有SkinWeight
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
	if (iActionIndex < 0) return -1; // 不合法的动作索引, 不进行动作变更
	if (this->skeletonSerializer == NULL)    return -1; // 没有this->skeletonSerializer, 无法进行动作变更
	if (this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount() == 0)  return -1; // 没有动作表, 无法进行动作变更

	// 流程简化
	this->skeletonSerializer->prepareAnimationKeys( iActionIndex );

	// 淡出前一个动作.
	if (fFadeoutTime > 0.0f)
	{
		// 流程简化
		this->skeletonSerializer->prepareAnimationKeys( m_CurnAction.m_iIndex );

		m_FadeoutAction.m_iIndex          = m_CurnAction.m_iIndex;
		m_FadeoutAction.m_fAnimTime       = m_CurnAction.m_fAnimTime;   	// 从头开始播放
		m_FadeoutAction.m_fAnimTimeLength = m_CurnAction.m_fAnimTimeLength; // 记录动作时间总长
		m_FadeoutAction.m_fPlaySpeed      = m_CurnAction.m_fPlaySpeed;
		m_fFadeoutTimeRemain              = fFadeoutTime;
		m_fFadeoutTime                    = fFadeoutTime;
	} 
	else 
	{ // 没有动作需要淡出
		m_fFadeoutTime = 1.0f;
		m_fFadeoutTimeRemain = 0.0f;
		m_FadeoutAction.m_iIndex = -1;
	}

	// 避免分母为 0, 直接令 m_fFadeoutTime 等于 1
	if (m_fFadeoutTime < 0.0f) m_fFadeoutTime = 1.0f;

	cAnimationActionInfo* pAnimationActionInfo = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo( iActionIndex );

	if (pAnimationActionInfo == NULL) return -1;
	// 新动作的起始时间 
	DWORD dwStartTime = pAnimationActionInfo->getAnimationActionStartTime();
	// 新动作的结束时间 
	DWORD dwEndTime   = pAnimationActionInfo->getAnimationActionEndTime();
	// 动作时间总长(200 unit per frame)
	m_CurnAction.m_iIndex = iActionIndex; // 记录新的动作索引
	// 每个Frame 200个单位, 24个Frame共4800个单位, 从0开始计算, 最后一张应为 4600, 所以减去 200
	m_CurnAction.m_fAnimTimeLength = float(dwEndTime - dwStartTime) - ((animationActionOperation & aoIgnoreCountBackwards1ndKeyframe) ? 200 : 0); // 记录动作时间总长
	// 如果有特别指定, 就锁在最后一张, 不然就从第一张开始播放
	m_CurnAction.m_fAnimTime = (animationActionOperation & aoCountBackwards2ndKeyframe) ? m_CurnAction.m_fAnimTimeLength : 0; // 播放起始位置

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
	if (iActionIndex < 0) return -1; // 不合法的动作索引, 不进行动作变更
	if (this->skeletonSerializer == NULL)    return -1; // 没有FrameWork, 无法进行动作变更
	if (this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount() == 0)  return -1; // 没有动作表, 无法进行动作变更

	// 060502 cyhsieh Act Loading 流程简化
	this->skeletonSerializer->prepareAnimationKeys( iActionIndex );

	cAnimationActionInfo* pAnimationActionInfo = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo( iActionIndex );
	if (pAnimationActionInfo == NULL) return -1;

	// 新动作的起始时间
	DWORD dwStartTime = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo(iActionIndex)->getAnimationActionStartTime();
	// 新动作的结束时间 
	DWORD dwEndTime   = this->skeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo(iActionIndex)->getAnimationActionEndTime();

	m_FadeoutAction.m_iIndex          = iActionIndex;
	m_FadeoutAction.m_fAnimTime       = 0;         // 从头开始播放
	m_FadeoutAction.m_fAnimTimeLength = float(dwEndTime - dwStartTime); // 记录动作时间总长
	m_FadeoutAction.m_fPlaySpeed      = fPlaySpeed;
	m_fFadeoutTimeRemain              = fFadeoutTime;
	m_fFadeoutTime                    = fFadeoutTime;

	return 0;
}

//------------------------------------------------------------------------------
/**
	设置装备
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
