#ifndef cSkinnedCharacterEquipPartH
#define cSkinnedCharacterEquipPartH
//------------------------------------------------------------------------------
/**
	@class KOK::cSkinnedCharacterEquipPart

	(C) 2008 cTuo
*/
#include "cEquipModelDB.h"
#include "..\..\Effect\cModelEffect.h"

namespace KOK
{

class cSkinnedCharacterEquipPart
{
private:
	struct sStatusEffect
	{
		BYTE                m_byType;                                   // ״̬����
		cModelEffect*       m_pModelEffect;                             // ״̬��Ч
		sStatusEffect()
			:m_byType(0), m_pModelEffect(NULL)
		{
		}
	};

	// cSkeletonHierarchy of equip or weapon for skinned character model.
	cSkeletonHierarchy*   m_pSkeletonHierarchy;
	// matrix of equip or weapon for skinned character model.
	D3DXMATRIXA16         m_matEqMatrix;

	//  ����
	int                   m_iActionIndex;                             // ��������
	BYTE                  m_byPartID;                                 // ��λID
	BYTE                  m_byLinkID;                                 // �����ID

	// ���²���ʾ����
	bool                  m_bModelExist;                              // �Ƿ��趨��Model
	bool                  m_bForseHide;                               // ǿ�Ʋ���ʾ

	// model effect
	BTList*               m_pModelEffectList;                         // װ��ר��ģ����ЧList

	// ״̬װ����Ч
	sStatusEffect*        m_pStatusEffect;                            // ״̬װ����Ч

	// ��������
	cParticleEffectBase*  m_pWeaponRibbon;                            // ����������
	bool                  m_bShowWeaponRibbon;                        // �Ƿ���ʾ����

	//   �޸�װ��Model Render�ܹ�(һ��SubMeshSerializer��һ��Render)
	int                   m_iTextureID;
	cEquipModel*          m_pEquipModel;                              // װ��Model
	int                   m_iRenderSize;                              // Render����
	stRENDER_ARRAY*       m_pRenderArray;                             // Render

	// װ����Ч
	DWORD                 m_dwParticleEffectSize;                     // ��������
	cParticleEffectBase** m_ppParticleEffects;                        // ����
	float                 m_fCurrentAnimTime;                         // Ŀǰ����ʱ��
	float                 m_fAnimElapsedTime;                         // ��������ʱ��

	// Thread
	BYTE                  m_byWaitResourceCount;                      // �ȴ�Thread��ȡ��ģ����Դ����

	public:
	cSkinnedCharacterEquipPart( BYTE byPartID, BYTE byLinkID );
	virtual ~cSkinnedCharacterEquipPart();

	// set cSkeletonHierarchy of equip or weapon for skinned character model.
	void setEquipSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy );
	// getcSkeletonHierarchy of equip or weapon for skinned character model.
	cSkeletonHierarchy* getEquipSkeletonHierarchy();
	// get visible of equip or weapon for skinned character model.
	bool getEquipVisible();

	// ����model����ᶶ
	void SetWorldTransform( const D3DXMATRIXA16& matTransform );

	//   �޸�װ��Model Render�ܹ�
	int GetTextureID( void ) { return m_iTextureID; }
	void SetEquipModel( cEquipModel* pEquipModel, int iTextureID, bool bWeaponRibbonLerp, cBeing* pOwnerBeing ); // �趨ģ��Model
	cEquipModel* GetEquipModel( void ) { return m_pEquipModel; }
	const D3DXMATRIXA16& GetEqMatrix( void ) { return m_matEqMatrix; }
	void SetFade( float fValue );                                     // �趨��������
	void SetShadowData( sShadowData* pShadowData );                   // �趨��Ӱ����

	// װ����Ч
	void TickAnimTime( float fElapsedTime );                          // ����Animationʱ��
	void ComputeAnimation( void );                                    // ����Animation

	// model effect
	void AddEquipModelEffect( char* pName, BYTE byStatusType );       // ����һ��װ��ר��ģ����Ч

	// ״̬װ����Ч
	void RemoveStatusModelEffect( void );                             // �Ƴ�״̬װ����Ч

	// ��������
	void SetShowWeaponRibbon( bool bShow ) { m_bShowWeaponRibbon = bShow; } // �趨�Ƿ���ʾ����
	// ȡ��renderer
	cRender* GetEqRender(int index) { if ((m_pRenderArray)&&
	(m_iRenderSize>index)) return m_pRenderArray[index].m_pRender; else return 
	NULL; };
	int GetEqRenderCount() { return m_iRenderSize; };

	// װ������������
	void ForceDraw( LPDIRECT3DDEVICE9 pD3DDDevice, D3DXMATRIXA16* pTransform ); 
	// ǿ�ƻ�(����Ƭ��)

	// ���
	void ForceAlpha( float falpha );                                  // ǿ���趨Render��Alphaֵ

	// ������ͼ����
	void SetFakeReflect( bool bShow );                                // �趨�Ƿ����ٷ���

	// ��������
	void AddToScene( bool bUseFocusLight );                           // �ӵ�Scene
	void SetFocusLight( void );                                       // ��Render���focus

	// ���²���ʾ����
	bool IsModelExist( void ) { return m_bModelExist; }               // �Ƿ���Model

	// ���²���ʾ����
	void SetForceHide( bool bHide ) { m_bForseHide = bHide; }         // �趨�Ƿ�ǿ������

	//  ����
	void SetActionIndex( int iActionIndex );                          // �趨����
	BYTE GetPartID( void ) { return m_byPartID; }                     // ȡ�ò�λID
	BYTE GetLinkID( void ) { return m_byLinkID; }                     // ȡ�������ID

	// ��Ч
	int GetEquipModelEffectCount( void );                             // ȡ��װ����Ч����
	char* GetEquipModelEffectName( int iIndex );                      // ȡ��װ����Ч����

	// Thread
	bool GetResourceReady( void )                                     // ģ�������Դ�Ƿ��ȡ���
	{
	return ( m_byWaitResourceCount == 0 ); 
	}
	void ResourceFinishLoad( void )                                   // ģ�������Դ��ȡ����
	{
	if( m_byWaitResourceCount > 0 )
	{
	m_byWaitResourceCount--;
	}
	}
};

}

#endif
