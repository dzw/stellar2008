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
	void SetEquip( int iTextureID, bool bWeaponRibbonLerp ); // �趨ģ��Model
	//void SetFade( float fValue );                                     // �趨��������
	//void SetShadowData( sShadowData* pShadowData );                   // �趨��Ӱ����

	void TickAnimTime( float fElapsedTime );                          // ����Animationʱ��
	void ComputeAnimation( void );                                    // ����Animation

	//void AddEquipModelEffect( char* pName, BYTE byStatusType );       // ����һ��װ��ר��ģ����Ч

	//void RemoveStatusModelEffect( void );                             // �Ƴ�״̬װ����Ч

	void SetShowWeaponRibbon( bool bShow );							 // �趨�Ƿ���ʾ����

	void ForceAlpha( float falpha );                                  // ǿ���趨Render��Alphaֵ

	void SetFakeReflect( bool bShow );                                // �趨�Ƿ����ٷ���

	void SetForceHide( bool bHide );							         // �趨�Ƿ�ǿ������

	void SetActionIndex( int iActionIndex );                          // �趨����
	BYTE GetPartID( void );						                     // ȡ�ò�λID
	BYTE GetLinkID( void );						                     // ȡ�������ID

	// ��Ч
	//int GetEquipModelEffectCount( void );                             // ȡ��װ����Ч����
	//char* GetEquipModelEffectName( int iIndex );                      // ȡ��װ����Ч����

protected:
	virtual void OnAttachToModel(const Ptr<Models::Model>& model);
	virtual void NotifyVisible(IndexT frameIndex);

	//struct sStatusEffect
	//{
	//	BYTE                m_byType;                                   // ״̬����
	//	cModelEffect*       m_pModelEffect;                             // ״̬��Ч
	//	sStatusEffect()
	//		:m_byType(0), m_pModelEffect(NULL)
	//	{
	//	}
	//};

	// cSkeletonHierarchy of equip or weapon for skinned character model.
	cSkeletonHierarchy*   m_pSkeletonHierarchy;

	//  ����
	int                   m_iActionIndex;                             // ��������
	BYTE                  m_byPartID;                                 // ��λID
	BYTE                  m_byLinkID;                                 // �����ID

	// ���²���ʾ����
	bool                  m_bModelExist;                              // �Ƿ��趨��Model
	bool                  m_bForseHide;                               // ǿ�Ʋ���ʾ

	// model effect
	//BTList*               m_pModelEffectList;                         // װ��ר��ģ����ЧList

	// ״̬װ����Ч
	//sStatusEffect*        m_pStatusEffect;                            // ״̬װ����Ч

	// ��������
	cParticleEffectBase*  m_pWeaponRibbon;                            // ����������
	bool                  m_bShowWeaponRibbon;                        // �Ƿ���ʾ����

	int                   m_iTextureID;

	// װ����Ч
	DWORD                 m_dwParticleEffectSize;                     // ��������
	cParticleEffectBase** m_ppParticleEffects;                        // ����
	float                 m_fCurrentAnimTime;                         // Ŀǰ����ʱ��
	float                 m_fAnimElapsedTime;                         // ��������ʱ��
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
