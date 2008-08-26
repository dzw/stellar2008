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

#define MAX_DECAL_VERTICES    256   // Decal最大Vertex数量
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
//	// 070214 cyhsieh spray喷射方向
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
// 武器刀光
// 给武器刀光用，武器会有特殊给刀光用的两个控制点，以这两个控制点连成Strip
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
//    D3DXMATRIXA16   m_matCurTransform;  // 目前的transform
//    D3DXVECTOR3     m_vLastMaxPos;      // 上次的位置
//    D3DXVECTOR3     m_vLastScale;       // 上次transform的scale
//    D3DXQUATERNION  m_quatLastRotation; // 上次transform的rotation
//    D3DXVECTOR3     m_vLastTranslation; // 上次transform的translation
//  };
//  sLerpInfo*  m_pLerpInfo;  // Lerp用资讯
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
// Desc: 贴在地表，随地表高度起伏的平面特效
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
//  // 玻ネDecalノ(玻ネよ猭把σGame Programming Gems2 4.8)
//  int ClipDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex );
//  int ClipDecalPolygonAgainstPlane( const D3DXPLANE& plane, int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex );
//  bool AddDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex );
//
//  static D3DXVECTOR3 m_vDecalVertices[MAX_DECAL_VERTICES];    // Decal Vertices
//  static DWORD       m_dwDecalVertexCount;                    // Decal Vertex数量
//  static WORD        m_wDecalIndices[MAX_DECAL_VERTICES * 3]; // Decal Indices
//  static DWORD       m_dwDecalIndexCount;                     // Decal Index数量
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
//  void InitializeResource( LPDIRECT3DDEVICE9 pD3DDevice );                  // 初始
//  
//  // 地物特效
//  cParticleEffectBase* CreateParticleEffectByEmitData( c3dsMaxEmitDataBase* pEmitData, char* pTexurePath = NULL );  // 玻ネMax采疭
//  
//  // 地物特效
//  cParticleEffectBase* CreateParticleEffectBySubMeshSerializer( cSubMeshSerializer* pSubMeshSerializer, char* pTexurePath = NULL ); // 玻ネBillboard采疭
//
//  // 武器刀光
//  cParticleEffectBase* CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath = NULL );  // 玻ネ疭
//
//  void ReleaseParticleEffect( cParticleEffectBase* pRelease );              // 释放粒子系统
//
//  void FrameMove( float fElapsedTime );                                     // Framemove
//  void Draw( LPDIRECT3DDEVICE9 pD3DDevice );                                // Draw
//
//  // 长效特效
//  void SetEnable( bool bEnable ) { m_bEnable = bEnable; }                   // 设定是否有作用
//
//  // 特效
//  void AddForceDrawParticleEffect( cParticleEffectBase* pParticleEffect );  // 增加照片用粒子特效
//  void ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice );                           // 强制画（给照片用）
//
//  static void SetCameraParam( const D3DXVECTOR3& vCameraPos, const D3DXMATRIXA16& matCameraView ) // 设定摄影机参数
//  { 
//    m_vCameraPos = vCameraPos;
//    m_matCameraView = matCameraView;
//  }
//  static const D3DXVECTOR3& GetCameraPos( void ) { return m_vCameraPos; }             // 取得摄影机位置
//  static const D3DXMATRIXA16& GetCameraViewMatrix( void ) { return m_matCameraView; } // 取得摄影机View矩阵
//
//protected:
//  // 长效特效
//  bool            m_bEnable;                                                // 是否有作用
//
//  cRenderBuffer*  m_pRenderBuffer;                                          // Render Buffer
//  cTextureDB*     m_pTextureDB;                                             // 贴图DB
//  cParticlePool*  m_pParticlePool;                                          // 粒子Pool
//
//  cRenderBuffer*  m_pDecalIndexBuffer;                                      // Index Buffer(给cParticleZodiac Decal用)
//
//  // 071225 cyhsieh 疭
//  static D3DXVECTOR3 m_vCameraPos;                                          // 摄影机位置(计算雾Alpha用)
//  static D3DXMATRIXA16 m_matCameraView;                                     // 摄影机View痻皚(计算粒子面向用)
//
//  BTList*         m_pParticleEffectList;                                    // 储存粒子特效List
//  BTList*         m_pParticleEffectSortList;                                // 排序用
//
//  // 特效
//  BTList*         m_pForceDrawParticleEffectList;                           // 画照片用粒子特效List
};
}
#endif