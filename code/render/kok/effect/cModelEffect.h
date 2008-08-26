#ifndef cModelEffectH
#define cModelEffectH

#include "..\Model\cMeshModel.h"
// Antony 970626 為了錄影功能需使用mud_fCallInfo結構
#include "../../NetWork/cNetWork.h"
// Antony End

// 070226 cyhsieh model effect table
struct sModelEffectInfo
{
  char* m_pFileName;        // 特效檔名
  BYTE  m_byLinkID;         // 連結位置

  char* m_pSoundName;       // 音效檔名
  DWORD m_dwSoundDelay;     // 音效延遲

  sModelEffectInfo()
  :m_pFileName(NULL), m_byLinkID(255), m_pSoundName(NULL),
   m_dwSoundDelay(0)
  {
  }

  ~sModelEffectInfo()
  {
    SAFE_DELETE_ARRAY( m_pFileName );
    SAFE_DELETE_ARRAY( m_pSoundName );
  }
};

// 080328 cyhsieh 天生特效
#define MAX_BORNEFFECT_COUNT  8
struct sBornEffectInfo
{
  DWORD m_dwBornEffect[MAX_BORNEFFECT_COUNT];

  sBornEffectInfo()
  {
    memset( m_dwBornEffect, 0xFF, sizeof(DWORD) * MAX_BORNEFFECT_COUNT );
  }
};

// 070205 cyhsieh model effect
class cModelEffect
{
public:
  enum EMoveType      // 飛行特效移動類型
  {
    NONE = 0,         // 無
    NORMAL,           // 一般
    STRAIGHT,         // 直線
    FORTHBACK,        // 去回
    FORTHBACK_HIDE,   // 去回(隱藏人物)
    //================//
    FORTHBACK_RETURN, // 去回(回程)
  };

  cModelEffect();
  ~cModelEffect();

  void LoadMesh( char* pFileName );
  void FrameMove( float fElapsedTime );

  void InitTransform( const D3DXMATRIXA16& matTransform );

  void SetTransform( const D3DXMATRIXA16& matTransform ) { if( m_pMoveToInfo == NULL ) m_matTransform = matTransform; }

  void SetMagicLinkID( BYTE byMagicLinkID ) { m_byMagicLinkID = byMagicLinkID; }
  BYTE GetMagicLinkID( void ) { return m_byMagicLinkID; }

  void ApplyTransformMatrix( float fElapsedTime, D3DXMATRIXA16* pTransform );
  void ApplyBoundingBox( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax );

  float SetTargetPos( const D3DXVECTOR3& vTargetPos, BYTE byMoveType, float fMoveSpeed ); // 設定飛行特效目標位置
  BYTE GetMoveType( void )                                  // 取得飛行特效移動類型
  {
    if( m_pMoveToInfo ) return m_pMoveToInfo->m_byMoveType;
    return NONE;
  }

  // 070205 cyhsieh test effect target
  bool GetIsDead( void ) { return m_bDead; }
  
  // 070226 cyhsieh model effect table
  static void InitEffectInfoList( void );                   // 初始化特效資訊列表
  static void DeleteEffectInfoList( void );                 // 刪除特效資訊列表
  static sModelEffectInfo* GetModelEffectInfo( DWORD dwEffectID );  // 取得特效資訊
  static int  GetEffectInfoCount()
  {
    return (m_pEffectInfoMap != NULL) ? m_pEffectInfoMap->GetTotalCount() : 0;
  }

  // 080328 cyhsieh 天生特效
  static void InitBornEffectList( void );                   // 初始化天生特效列表
  static void DeleteBornEffectList( void );                 // 刪除天生特效列表
  static sBornEffectInfo* GetBornEffect( DWORD dwBornEffectID );  // 取得天生特效資訊

  // 070226 cyhsieh model effect
  void SetLoop( bool bLoop ) { m_bLoop = bLoop; }
  bool GetLoop( void ) { return m_bLoop; }
  void SetEffectID( WORD wEffectID ) { m_wEffectID = wEffectID; }
  WORD GetEffectID( void ) { return m_wEffectID; }

  // 070412 cyhsieh 長效特效
  void SetGuid( char* pGuid );
  char* GetGuid( void ) { return m_pGuid; }

  // 070917 cyhsieh 特效
  void SetEnable( bool bEnable ) { m_bEnable = bEnable; }   // 設定是否有作用

  // 071225 cyhsieh 特效
  char* GetMeshModelName( void );                           // 取得模型名稱
  void ForceDrawParticleEffects( void );                    // 強制畫粒子特效(給照片用)

  void SetAudioManage(cAudioManage* pAudioManage) { m_pAudioManage = pAudioManage; }
  void PlayAudio();

  void SetForceDir( float fDirAngle );                      // 設定強制旋轉方向
  const D3DXMATRIXA16* GetForceRotation( void ) { return m_pForceRotation; }  // 取得強制旋轉矩陣

protected:
  void ComputeAnimation( float fElapsedTime );
  void UpdateAnimTimeOnly( float fElapsedTime );            // 當沒作用時，僅更新動畫時間

  static bool ParseModelEffectInfo( cECReader& ecReader );  // 讀取特效列表
  static bool ParseSound(cECReader& Reader, sModelEffectInfo* pMEI);  // 讀取特效列表

  cMeshModel*             m_pMeshModel;                     // 模型
  int                     m_iRenderArraySize;
  stEFFECT_RENDER_ARRAY*  m_pRenderArray;

  float                   m_fCurrentAnimTime;               // 動畫時間

  D3DXMATRIXA16           m_matTransform;

  D3DXMATRIXA16*          m_pForceRotation;                 // 強制旋轉矩陣

  DWORD                   m_dwParticleEffectSize;           // 粒子特效
  cParticleEffectBase**   m_ppParticleEffects;

  BYTE                    m_byMagicLinkID;                  // 連結位置
  DWORD                   m_dwSpecialType;                  // 特殊種類

  // 070917 cyhsieh 特效
  bool                    m_bEnable;                        // 是否有作用
  
  bool                    m_bDead;                          // 是否以死亡
  bool                    m_bLoop;                          // 是否重複播放
  WORD                    m_wEffectID;                      // 特效ID
  char*                   m_pGuid;                          // 長效特效GUID

  // 070205 cyhsieh test effect target
  struct sMoveToInfo
  {
    BYTE                  m_byMoveType;
    D3DXVECTOR3           m_vInitialPos;
    D3DXVECTOR3           m_vCurrentPos;
    D3DXVECTOR3           m_vRight;
    D3DXVECTOR3           m_vTargetPos;
    float                 m_fRotateY;
    float                 m_fMoveSpeed;
    float                 m_fLerp;
    float                 m_fLerpAddPS;
    float                 m_fAdjustHeight;
  };
  sMoveToInfo*            m_pMoveToInfo;                    // 飛行特效資訊

  cAudioManage*           m_pAudioManage;
  cAudio*                 m_pNowAudio;                      // 目前特效的聲音

  // 070226 cyhsieh model effect table
  static cHashMap*        m_pEffectInfoMap;                 // 特效資訊列表
  // 080328 cyhsieh 天生特效
  static BTList*          m_pBornEffectList;                // 天生特效列表

	// Antony 970625 為了錄影
public:
	// fMT_CCharBuild 只需執行一次
	// 以g_pGameTerrain->m_pEffectFromPackageList->GetItem()取得ModelEffect
	mud_fCallInfo *OutputCharBuild();
	inline void SetMapX( float x ) { m_fMapX = x; }
	inline void SetMapZ( float z ) { m_fMapZ = z; }

private:
	float m_fMapX;
	float m_fMapZ;
	// Antony End
};

#endif