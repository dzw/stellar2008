#pragma once
#ifndef KOK_EQPIPINSTANCE_H
#define KOK_EQPIPINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::EquipInstance
    
    (C) 2008 cTuo
*/
#include "models/modelinstance.h"
#include "kok/effect/c3dsMaxParticle.h"
#include "kok/model/animation/cSkeletonHierarchy.h"

//------------------------------------------------------------------------------
namespace KOK
{
class EquipInstance : public Models::ModelInstance
{
    DeclareClass(EquipInstance);
public:
    /// constructor
    EquipInstance();
    /// destructor
    virtual ~EquipInstance();

    /// perform per-frame update (after setting transform, visibility, time, etc)
    virtual void Update();

	// set cSkeletonHierarchy of equip or weapon for skinned character model.
	void setEquipSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy );
	// getcSkeletonHierarchy of equip or weapon for skinned character model.
	cSkeletonHierarchy* getEquipSkeletonHierarchy();
	// get visible of equip or weapon for skinned character model.
	//bool getEquipVisible();

	void SetWorldTransform( const Math::matrix44&  matTransform );

	int GetTextureID( void ) { return m_iTextureID; }
	void SetEquip( int iTextureID, bool bWeaponRibbonLerp ); // 设定模型Model
	//void SetFade( float fValue );                                     // 设定淡出淡入
	//void SetShadowData( sShadowData* pShadowData );                   // 设定阴影资料

	void TickAnimTime( float fElapsedTime );                          // 更新Animation时间
	void ComputeAnimation( void );                                    // 计算Animation

	//void AddEquipModelEffect( char* pName, BYTE byStatusType );       // 新增一个装备专用模型特效

	//void RemoveStatusModelEffect( void );                             // 移除状态装备特效

	void SetShowWeaponRibbon( bool bShow );							 // 设定是否显示刀光

	void ForceAlpha( float falpha );                                  // 强制设定Render的Alpha值

	void SetFakeReflect( bool bShow );                                // 设定是否开启假反光

	void SetForceHide( bool bHide );							         // 设定是否强制隐藏

	void SetActionIndex( int iActionIndex );                          // 设定动作
	BYTE GetPartID( void );						                     // 取得部位ID
	BYTE GetLinkID( void );						                     // 取得连结点ID

	// 特效
	//int GetEquipModelEffectCount( void );                             // 取得装备特效数量
	//char* GetEquipModelEffectName( int iIndex );                      // 取得装备特效名称

protected:
	virtual void OnAttachToModel(const Ptr<Models::Model>& model);
	virtual void NotifyVisible(IndexT frameIndex);

	//struct sStatusEffect
	//{
	//	BYTE                m_byType;                                   // 状态类型
	//	cModelEffect*       m_pModelEffect;                             // 状态特效
	//	sStatusEffect()
	//		:m_byType(0), m_pModelEffect(NULL)
	//	{
	//	}
	//};

	// cSkeletonHierarchy of equip or weapon for skinned character model.
	cSkeletonHierarchy*   m_pSkeletonHierarchy;

	//  钓竿
	int                   m_iActionIndex;                             // 动作索引
	BYTE                  m_byPartID;                                 // 部位ID
	BYTE                  m_byLinkID;                                 // 连结点ID

	// 坐下不显示武器
	bool                  m_bModelExist;                              // 是否设定了Model
	bool                  m_bForseHide;                               // 强制不显示

	// model effect
	//BTList*               m_pModelEffectList;                         // 装备专用模型特效List

	// 状态装备特效
	//sStatusEffect*        m_pStatusEffect;                            // 状态装备特效

	// 武器刀光
	cParticleEffectBase*  m_pWeaponRibbon;                            // 刀光用粒子
	bool                  m_bShowWeaponRibbon;                        // 是否显示刀光

	int                   m_iTextureID;

	// 装备特效
	DWORD                 m_dwParticleEffectSize;                     // 粒子数量
	cParticleEffectBase** m_ppParticleEffects;                        // 粒子
	float                 m_fCurrentAnimTime;                         // 目前动画时间
	float                 m_fAnimElapsedTime;                         // 动画经过时间
};

inline void 
EquipInstance::setEquipSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy )
{
	m_pSkeletonHierarchy = pSkeletonHierarchy;
}

inline void 
EquipInstance::SetShowWeaponRibbon( bool bShow )
{
	m_bShowWeaponRibbon = bShow; 
}

inline void 
EquipInstance::SetForceHide( bool bHide ) 
{
	m_bForseHide = bHide; 
}         

inline void 
EquipInstance::SetActionIndex( int iActionIndex )
{
	m_iActionIndex = iActionIndex;
}

inline BYTE 
EquipInstance::GetPartID()
{
	return m_byPartID; 
}

inline BYTE 
EquipInstance::GetLinkID()
{ 
	return m_byLinkID; 
}                     

} // namespace Models
//------------------------------------------------------------------------------
#endif
