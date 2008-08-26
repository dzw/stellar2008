#ifndef cModelEffectH
#define cModelEffectH

#include "..\Model\cMeshModel.h"
// Antony 970626 ���F���v�\��ݨϥ�mud_fCallInfo���c
#include "../../NetWork/cNetWork.h"
// Antony End

// 070226 cyhsieh model effect table
struct sModelEffectInfo
{
  char* m_pFileName;        // �S���ɦW
  BYTE  m_byLinkID;         // �s����m

  char* m_pSoundName;       // �����ɦW
  DWORD m_dwSoundDelay;     // ���ĩ���

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

// 080328 cyhsieh �ѥͯS��
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
  enum EMoveType      // ����S�Ĳ�������
  {
    NONE = 0,         // �L
    NORMAL,           // �@��
    STRAIGHT,         // ���u
    FORTHBACK,        // �h�^
    FORTHBACK_HIDE,   // �h�^(���äH��)
    //================//
    FORTHBACK_RETURN, // �h�^(�^�{)
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

  float SetTargetPos( const D3DXVECTOR3& vTargetPos, BYTE byMoveType, float fMoveSpeed ); // �]�w����S�ĥؼЦ�m
  BYTE GetMoveType( void )                                  // ���o����S�Ĳ�������
  {
    if( m_pMoveToInfo ) return m_pMoveToInfo->m_byMoveType;
    return NONE;
  }

  // 070205 cyhsieh test effect target
  bool GetIsDead( void ) { return m_bDead; }
  
  // 070226 cyhsieh model effect table
  static void InitEffectInfoList( void );                   // ��l�ƯS�ĸ�T�C��
  static void DeleteEffectInfoList( void );                 // �R���S�ĸ�T�C��
  static sModelEffectInfo* GetModelEffectInfo( DWORD dwEffectID );  // ���o�S�ĸ�T
  static int  GetEffectInfoCount()
  {
    return (m_pEffectInfoMap != NULL) ? m_pEffectInfoMap->GetTotalCount() : 0;
  }

  // 080328 cyhsieh �ѥͯS��
  static void InitBornEffectList( void );                   // ��l�ƤѥͯS�ĦC��
  static void DeleteBornEffectList( void );                 // �R���ѥͯS�ĦC��
  static sBornEffectInfo* GetBornEffect( DWORD dwBornEffectID );  // ���o�ѥͯS�ĸ�T

  // 070226 cyhsieh model effect
  void SetLoop( bool bLoop ) { m_bLoop = bLoop; }
  bool GetLoop( void ) { return m_bLoop; }
  void SetEffectID( WORD wEffectID ) { m_wEffectID = wEffectID; }
  WORD GetEffectID( void ) { return m_wEffectID; }

  // 070412 cyhsieh ���įS��
  void SetGuid( char* pGuid );
  char* GetGuid( void ) { return m_pGuid; }

  // 070917 cyhsieh �S��
  void SetEnable( bool bEnable ) { m_bEnable = bEnable; }   // �]�w�O�_���@��

  // 071225 cyhsieh �S��
  char* GetMeshModelName( void );                           // ���o�ҫ��W��
  void ForceDrawParticleEffects( void );                    // �j��e�ɤl�S��(���Ӥ���)

  void SetAudioManage(cAudioManage* pAudioManage) { m_pAudioManage = pAudioManage; }
  void PlayAudio();

  void SetForceDir( float fDirAngle );                      // �]�w�j������V
  const D3DXMATRIXA16* GetForceRotation( void ) { return m_pForceRotation; }  // ���o�j�����x�}

protected:
  void ComputeAnimation( float fElapsedTime );
  void UpdateAnimTimeOnly( float fElapsedTime );            // ��S�@�ήɡA�ȧ�s�ʵe�ɶ�

  static bool ParseModelEffectInfo( cECReader& ecReader );  // Ū���S�ĦC��
  static bool ParseSound(cECReader& Reader, sModelEffectInfo* pMEI);  // Ū���S�ĦC��

  cMeshModel*             m_pMeshModel;                     // �ҫ�
  int                     m_iRenderArraySize;
  stEFFECT_RENDER_ARRAY*  m_pRenderArray;

  float                   m_fCurrentAnimTime;               // �ʵe�ɶ�

  D3DXMATRIXA16           m_matTransform;

  D3DXMATRIXA16*          m_pForceRotation;                 // �j�����x�}

  DWORD                   m_dwParticleEffectSize;           // �ɤl�S��
  cParticleEffectBase**   m_ppParticleEffects;

  BYTE                    m_byMagicLinkID;                  // �s����m
  DWORD                   m_dwSpecialType;                  // �S�����

  // 070917 cyhsieh �S��
  bool                    m_bEnable;                        // �O�_���@��
  
  bool                    m_bDead;                          // �O�_�H���`
  bool                    m_bLoop;                          // �O�_���Ƽ���
  WORD                    m_wEffectID;                      // �S��ID
  char*                   m_pGuid;                          // ���įS��GUID

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
  sMoveToInfo*            m_pMoveToInfo;                    // ����S�ĸ�T

  cAudioManage*           m_pAudioManage;
  cAudio*                 m_pNowAudio;                      // �ثe�S�Ī��n��

  // 070226 cyhsieh model effect table
  static cHashMap*        m_pEffectInfoMap;                 // �S�ĸ�T�C��
  // 080328 cyhsieh �ѥͯS��
  static BTList*          m_pBornEffectList;                // �ѥͯS�ĦC��

	// Antony 970625 ���F���v
public:
	// fMT_CCharBuild �u�ݰ���@��
	// �Hg_pGameTerrain->m_pEffectFromPackageList->GetItem()���oModelEffect
	mud_fCallInfo *OutputCharBuild();
	inline void SetMapX( float x ) { m_fMapX = x; }
	inline void SetMapZ( float z ) { m_fMapZ = z; }

private:
	float m_fMapX;
	float m_fMapZ;
	// Antony End
};

#endif