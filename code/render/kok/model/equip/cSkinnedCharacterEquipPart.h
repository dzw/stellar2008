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
		BYTE                m_byType;                                   // 状态类型
		cModelEffect*       m_pModelEffect;                             // 状态特效
		sStatusEffect()
			:m_byType(0), m_pModelEffect(NULL)
		{
		}
	};

	// cSkeletonHierarchy of equip or weapon for skinned character model.
	cSkeletonHierarchy*   m_pSkeletonHierarchy;
	// matrix of equip or weapon for skinned character model.
	D3DXMATRIXA16         m_matEqMatrix;

	//  钓竿
	int                   m_iActionIndex;                             // 动作索引
	BYTE                  m_byPartID;                                 // 部位ID
	BYTE                  m_byLinkID;                                 // 连结点ID

	// 坐下不显示武器
	bool                  m_bModelExist;                              // 是否设定了Model
	bool                  m_bForseHide;                               // 强制不显示

	// model effect
	BTList*               m_pModelEffectList;                         // 装备专用模型特效List

	// 状态装备特效
	sStatusEffect*        m_pStatusEffect;                            // 状态装备特效

	// 武器刀光
	cParticleEffectBase*  m_pWeaponRibbon;                            // 刀光用粒子
	bool                  m_bShowWeaponRibbon;                        // 是否显示刀光

	//   修改装备Model Render架构(一个SubMeshSerializer有一个Render)
	int                   m_iTextureID;
	cEquipModel*          m_pEquipModel;                              // 装备Model
	int                   m_iRenderSize;                              // Render数量
	stRENDER_ARRAY*       m_pRenderArray;                             // Render

	// 装备特效
	DWORD                 m_dwParticleEffectSize;                     // 粒子数量
	cParticleEffectBase** m_ppParticleEffects;                        // 粒子
	float                 m_fCurrentAnimTime;                         // 目前动画时间
	float                 m_fAnimElapsedTime;                         // 动画经过时间

	// Thread
	BYTE                  m_byWaitResourceCount;                      // 等待Thread读取完模型资源数量

	public:
	cSkinnedCharacterEquipPart( BYTE byPartID, BYTE byLinkID );
	virtual ~cSkinnedCharacterEquipPart();

	// set cSkeletonHierarchy of equip or weapon for skinned character model.
	void setEquipSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy );
	// getcSkeletonHierarchy of equip or weapon for skinned character model.
	cSkeletonHierarchy* getEquipSkeletonHierarchy();
	// get visible of equip or weapon for skinned character model.
	bool getEquipVisible();

	// 修正model配件会抖
	void SetWorldTransform( const D3DXMATRIXA16& matTransform );

	//   修改装备Model Render架构
	int GetTextureID( void ) { return m_iTextureID; }
	void SetEquipModel( cEquipModel* pEquipModel, int iTextureID, bool bWeaponRibbonLerp, cBeing* pOwnerBeing ); // 设定模型Model
	cEquipModel* GetEquipModel( void ) { return m_pEquipModel; }
	const D3DXMATRIXA16& GetEqMatrix( void ) { return m_matEqMatrix; }
	void SetFade( float fValue );                                     // 设定淡出淡入
	void SetShadowData( sShadowData* pShadowData );                   // 设定阴影资料

	// 装备特效
	void TickAnimTime( float fElapsedTime );                          // 更新Animation时间
	void ComputeAnimation( void );                                    // 计算Animation

	// model effect
	void AddEquipModelEffect( char* pName, BYTE byStatusType );       // 新增一个装备专用模型特效

	// 状态装备特效
	void RemoveStatusModelEffect( void );                             // 移除状态装备特效

	// 武器刀光
	void SetShowWeaponRibbon( bool bShow ) { m_bShowWeaponRibbon = bShow; } // 设定是否显示刀光
	// 取得renderer
	cRender* GetEqRender(int index) { if ((m_pRenderArray)&&
	(m_iRenderSize>index)) return m_pRenderArray[index].m_pRender; else return 
	NULL; };
	int GetEqRenderCount() { return m_iRenderSize; };

	// 装备介面人物照
	void ForceDraw( LPDIRECT3DDEVICE9 pD3DDDevice, D3DXMATRIXA16* pTransform ); 
	// 强制画(给照片用)

	// 骑乘
	void ForceAlpha( float falpha );                                  // 强制设定Render的Alpha值

	// 反光贴图开关
	void SetFakeReflect( bool bShow );                                // 设定是否开启假反光

	// 反光整理
	void AddToScene( bool bUseFocusLight );                           // 加到Scene
	void SetFocusLight( void );                                       // 将Render设成focus

	// 坐下不显示武器
	bool IsModelExist( void ) { return m_bModelExist; }               // 是否有Model

	// 坐下不显示武器
	void SetForceHide( bool bHide ) { m_bForseHide = bHide; }         // 设定是否强制隐藏

	//  钓竿
	void SetActionIndex( int iActionIndex );                          // 设定动作
	BYTE GetPartID( void ) { return m_byPartID; }                     // 取得部位ID
	BYTE GetLinkID( void ) { return m_byLinkID; }                     // 取得连结点ID

	// 特效
	int GetEquipModelEffectCount( void );                             // 取得装备特效数量
	char* GetEquipModelEffectName( int iIndex );                      // 取得装备特效名称

	// Thread
	bool GetResourceReady( void )                                     // 模型相关资源是否读取完毕
	{
	return ( m_byWaitResourceCount == 0 ); 
	}
	void ResourceFinishLoad( void )                                   // 模型相关资源读取结束
	{
	if( m_byWaitResourceCount > 0 )
	{
	m_byWaitResourceCount--;
	}
	}
};

}

#endif
