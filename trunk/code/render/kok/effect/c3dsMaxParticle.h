#ifndef c3dsMaxParticleH
#define c3dsMaxParticleH
//------------------------------------------------------------------------------
/**
	@class KOK::cParticle

	(C) 2008 cTuo
*/

#include "math/vector.h"
#include "util/string.h"
#include "io/stream.h"
#include "kok/material/material.h"

namespace KOK
{

//#include "..\Scene\c3dsMaxParticleRender.h"

#define MAX_DECAL_VERTICES    256   // Decal���Vertex����
#define DECAL_EPSILON         0.25f

struct cParticle
{
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vDir;
	D3DXCOLOR m_color;
	float        m_fAngle;
	float        m_fSize;
	float        m_fLifeTime;
	float        m_fTotalLifeTime;

	DWORD        m_dwTextureAniIndex;
	float        m_fTextureAniTime;

	cParticle*   m_pPrevParticle;
	cParticle*   m_pNextParticle;
};


class cParticlePool
{
	friend class cParticleEffectBase;

public:
	cParticlePool( DWORD dwCount, void* pPool, DWORD dwDataSize );
	~cParticlePool();

	cParticle* NewParticle( void );
  
protected:
	char*       m_pParticlePool;
	DWORD       m_dwParticleDataSize;
	bool		m_bInternallyAllocated;
	DWORD       m_dwTotalParticleCount;
	DWORD       m_dwActiveParticleCount;
	cParticle*  m_pFirstFreeParticle;
	cParticle*  m_pLastFreeParticle;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class c3dsMaxEmitDataBase
{
public:
  c3dsMaxEmitDataBase();
  virtual ~c3dsMaxEmitDataBase();

  virtual void ImportFromMemory( const Ptr<IO::Stream>& stream ) = 0;
  void ImportMaterialFromMemory( const Ptr<IO::Stream>& stream, int iIndex );
  void ImportMaterialAnimationFromMemory( const Ptr<IO::Stream>& stream, int iIndex );

  DWORD GetDataType( void ) { return m_dwDataType; }
  Util::String GetLinkName( void ) { return m_pLinkName; }

  DWORD GetMaterialSize( void ) { return m_dwNumMaterial; }
  cMaterial* GetMaterial( DWORD dwIndex );
  cMaterialAnimator* GetMaterialAnimator( DWORD dwIndex );

protected:
  DWORD       m_dwDataType;
  Util::String m_pLinkName;

  DWORD       m_dwNumMaterial;
  cMaterial*  m_pMaterial;
  cMaterialAnimator*  m_pMaterialAnimator;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh particle cloud
struct sPCloudInfo
{
  DWORD m_dwFormation;
  float m_fMoveSpeed;
  float m_fMoveSpeedVariation;
  DWORD m_dwMoveType;
  DWORD m_dwBirthMethod;
  DWORD m_dwBirthRate;
  DWORD m_dwBirthTotal;
  DWORD m_dwEmitStart;
  DWORD m_dwEmitStop;
  DWORD m_dwLife;
  DWORD m_dwLifeVariation;
  float m_fSize;
  float m_fSizeVariation;
  DWORD m_dwGrowTime;
  DWORD m_dwFadeTime;
  float m_fEmitWidth;
  float m_fEmitHeight;
  float m_fEmitDepth;
  DWORD m_dwSpinTime;
  float m_fSpinVariation;
  float m_fSpinPhaseDeg;
  float m_fSpinPhaseDegVariation;

  sPCloudInfo();
};

class c3dsMaxEmitDataCloud : public c3dsMaxEmitDataBase
{
public:
  c3dsMaxEmitDataCloud();

  virtual void ImportFromMemory( const Ptr<IO::Stream>& stream );
  
  sPCloudInfo& GetPCloudInfo( void ) { return m_sPCloudInfo; }
  

protected:
  sPCloudInfo m_sPCloudInfo;
};


////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh particle superspray
struct sSuperSprayInfo
{
  float m_fOffAxisDeg;
  float m_fOffAxisSpreadDeg;
  float m_fOffPlaneDeg;
  float m_fOffPlaneSpreadDeg;
  float m_fMoveSpeed;
  float m_fMoveSpeedVariation;
  DWORD m_dwBirthMethod;
  DWORD m_dwBirthRate;
  DWORD m_dwBirthTotal;
  DWORD m_dwEmitStart;
  DWORD m_dwEmitStop;
  DWORD m_dwLife;
  DWORD m_dwLifeVariation;
  float m_fSize;
  float m_fSizeVariation;
  DWORD m_dwGrowTime;
  DWORD m_dwFadeTime;
  float m_fEmitWidth;
  DWORD m_dwSpinTime;
  float m_fSpinVariation;
  float m_fSpinPhaseDeg;
  float m_fSpinPhaseDegVariation;

  sSuperSprayInfo();
};

// 070205 cyhsieh particle superspray
class c3dsMaxEmitDataSuperSpray : public c3dsMaxEmitDataBase
{
public:
  c3dsMaxEmitDataSuperSpray();

  virtual void ImportFromMemory( const Ptr<IO::Stream>& stream );
  
  sSuperSprayInfo& GetSuperSprayInfo( void ) { return m_sSuperSprayInfo; }

  void CreateSpecialMaterialAnimator( void );

protected:
  sSuperSprayInfo m_sSuperSprayInfo;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class cParticleEffectBase
{
//public:
//	cParticleEffectBase( cParticlePool* pParticlePool );
//	virtual ~cParticleEffectBase();
//
//	void AddParticle( cParticle* pParticle );
//	void AddParticleFirst( cParticle* pParticle );
//	void FreeParticle( cParticle* pParticle );
//
//	virtual void FrameMove( float fElapsedTime ) = 0;
//	virtual void DisableFrameMove( float fElapsedTime ) {}
//	virtual void Draw( LPDIRECT3DDEVICE9 pD3DDevice );
//
//	void SetCurPosition( D3DXVECTOR3& vPos ) { m_vCurPosition = vPos; }
//	D3DXVECTOR3& GetCurPosition( void ) { return m_vCurPosition; }
//	void SetLinkName( Util::String pName );
//	Util::String GetLinkName( void ) { return m_pLinkName; }
//	c3dsMaxParticleRender* GetRender( void ) { return m_pRender; }
//
//	virtual void InitTransformMatrix( D3DXMATRIXA16* pTransform ) {}
//	virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform ) {}
//	virtual void ApplyBoundingBox( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax ) {}
//
//	// 070213 cyhsieh Particle Emit Time
//	virtual void SetCurAnimTime( float fTime ) {}
//
//	DWORD GetRenderAlphaBlendType( void );
//
//	bool GetRenderEnabled( void )
//	{
//		if( m_pRender ) return m_pRender->bEnabled;
//			return false;
//	}
//
//	float ComputeFogAlphaFactor( void );
//
//protected:
//	cParticlePool*  m_pParticlePool;
//
//	cParticle*      m_pFirstUsedParticle;
//	cParticle*      m_pLastUsedParticle;
//
//	Util::String           m_pLinkName;
//	D3DXVECTOR3     m_vCurPosition;
//
//	c3dsMaxParticleRender* m_pRender;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class cParticleCloud : public cParticleEffectBase
{
//public:
//	cParticleCloud( cParticlePool* pParticlePool, c3dsMaxEmitDataCloud* pEmitDataCloud );
//
//	virtual void FrameMove( float fElapsedTime );
//	virtual void DisableFrameMove( float fElapsedTime );
//
//	virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );
//	virtual void ApplyBoundingBox( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax );
//
//	// 070213 cyhsieh Particle Emit Time
//	virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }
//
//	protected:
//	sPCloudInfo m_sPCloudInfo;
//
//	float       m_fTimeSinceLastEmit;
//
//	// 070213 cyhsieh Particle Emit Time
//	float       m_fCurAnimTime;
//
//	bool        m_bEmitParticles;
//
//	D3DXVECTOR3 m_vEmitDirRight;
//	D3DXVECTOR3 m_vEmitDirUp;
//	D3DXVECTOR3 m_vEmitDirForward;
//	D3DXVECTOR3 m_vEmitForceDir;
//	D3DXVECTOR3 m_vCenterOffset;
//
//	D3DXMATRIXA16 m_matRotate;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh particle superspray
class cParticleSuperSpray : public cParticleEffectBase
{
//public:
//	cParticleSuperSpray( cParticlePool* pParticlePool, c3dsMaxEmitDataSuperSpray* pEmitDataSuperSpray );
//
//	virtual void FrameMove( float fElapsedTime );
//	virtual void DisableFrameMove( float fElapsedTime );
//
//	virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );
//
//	// 070213 cyhsieh Particle Emit Time
//	virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }
//
//	protected:
//	sSuperSprayInfo m_sSuperSprayInfo;
//
//	float       m_fTimeSinceLastEmit;
//
//	// 070213 cyhsieh Particle Emit Time
//	float       m_fCurAnimTime;
//
//	bool        m_bEmitParticles;
//
//	D3DXVECTOR3 m_vEmitDirRight;
//	D3DXVECTOR3 m_vEmitDirForward;
//	D3DXVECTOR3 m_vEmitForceDir;
//
//	// 070214 cyhsieh spray���䷽��
//	D3DXMATRIXA16 m_matTransForm;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh ribbon
class cParticleRibbon : public cParticleEffectBase
{
	// cTuo

//public:
//  cParticleRibbon( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer );
//
//  virtual void FrameMove( float fElapsedTime );
//  virtual void DisableFrameMove( float fElapsedTime );
//
//  virtual void InitTransformMatrix( D3DXMATRIXA16* pTransform );
//  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );
//
//protected:
//  D3DXVECTOR3 m_vDirUp;
//  D3DXVECTOR3 m_vLastPos;
//  float       m_fAddSegmentLength;
//  float       m_fMaxRibbonLength;
//  float       m_fRibbonWidth;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
// �����������ã�������������������õ��������Ƶ㣬�����������Ƶ�����Strip
class cParticleWeaponRibbon : public cParticleEffectBase
{
//public:
//  cParticleWeaponRibbon( cParticlePool* pParticlePool, bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos );
//  ~cParticleWeaponRibbon();
//
//  virtual void FrameMove( float fElapsedTime );
//  virtual void DisableFrameMove( float fElapsedTime );
//
//  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );
//
//protected:
//  bool        m_bAddSegment;
//  D3DXVECTOR3 m_vOrigMinPos;
//  D3DXVECTOR3 m_vOrigMaxPos;
//  D3DXVECTOR3 m_vMinPos;
//  D3DXVECTOR3 m_vMaxPos;
//
//  struct sLerpInfo
//  {
//    D3DXMATRIXA16   m_matCurTransform;  // Ŀǰ��transform
//    D3DXVECTOR3     m_vLastMaxPos;      // �ϴε�λ��
//    D3DXVECTOR3     m_vLastScale;       // �ϴ�transform��scale
//    D3DXQUATERNION  m_quatLastRotation; // �ϴ�transform��rotation
//    D3DXVECTOR3     m_vLastTranslation; // �ϴ�transform��translation
//  };
//  sLerpInfo*  m_pLerpInfo;  // Lerp����Ѷ
};


////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh billboard
class cParticleBillboard : public cParticleEffectBase
{
	//cTuo

//public:
//  cParticleBillboard( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer, DWORD dwBillboardType );
//
//  virtual void FrameMove( float fElapsedTime );
//
//  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );
//
//  // Billboard Animation Time
//  virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }
//
//protected:
//  D3DXVECTOR3 m_vCorner[3];
//  float       m_fWidth;
//  DWORD       m_dwBillboardType;
//
//  // Billboard Animation Time
//  float       m_fCurAnimTime;
};


//---------------------------------------------------------------------------
// Name: cParticleZodiac
// Desc: ���ڵر���ر�߶������ƽ����Ч
//---------------------------------------------------------------------------
class cParticleZodiac : public cParticleEffectBase
{
	// cTuo

//public:
//  cParticleZodiac( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer );
//
//  virtual void FrameMove( float fElapsedTime );
//
//  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );
//
//  virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }
//
//protected:
//  D3DXVECTOR3 m_vCorner[3];
//  float       m_fHalfWidth;
//  float       m_fCurAnimTime;
//
//  // �H�U�����ͥi���a�W��Decal��(���ͤ�k�Ѧ�Game Programming Gems2 4.8)
//  int ClipDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex );
//  int ClipDecalPolygonAgainstPlane( const D3DXPLANE& plane, int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex );
//  bool AddDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex );
//
//  static D3DXVECTOR3 m_vDecalVertices[MAX_DECAL_VERTICES];    // Decal Vertices
//  static DWORD       m_dwDecalVertexCount;                    // Decal Vertex����
//  static WORD        m_wDecalIndices[MAX_DECAL_VERTICES * 3]; // Decal Indices
//  static DWORD       m_dwDecalIndexCount;                     // Decal Index����
//  static D3DXPLANE   m_planeLeft;                             // Boundary Clip Plane
//  static D3DXPLANE   m_planeRight;
//  static D3DXPLANE   m_planeBack;
//  static D3DXPLANE   m_planeFront;
};


////////////////////////////////////////////////////////////////////////////////////////////////

class c3dsMaxParticleManager
{
	//cTuo

//public:
//  c3dsMaxParticleManager();
//  ~c3dsMaxParticleManager();
//
//  void InitializeResource( LPDIRECT3DDEVICE9 pD3DDevice );                  // ���ʼ
//  
//  // ������Ч
//  cParticleEffectBase* CreateParticleEffectByEmitData( c3dsMaxEmitDataBase* pEmitData, char* pTexurePath = NULL );  // ����Max�ɤl�S��
//  
//  // ������Ч��
//  cParticleEffectBase* CreateParticleEffectBySubMeshSerializer( cSubMeshSerializer* pSubMeshSerializer, char* pTexurePath = NULL ); // ����Billboard�ɤl�S��
//
//  // ���������
//  cParticleEffectBase* CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath = NULL );  // ���ͤM���S��
//
//  void ReleaseParticleEffect( cParticleEffectBase* pRelease );              // �ͷ�����ϵͳ
//
//  void FrameMove( float fElapsedTime );                                     // Framemove
//  void Draw( LPDIRECT3DDEVICE9 pD3DDevice );                                // Draw
//
//  // ��Ч��Ч
//  void SetEnable( bool bEnable ) { m_bEnable = bEnable; }                   // �趨�Ƿ�������
//
//  // ��Ч
//  void AddForceDrawParticleEffect( cParticleEffectBase* pParticleEffect );  // ������Ƭ��������Ч
//  void ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice );                           // ǿ�ƻ�������Ƭ�ã�
//
//  static void SetCameraParam( const D3DXVECTOR3& vCameraPos, const D3DXMATRIXA16& matCameraView ) // �趨��Ӱ������
//  { 
//    m_vCameraPos = vCameraPos;
//    m_matCameraView = matCameraView;
//  }
//  static const D3DXVECTOR3& GetCameraPos( void ) { return m_vCameraPos; }             // ȡ����Ӱ��λ��
//  static const D3DXMATRIXA16& GetCameraViewMatrix( void ) { return m_matCameraView; } // ȡ����Ӱ��View����
//
//protected:
//  // ��Ч��Ч
//  bool            m_bEnable;                                                // �Ƿ�������
//
//  cRenderBuffer*  m_pRenderBuffer;                                          // Render Buffer
//  cTextureDB*     m_pTextureDB;                                             // ��ͼDB
//  cParticlePool*  m_pParticlePool;                                          // ����Pool
//
//  cRenderBuffer*  m_pDecalIndexBuffer;                                      // Index Buffer(��cParticleZodiac Decal��)
//
//  // 071225 cyhsieh �S��
//  static D3DXVECTOR3 m_vCameraPos;                                          // ��Ӱ��λ��(������Alpha��)
//  static D3DXMATRIXA16 m_matCameraView;                                     // ��Ӱ��View�x�}(��������������)
//
//  BTList*         m_pParticleEffectList;                                    // ����������ЧList
//  BTList*         m_pParticleEffectSortList;                                // ������
//
//  // ��Ч
//  BTList*         m_pForceDrawParticleEffectList;                           // ����Ƭ��������ЧList
};
}
#endif