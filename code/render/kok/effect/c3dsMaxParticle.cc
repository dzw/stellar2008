//------------------------------------------------------------------------------
//  c3dsmaxparticle.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "c3dsMaxParticle.h"
#include "memory/memory.h"
#include "kok/model/modeldef.h"

namespace KOK
{

using namespace IO;
using namespace Memory;

//D3DXVECTOR3 c3dsMaxParticleManager::m_vCameraPos;
//D3DXMATRIXA16 c3dsMaxParticleManager::m_matCameraView;
//
//D3DXVECTOR3 cParticleZodiac::m_vDecalVertices[MAX_DECAL_VERTICES];
//DWORD cParticleZodiac::m_dwDecalVertexCount = 0;
//WORD cParticleZodiac::m_wDecalIndices[MAX_DECAL_VERTICES * 3];
//DWORD cParticleZodiac::m_dwDecalIndexCount = 0;
//D3DXPLANE cParticleZodiac::m_planeLeft( 1.0f, 0.0f, 0.0f, 0.0f );
//D3DXPLANE cParticleZodiac::m_planeRight( -1.0f, 0.0f, 0.0f, 0.0f );
//D3DXPLANE cParticleZodiac::m_planeBack( 0.0f, 0.0f, 1.0f, 0.0f );
//D3DXPLANE cParticleZodiac::m_planeFront( 0.0f, 0.0f, -1.0f, 0.0f );

cParticlePool::cParticlePool( DWORD dwCount, void* pPool, DWORD dwDataSize )
:m_dwActiveParticleCount(0), m_pFirstFreeParticle(NULL), m_pLastFreeParticle(NULL)
{
  m_dwTotalParticleCount = dwCount;
  m_dwParticleDataSize = dwDataSize;

  if( pPool )
  {
    m_pParticlePool = (char*)pPool;
    m_bInternallyAllocated = false;
  }
  else
  {
    m_pParticlePool = n_new_array(char, dwCount * dwDataSize);
    m_bInternallyAllocated = true;
  }
}

cParticlePool::~cParticlePool()
{
  if( m_bInternallyAllocated )
  {
    delete[] m_pParticlePool;
  }
}

cParticle* cParticlePool::NewParticle( void )
{
  cParticle* pParticle = m_pFirstFreeParticle;
	if( pParticle )
	{
		cParticle* pNext = pParticle->m_pNextParticle;
		if( pNext )
		{
			m_pFirstFreeParticle = pNext;
			pNext->m_pPrevParticle = NULL;
		}
		else
		{
			m_pFirstFreeParticle = NULL;
			m_pLastFreeParticle = NULL;
		}
		
		return pParticle;
	}
	
  DWORD dwCount = m_dwActiveParticleCount;
  if( dwCount < m_dwTotalParticleCount )
  {
    pParticle = (cParticle*)( m_pParticlePool + dwCount * m_dwParticleDataSize );
    m_dwActiveParticleCount = dwCount + 1;
    return pParticle;
  }

	return NULL;
}

c3dsMaxEmitDataBase::c3dsMaxEmitDataBase()
:m_dwDataType(0), 
 m_dwNumMaterial(0), m_pMaterial(NULL), m_pMaterialAnimator(NULL)
{
}

c3dsMaxEmitDataBase::~c3dsMaxEmitDataBase()
{
  n_delete_array( m_pMaterial );
  n_delete_array( m_pMaterialAnimator );
}

void c3dsMaxEmitDataBase::ImportMaterialFromMemory( const Ptr<Stream>& stream, int iIndex )
{
	// 贴图长度
	int iTextureNameLength = 0;
	stream->Read(&iTextureNameLength, sizeof(int));
	m_pMaterial[iIndex].m_iNameSize = iTextureNameLength;
	// 贴图名称
	ReadString(stream, m_pMaterial[iIndex].m_pszTextName, iTextureNameLength);
  
	// 材质
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.r = m_pMaterial[iIndex].m_D3DMaterial.Ambient.r = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.g = m_pMaterial[iIndex].m_D3DMaterial.Ambient.g = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.b = m_pMaterial[iIndex].m_D3DMaterial.Ambient.b = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.a = m_pMaterial[iIndex].m_D3DMaterial.Ambient.a = 1.0f;

	// Set the RGBA for diffuse reflection.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.r, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.g, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.b, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.a, sizeof(float));

	// Set the color and sharpness of specular highlights.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Power,      sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.r, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.g, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.b, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.a, sizeof(float));

	// Set the RGBA for emissive color.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.r, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.g, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.b, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.a, sizeof(float));

	DWORD dwAlphaBlendType;
	stream->Read(&dwAlphaBlendType, sizeof(DWORD));
	m_pMaterial[iIndex].SetAlphaBlendType( dwAlphaBlendType );

	DWORD dwTextureAniRows;
	stream->Read(&dwTextureAniRows, sizeof(DWORD));
	m_pMaterial[iIndex].SetTextureAniRows( dwTextureAniRows );

	DWORD dwTextureAniCols;
	stream->Read(&dwTextureAniCols, sizeof(DWORD));
	m_pMaterial[iIndex].SetTextureAniCols( dwTextureAniCols );

	DWORD dwTextureAniRepeat;
	stream->Read(&dwTextureAniRepeat, sizeof(DWORD));
	m_pMaterial[iIndex].SetTextureAniRepeat( dwTextureAniRepeat );

	ImportMaterialAnimationFromMemory( stream, iIndex );
}

void c3dsMaxEmitDataBase::ImportMaterialAnimationFromMemory( const Ptr<Stream>& stream, int iIndex )
{
	n_assert(stream.isvalid());
	MemoryMark mm(MemStack);

	DWORD dwKey;
	DWORD dwType;
	DWORD dwTotalKeys;
	//sMaterialAnimationKey matAniKey;
	D3DXVECTOR2 vPos;
	D3DXVECTOR2 vScale;
	float* pTimeTmp = NULL;
	D3DXVECTOR2* pVecTmp = NULL;
	float* pFloatTemp = NULL;
	// extended material animation diffuse color
	D3DXCOLOR* pColorTemp = NULL;

	do
	{
		stream->Read(&dwType, sizeof(DWORD));
    
		switch( dwType )
		{
		case 1: // offset
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pVecTmp = new(MemStack) D3DXVECTOR2[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pVecTmp, sizeof(D3DXVECTOR2)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddPosKey( pTimeTmp[dwKey], pVecTmp[dwKey] );
			}
		break;
		case 2: // tiling
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pVecTmp = new(MemStack) D3DXVECTOR2[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pVecTmp, sizeof(D3DXVECTOR2)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddScaleKey( pTimeTmp[dwKey], pVecTmp[dwKey] );
			}
		break;
		case 3: // wangle
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pFloatTemp = new(MemStack) float[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pFloatTemp, sizeof(float)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddRotateZKey( pTimeTmp[dwKey], pFloatTemp[dwKey] );
			}
		break;
			// extended material animation diffuse color
		case 4: // diffuse & opacity
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pColorTemp = new(MemStack)D3DXCOLOR[dwTotalKeys];
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddDiffuseOpacityKey( pTimeTmp[dwKey], pColorTemp[dwKey] );
			}
		break;
		// extended material animation texture address
		case 5:
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			m_pMaterialAnimator[iIndex].SetAddressFlag( dwTotalKeys );
			break;
		}

	} while( dwType != 0 );
}

cMaterial* c3dsMaxEmitDataBase::GetMaterial( DWORD dwIndex )
{
  if( m_pMaterial && dwIndex < m_dwNumMaterial )
  {
    return &m_pMaterial[dwIndex];
  }

  return NULL;
}

cMaterialAnimator* c3dsMaxEmitDataBase::GetMaterialAnimator( DWORD dwIndex )
{
  if( m_pMaterialAnimator && dwIndex < m_dwNumMaterial )
  {
    return &m_pMaterialAnimator[dwIndex];
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
sPCloudInfo::sPCloudInfo()
:m_dwFormation(0), m_fMoveSpeed(0.0f), m_fMoveSpeedVariation(0.0f), m_dwMoveType(0),
 m_dwBirthMethod(0), m_dwBirthRate(0), m_dwBirthTotal(0), m_dwEmitStart(0),
 m_dwEmitStop(0), m_dwLife(0), m_dwLifeVariation(0), m_fSize(0.0f), m_fSizeVariation(0.0f),
 m_dwGrowTime(0), m_dwFadeTime(0), m_fEmitWidth(0.0f), m_fEmitHeight(0.0f),
 m_fEmitDepth(0.0f), m_dwSpinTime(0), m_fSpinVariation(0.0f), m_fSpinPhaseDeg(0.0f),
 m_fSpinPhaseDegVariation(0.0f)
{
}



////////////////////////////////////////////////////////////////////////////////////////////////
c3dsMaxEmitDataCloud::c3dsMaxEmitDataCloud()
{
  m_dwDataType = 1;
}

void c3dsMaxEmitDataCloud::ImportFromMemory( const Ptr<Stream>& stream )
{
	// data
	stream->Read(&m_sPCloudInfo, sizeof(sPCloudInfo));

	// link
	int iLength;
	stream->Read(&iLength, sizeof(int));
	ReadString(stream, m_pLinkName, iLength);

	// material
	stream->Read(&m_dwNumMaterial, sizeof(DWORD));
	
	n_delete_array( m_pMaterial );
	n_delete_array( m_pMaterialAnimator );
	if( m_dwNumMaterial > 0 )
	{
		m_pMaterial = n_new_array(cMaterial, m_dwNumMaterial);
		m_pMaterialAnimator = n_new_array(cMaterialAnimator, m_dwNumMaterial);

		for( unsigned int i = 0; i < m_dwNumMaterial; i++ )
		{
			ImportMaterialFromMemory( stream, i );
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh particle superspray
sSuperSprayInfo::sSuperSprayInfo()
:m_fOffAxisDeg(0.0f), m_fOffAxisSpreadDeg(0.0f),
 m_fOffPlaneDeg(0.0f), m_fOffPlaneSpreadDeg(0.0f),
 m_fMoveSpeed(0.0f), m_fMoveSpeedVariation(0.0f),
 m_dwBirthMethod(0), m_dwBirthRate(0), m_dwBirthTotal(0), m_dwEmitStart(0),
 m_dwEmitStop(0), m_dwLife(0), m_dwLifeVariation(0), m_fSize(0.0f), m_fSizeVariation(0.0f),
 m_dwGrowTime(0), m_dwFadeTime(0), m_fEmitWidth(0.0f),
 m_dwSpinTime(0), m_fSpinVariation(0.0f), m_fSpinPhaseDeg(0.0f),
 m_fSpinPhaseDegVariation(0.0f)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh particle superspray
c3dsMaxEmitDataSuperSpray::c3dsMaxEmitDataSuperSpray()
{
  m_dwDataType = 2;
}

void c3dsMaxEmitDataSuperSpray::ImportFromMemory( const Ptr<Stream>& stream )
{
	// data
	stream->Read(&m_sSuperSprayInfo, sizeof(sSuperSprayInfo));
	
	// link
	int iLength;
	stream->Read(&iLength, sizeof(int));

	if( iLength > 0 )
	{
		ReadString(stream, m_pLinkName, iLength);
	}

	// material
	stream->Read(&m_dwNumMaterial, sizeof(DWORD));

	n_delete_array( m_pMaterial );
	n_delete_array( m_pMaterialAnimator );

	if( m_dwNumMaterial > 0 )
	{
		m_pMaterial = n_new_array(cMaterial, m_dwNumMaterial);
		m_pMaterialAnimator = n_new_array(cMaterialAnimator, m_dwNumMaterial);

		for( unsigned int i = 0; i < m_dwNumMaterial; i++ )
		{
			ImportMaterialFromMemory( stream, i );
		}
	}
}

//-----------------------------------------------------------------------------
// Name: CreateSpecialMaterialAnimator
// Desc: 
//-----------------------------------------------------------------------------
void c3dsMaxEmitDataSuperSpray::CreateSpecialMaterialAnimator( void )
{
	if( m_dwNumMaterial == 0 )
	{
		m_dwNumMaterial = 1;
		m_pMaterialAnimator = n_new_array(cMaterialAnimator, m_dwNumMaterial);
	}
}
//
////-----------------------------------------------------------------------------
//// Name: cParticleEffectBase
//// Desc: 
////-----------------------------------------------------------------------------
//cParticleEffectBase::cParticleEffectBase( cParticlePool* pParticlePool )
//:m_pParticlePool(pParticlePool), m_pFirstUsedParticle(NULL), m_pLastUsedParticle(NULL),
// m_pLinkName(NULL), m_vCurPosition( 0.0f, 0.0f, 0.0f ), m_pRender(NULL)
//{
//}
//
//
////-----------------------------------------------------------------------------
//// Name: ~cParticleEffectBase
//// Desc: 
////-----------------------------------------------------------------------------
//cParticleEffectBase::~cParticleEffectBase()
//{
//  // 070215 cyhsieh Free Particle
//  cParticle* pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNext = pParticle->m_pNextParticle;
//    FreeParticle( pParticle );
//
//    pParticle = pNext;
//  }
//
//  n_delete_array( m_pLinkName );
//  SAFE_RELEASE( m_pRender );
//}
//
//
////-----------------------------------------------------------------------------
//// Name: AddParticle
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleEffectBase::AddParticle( cParticle* pParticle )
//{
//  cParticle* pLast = m_pLastUsedParticle;
//	if( pLast )
//	{
//		pLast->m_pNextParticle = pParticle;
//		m_pLastUsedParticle = pParticle;
//		pParticle->m_pPrevParticle = pLast;
//		pParticle->m_pNextParticle = NULL;
//	}
//	else
//	{
//		m_pFirstUsedParticle = pParticle;
//		m_pLastUsedParticle = pParticle;
//		pParticle->m_pPrevParticle = NULL;
//		pParticle->m_pNextParticle = NULL;
//	}
//}
//
//
////-----------------------------------------------------------------------------
//// Name: AddParticleFirst
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleEffectBase::AddParticleFirst( cParticle* pParticle )
//{
//  cParticle* pFirst = m_pFirstUsedParticle;
//	if( pFirst )
//	{
//    pFirst->m_pPrevParticle = pParticle;
//    m_pFirstUsedParticle = pParticle;
//    pParticle->m_pPrevParticle = NULL;
//    pParticle->m_pNextParticle = pFirst;
//	}
//	else
//	{
//		m_pFirstUsedParticle = pParticle;
//		m_pLastUsedParticle = pParticle;
//		pParticle->m_pPrevParticle = NULL;
//		pParticle->m_pNextParticle = NULL;
//	}
//}
//
//
////-----------------------------------------------------------------------------
//// Name: FreeParticle
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleEffectBase::FreeParticle( cParticle* pParticle )
//{
//  cParticle* pPrev = pParticle->m_pPrevParticle;
//  cParticle* pNext = pParticle->m_pNextParticle;
//		
//	if( m_pFirstUsedParticle == pParticle ) m_pFirstUsedParticle = pNext;
//	if( m_pLastUsedParticle == pParticle ) m_pLastUsedParticle = pPrev;
//	
//  if( pPrev ) pPrev->m_pNextParticle = pNext;
//	if( pNext ) pNext->m_pPrevParticle = pPrev;
//	
//	
//	cParticle* pLast = m_pParticlePool->m_pLastFreeParticle;
//	if( pLast )
//	{
//		pLast->m_pNextParticle = pParticle;
//		m_pParticlePool->m_pLastFreeParticle = pParticle;
//		pParticle->m_pPrevParticle = pLast;
//		pParticle->m_pNextParticle = NULL;
//	}
//	else
//	{
//		m_pParticlePool->m_pFirstFreeParticle = pParticle;
//		m_pParticlePool->m_pLastFreeParticle = pParticle;
//		pParticle->m_pPrevParticle = NULL;
//		pParticle->m_pNextParticle = NULL;
//	}
//}
//
////-----------------------------------------------------------------------------
//// Name: Draw
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleEffectBase::Draw( LPDIRECT3DDEVICE9 pD3DDevice )
//{
//  if( m_pRender == NULL || m_pRender->m_dwVertexSize < 3 )
//  {
//    return;
//  }
//
//  m_pRender->UpdateRenderBuffer();
//  m_pRender->Draw( pD3DDevice );
//  
//  m_pRender->bEnabled = false;
//}
//
////-----------------------------------------------------------------------------
//// Name: SetLinkName
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleEffectBase::SetLinkName( char* pName )
//{
//  n_delete_array( m_pLinkName );
//
//  if( pName )
//  {
//    int iLength = strlen( pName );
//    if( iLength > 0 )
//    {
//      m_pLinkName = n_new(char, iLength + 1);
//      strcpy( m_pLinkName, pName );
//    }
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: GetRenderAlphaBlendType
//// Desc: 
////-----------------------------------------------------------------------------
//DWORD cParticleEffectBase::GetRenderAlphaBlendType( void )
//{
//  if( m_pRender )
//  {
//    return m_pRender->GetMaterialAlphaBlendType();
//  }
//  return 0;
//}
//
////-----------------------------------------------------------------------------
//// Name: ComputeFogAlphaFactor
//// Desc: 
////-----------------------------------------------------------------------------
//float cParticleEffectBase::ComputeFogAlphaFactor( void )
//{
//  float fFogAlphaFactor = 0.0f;
//  
//  if( m_pRender && g_mFogManage )
//  {
//    float fFogStart = g_mFogManage->GetFogStart();
//    float fFogEnd = g_mFogManage->GetFogEnd();
//
//    D3DXVECTOR3 vToCamera = c3dsMaxParticleManager::GetCameraPos() - m_vCurPosition;
//    float fDistanceToCamera = D3DXVec3Length( &vToCamera );
//
//    if( fDistanceToCamera <= fFogStart )
//    {
//      fFogAlphaFactor = 1.0f;
//    }
//    else if( fDistanceToCamera >= fFogEnd )
//    {
//      fFogAlphaFactor = 0.0f;
//    }
//    else
//    {
//      fFogAlphaFactor = ( fFogEnd - fDistanceToCamera ) / ( fFogEnd - fFogStart );
//    }
//  }
//
//  return fFogAlphaFactor;
//}
//
//
////-----------------------------------------------------------------------------
//// Name: cParticleCloud
//// Desc: 
////-----------------------------------------------------------------------------
//cParticleCloud::cParticleCloud( cParticlePool* pParticlePool, c3dsMaxEmitDataCloud* pEmitDataCloud )
//:cParticleEffectBase( pParticlePool ),
// m_fTimeSinceLastEmit(0.0f), m_fCurAnimTime(0.0), m_bEmitParticles(true),
// m_vEmitDirRight(1.0f, 0.0f, 0.0f), m_vEmitDirUp(0.0f, 1.0f, 0.0f), m_vEmitDirForward(0.0f, 0.0f, 1.0f),
// m_vEmitForceDir(0.0f, 0.0f, 0.0f), m_vCenterOffset(0.0f, 0.0f, 0.0f)
//{
//  if( pEmitDataCloud )
//  {
//    m_sPCloudInfo = pEmitDataCloud->GetPCloudInfo();
//    SetLinkName( pEmitDataCloud->GetLinkName() );
//
//    DWORD dwRenderAlphaBlendType = 0;
//    c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
//    m_pRender = (c3dsMaxParticleRender*)pRender->Create();
//    if( m_pRender )
//    {
//      m_pRender->iMaterialSize = pEmitDataCloud->GetMaterialSize();
//      if( m_pRender->iMaterialSize > 0 )
//      {
//        cMaterial* pMaterial;
//        cMaterialAnimator* pMaterialAnimator;
//        m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
//        for( int i = 0; i < m_pRender->iMaterialSize; i++ )
//        {
//          pMaterial = pEmitDataCloud->GetMaterial( i );
//          if( pMaterial )
//          {
//            m_pRender->m_pMaterial[i].m_iNameSize = pMaterial->m_iNameSize;
//            if( pMaterial->m_iNameSize > 0 )
//            {
//              m_pRender->m_pMaterial[i].m_pszTextName = _NEW char[pMaterial->m_iNameSize + 1];
//              if( m_pRender->m_pMaterial[i].m_pszTextName )
//              {
//                strcpy( m_pRender->m_pMaterial[i].m_pszTextName, pMaterial->m_pszTextName );
//              }
//            }
//            m_pRender->m_pMaterial[i].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
//            m_pRender->m_pMaterial[i].SetTextureAniRows( pMaterial->GetTextureAniRows() );
//            m_pRender->m_pMaterial[i].SetTextureAniCols( pMaterial->GetTextureAniCols() );
//            m_pRender->m_pMaterial[i].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );
//
//            if( i == 0 )
//            {
//              dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
//            }
//          }
//
//          pMaterialAnimator = pEmitDataCloud->GetMaterialAnimator( i );
//          if( pMaterialAnimator )
//          {
//            m_pRender->m_pMaterial[i].SetMaterialAnimator( pMaterialAnimator );
//
//            // 070212 cyhsieh extended material animation texture address
//            if( m_pRender->m_pMaterial[i].GetTextureTransformMatrix() )
//            {
//              m_pRender->m_pMaterial[i].GetAnimatorAddress( m_pRender->m_dwAddressU, m_pRender->m_dwAddressV );
//            }
//          }
//
//        }
//      }
//
//      switch( dwRenderAlphaBlendType )
//      {
//      case 0:   // Opaque
//        m_pRender->m_bAlphaBlendenable = false;
//        m_pRender->m_bZWriteenable = true;
//        break;
//      case 1:   // Additive
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//        m_pRender->m_dwDestBlend = D3DBLEND_ONE;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      case 2:   // Alpha
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//        m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      case 3:   // Modulate
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//        m_pRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      case 4:   // Modulate Inv
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//        m_pRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      }
//
//      int iPredictVertexSize = m_sPCloudInfo.m_dwBirthRate * ( m_sPCloudInfo.m_dwLife / 160 ) * 4;
//      m_pRender->m_dwRenderMeshSize = ( iPredictVertexSize + 15 ) & ~0xF;  // 16计
//      m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[m_pRender->m_dwRenderMeshSize];
//    }
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleCloud::FrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  float fFogAlphaFactor = ComputeFogAlphaFactor();
//  if( fFogAlphaFactor <= 0.0f )
//  {
//    return;
//  }
//
//  // 070213 cyhsieh Particle Emit Time
//  m_bEmitParticles = ( ( m_fCurAnimTime >= m_sPCloudInfo.m_dwEmitStart ) &&
//                       ( m_fCurAnimTime <= m_sPCloudInfo.m_dwEmitStop ) );
// 
//  if( m_bEmitParticles )
//  {
//    m_fTimeSinceLastEmit += fElapsedTime;
//    if( m_fTimeSinceLastEmit > 160.0f )
//    {
//      for( unsigned int i = 0; i < m_sPCloudInfo.m_dwBirthRate; i++ )
//      {
//        cParticle* pParticle = m_pParticlePool->NewParticle();
//        if( pParticle )
//        {
//          if( m_sPCloudInfo.m_dwMoveType == 2 )
//          {
//            pParticle->m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//          }
//          else
//          {
//            pParticle->m_vPos = m_vCurPosition;
//          }
//          
//          pParticle->m_vPos += ( ( (float)rand() / RAND_MAX ) - 0.5f ) * m_vEmitDirRight * m_sPCloudInfo.m_fEmitWidth;
//          pParticle->m_vPos += ( ( (float)rand() / RAND_MAX ) - 0.5f ) * m_vEmitDirUp * m_sPCloudInfo.m_fEmitHeight;
//          pParticle->m_vPos += ( ( (float)rand() / RAND_MAX ) - 0.5f ) * m_vEmitDirForward * m_sPCloudInfo.m_fEmitDepth;
//          
//          float fRandomLifeTime = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//          float fRandomSize = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//          
//          // 070731 cyhsieh particle initial rotation
//          if( m_sPCloudInfo.m_fSpinPhaseDeg == 0 )
//          {
//            pParticle->m_fAngle = 0.0f;
//          }
//          else
//          {
//            float fRandom = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//            pParticle->m_fAngle = m_sPCloudInfo.m_fSpinPhaseDeg * ( 1.0f + fRandom * m_sPCloudInfo.m_fSpinPhaseDegVariation );
//          }
//          
//          pParticle->m_fSize = m_sPCloudInfo.m_fSize * ( 1.0f + m_sPCloudInfo.m_fSizeVariation * fRandomSize );
//          pParticle->m_fTotalLifeTime = (float)m_sPCloudInfo.m_dwLife + m_sPCloudInfo.m_dwLifeVariation * fRandomLifeTime;
//          pParticle->m_fLifeTime = pParticle->m_fTotalLifeTime;
//          pParticle->m_vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//          pParticle->m_dwTextureAniIndex = 0;
//          pParticle->m_fTextureAniTime = 0.0f;
//          AddParticle( pParticle );
//        }
//      }
//      
//      m_fTimeSinceLastEmit = 0.0f;
//    }
//  }
//
//  float fAngleChangedPS;
//  if( m_sPCloudInfo.m_dwSpinTime == 0 )
//  {
//    fAngleChangedPS = 0.0f;
//  }
//  else
//  {
//    fAngleChangedPS = ( 2.0f * D3DX_PI ) / m_sPCloudInfo.m_dwSpinTime;
//  }
//  
//  cParticle* pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNext = pParticle->m_pNextParticle;
//
//    if( ( pParticle->m_fLifeTime -= fElapsedTime ) >= 0.0f )
//    {
//      pParticle->m_vPos += pParticle->m_vDir * fElapsedTime;
//      pParticle->m_fAngle += fAngleChangedPS * fElapsedTime;
//      
//      if( m_sPCloudInfo.m_dwLife - pParticle->m_fLifeTime < m_sPCloudInfo.m_dwGrowTime )
//      {
//        pParticle->m_fSize = ( ( ( m_sPCloudInfo.m_dwLife - pParticle->m_fLifeTime ) / m_sPCloudInfo.m_dwGrowTime ) * 0.9f + 0.1f ) * m_sPCloudInfo.m_fSize;
//      }
//      if( pParticle->m_fLifeTime < m_sPCloudInfo.m_dwFadeTime )
//      {
//        pParticle->m_fSize = ( ( pParticle->m_fLifeTime / m_sPCloudInfo.m_dwFadeTime ) * 0.9f + 0.1f ) * m_sPCloudInfo.m_fSize;
//      }
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNext;
//  }
//
//  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
//  if( pRenderMesh == NULL )
//  {
//    return;
//  }
//
//  cMaterial* pMaterial = NULL;
//  if( m_pRender->iMaterialSize > 0 )
//  {
//    pMaterial = &m_pRender->m_pMaterial[0];
//  }
//
//  const D3DXMATRIXA16& matView = c3dsMaxParticleManager::GetCameraViewMatrix();
//
//  D3DXVECTOR3 vRight = D3DXVECTOR3( matView._11, matView._21, matView._31 );
//  D3DXVECTOR3 vUp = D3DXVECTOR3( matView._12, matView._22, matView._32 );
//  D3DXVECTOR3 vDir = D3DXVECTOR3( matView._13, matView._23, matView._33 );
//  D3DXVECTOR3 vA = -vRight - vUp;
//	D3DXVECTOR3 vB = -vRight + vUp;
//	D3DXVECTOR3 vC =  vRight - vUp;
//	D3DXVECTOR3 vD =  vRight + vUp;
//
//  DWORD dwVertexSize = 0;
//  pParticle = m_pFirstUsedParticle;
//  DWORD dwTotalCount = 0;
//  while( pParticle )
//  {
//    dwTotalCount++;
//    if( dwTotalCount * 4 > m_pRender->m_dwRenderMeshSize )
//    {
//      break;
//    }
//
//    D3DXVECTOR3 vPos = pParticle->m_vPos;
//    if( m_sPCloudInfo.m_dwMoveType == 2 )
//    {
//      D3DXVec3TransformCoord( &vPos, &vPos, &m_matRotate );
//      vPos += m_vCurPosition;
//    }
//
//    if( pParticle->m_fAngle != 0 )
//    {
//      D3DXMATRIXA16 matRotate;
//      D3DXMatrixRotationAxis( &matRotate, &vDir, pParticle->m_fAngle );
//      D3DXVECTOR3 vRotateRight, vRotateUp;
//      D3DXVec3TransformNormal( &vRotateRight, &vRight, &matRotate );
//      D3DXVec3TransformNormal( &vRotateUp, &vUp, &matRotate );
//      D3DXVECTOR3 vRotateA = -vRotateRight - vRotateUp;
//	    D3DXVECTOR3 vRotateB = -vRotateRight + vRotateUp;
//	    D3DXVECTOR3 vRotateC =  vRotateRight - vRotateUp;
//	    D3DXVECTOR3 vRotateD =  vRotateRight + vRotateUp;
//      pRenderMesh[dwVertexSize + 0].p = vPos + vRotateA * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 1].p = vPos + vRotateB * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 2].p = vPos + vRotateC * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 3].p = vPos + vRotateD * pParticle->m_fSize;
//    }
//    else
//    {
//      pRenderMesh[dwVertexSize + 0].p = vPos + vA * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 1].p = vPos + vB * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 2].p = vPos + vC * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 3].p = vPos + vD * pParticle->m_fSize;
//    }
//
//    D3DCOLOR color = 0xFFFFFF | ( ( (DWORD)( fFogAlphaFactor * 255.0f ) ) << 24 );
//    if( pMaterial )
//    {
//      pMaterial->SetAniTime( pParticle->m_fTotalLifeTime - pParticle->m_fLifeTime );
//      const D3DXCOLOR* pColor = pMaterial->GetDiffuseOpacity();
//      if( pColor )
//      {
//        color = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a * fFogAlphaFactor );
//      }
//    }
//    pRenderMesh[dwVertexSize + 0].color = color;
//    pRenderMesh[dwVertexSize + 1].color = color;
//    pRenderMesh[dwVertexSize + 2].color = color;
//    pRenderMesh[dwVertexSize + 3].color = color;
//
//    // 070205 cyhsieh texture animation row & col
//    pParticle->m_fTextureAniTime += fElapsedTime;
//    if( pParticle->m_fTextureAniTime > 160.0f )
//    {
//      pParticle->m_fTextureAniTime -= 160.0f;
//      DWORD dwRepeat = 0;
//      if( pMaterial && ( dwRepeat = pMaterial->GetTextureAniRepeat() ) > 0 )
//      {
//        DWORD dwTextureAniRows = pMaterial->GetTextureAniRows();
//        DWORD dwTextureAniCols = pMaterial->GetTextureAniCols();
//        DWORD dwRowCols = dwTextureAniRows * dwTextureAniCols;
//
//        DWORD dwIndex = pParticle->m_dwTextureAniIndex % ( dwRowCols );
//        float fUDiff = 1.0f / dwTextureAniRows;
//        float fVDiff = 1.0f / dwTextureAniCols;
//
//        float fStartU = ( dwIndex % dwTextureAniRows ) * fUDiff;
//        float fStartV = ( dwIndex % dwTextureAniCols ) * fVDiff;
//
//        pRenderMesh[dwVertexSize + 0].tu = fStartU;
//        pRenderMesh[dwVertexSize + 0].tv = fStartV + fVDiff;
//        pRenderMesh[dwVertexSize + 1].tu = fStartU;
//        pRenderMesh[dwVertexSize + 1].tv = fStartV;
//        pRenderMesh[dwVertexSize + 2].tu = fStartU + fUDiff;
//        pRenderMesh[dwVertexSize + 2].tv = fStartV + fVDiff;
//        pRenderMesh[dwVertexSize + 3].tu = fStartU + fUDiff;
//        pRenderMesh[dwVertexSize + 3].tv = fStartV;
//
//        if( dwRepeat == 999 || pParticle->m_dwTextureAniIndex < dwRepeat * dwRowCols - 1 )
//        {
//          pParticle->m_dwTextureAniIndex++;
//        }
//      }
//      else
//      {
//        pRenderMesh[dwVertexSize + 0].tu = 0.0f;
//        pRenderMesh[dwVertexSize + 0].tv = 1.0f;
//        pRenderMesh[dwVertexSize + 1].tu = 0.0f;
//        pRenderMesh[dwVertexSize + 1].tv = 0.0f;
//        pRenderMesh[dwVertexSize + 2].tu = 1.0f;
//        pRenderMesh[dwVertexSize + 2].tv = 1.0f;
//        pRenderMesh[dwVertexSize + 3].tu = 1.0f;
//        pRenderMesh[dwVertexSize + 3].tv = 0.0f;
//      }
//    }
//
//    dwVertexSize += 4;
//
//    pParticle = pParticle->m_pNextParticle;
//  }
//
//  m_pRender->m_dwVertexSize = dwVertexSize;
//  m_pRender->bEnabled = true;
//}
//
////-----------------------------------------------------------------------------
//// Name: DisableFrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleCloud::DisableFrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  cParticle* pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNext = pParticle->m_pNextParticle;
//
//    if( ( pParticle->m_fLifeTime -= fElapsedTime ) >= 0.0f )
//    {
//      pParticle->m_vPos += pParticle->m_vDir * fElapsedTime;
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNext;
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: ApplyTransformMatrix
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleCloud::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
//{
//  D3DXVECTOR3 vOffset;
//  D3DXVec3TransformNormal( &vOffset, &m_vCenterOffset, pTransform );
//  m_vCurPosition.x = pTransform->_41;
//  m_vCurPosition.y = pTransform->_42;
//  m_vCurPosition.z = pTransform->_43;
//  
//  m_vCurPosition += vOffset;
//
//  m_matRotate = *pTransform;
//  m_matRotate._41 = 0.0f;
//  m_matRotate._42 = 0.0f;
//  m_matRotate._43 = 0.0f;
//
//  D3DXVECTOR3 vRight( 1.0f, 0.0f, 0.0f );
//  D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
//  D3DXVECTOR3 vForward( 0.0f, 0.0f, 1.0f );
//
//  if( m_sPCloudInfo.m_dwMoveType != 2 )
//  {
//    D3DXVec3TransformNormal( &m_vEmitDirRight, &vRight, pTransform );
//    D3DXVec3TransformNormal( &m_vEmitDirUp, &vUp, pTransform );
//    D3DXVec3TransformNormal( &m_vEmitDirForward, &vForward, pTransform );
//  }
//
//  D3DXVec3TransformNormal( &m_vEmitForceDir, &-vForward, pTransform );
//}
//
////-----------------------------------------------------------------------------
//// Name: ApplyBoundingBox
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleCloud::ApplyBoundingBox( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax )
//{
//  m_vCenterOffset = ( vMin + vMax ) * 0.5f;
//
//  D3DXVECTOR3 vDiff = vMax - vMin;
//  m_sPCloudInfo.m_fEmitWidth = vDiff.x;
//  m_sPCloudInfo.m_fEmitHeight = vDiff.y;
//  m_sPCloudInfo.m_fEmitDepth = vDiff.z;
//}
//
//
////-----------------------------------------------------------------------------
//// Name: cParticleSuperSpray
//// Desc: 
////-----------------------------------------------------------------------------
//// 070205 cyhsieh particle superspray
//cParticleSuperSpray::cParticleSuperSpray( cParticlePool* pParticlePool, c3dsMaxEmitDataSuperSpray* pEmitDataSuperSpray )
//:cParticleEffectBase( pParticlePool ),
// m_fTimeSinceLastEmit(0.0f), m_fCurAnimTime(0.0f), m_bEmitParticles(true),
// m_vEmitDirRight(1.0f, 0.0f, 0.0f), m_vEmitDirForward(0.0f, 0.0f, 1.0f), m_vEmitForceDir(0.0f, 0.0f, 0.0f)
//{
//  if( pEmitDataSuperSpray )
//  {
//    m_sSuperSprayInfo = pEmitDataSuperSpray->GetSuperSprayInfo();
//    SetLinkName( pEmitDataSuperSpray->GetLinkName() );
//
//    DWORD dwRenderAlphaBlendType = 0;
//    c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
//    m_pRender = (c3dsMaxParticleRender*)pRender->Create();
//    if( m_pRender )
//    {
//      m_pRender->iMaterialSize = pEmitDataSuperSpray->GetMaterialSize();
//      if( m_pRender->iMaterialSize > 0 )
//      {
//        cMaterial* pMaterial;
//        cMaterialAnimator*  pMaterialAnimator;
//        m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
//        for( int i = 0; i < m_pRender->iMaterialSize; i++ )
//        {
//          pMaterial = pEmitDataSuperSpray->GetMaterial( i );
//          if( pMaterial )
//          {
//            m_pRender->m_pMaterial[i].m_iNameSize = pMaterial->m_iNameSize;
//            if( pMaterial->m_iNameSize > 0 )
//            {
//              m_pRender->m_pMaterial[i].m_pszTextName = _NEW char[pMaterial->m_iNameSize + 1];
//              if( m_pRender->m_pMaterial[i].m_pszTextName )
//              {
//                strcpy( m_pRender->m_pMaterial[i].m_pszTextName, pMaterial->m_pszTextName );
//              }
//            }
//            m_pRender->m_pMaterial[i].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
//            m_pRender->m_pMaterial[i].SetTextureAniRows( pMaterial->GetTextureAniRows() );
//            m_pRender->m_pMaterial[i].SetTextureAniCols( pMaterial->GetTextureAniCols() );
//            m_pRender->m_pMaterial[i].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );
//
//            if( i == 0 )
//            {
//              dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
//            }
//          }
//          pMaterialAnimator = pEmitDataSuperSpray->GetMaterialAnimator( i );
//          if( pMaterialAnimator )
//          {
//            m_pRender->m_pMaterial[i].SetMaterialAnimator( pMaterialAnimator );
//
//            // 070212 cyhsieh extended material animation texture address
//            if( m_pRender->m_pMaterial[i].GetTextureTransformMatrix() )
//            {
//              m_pRender->m_pMaterial[i].GetAnimatorAddress( m_pRender->m_dwAddressU, m_pRender->m_dwAddressV );
//            }
//          }
//        }
//      }
//
//      switch( dwRenderAlphaBlendType )
//      {
//      case 0:   // Opaque
//        m_pRender->m_bAlphaBlendenable = false;
//        m_pRender->m_bZWriteenable = true;
//        break;
//      case 1:   // Additive
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//        m_pRender->m_dwDestBlend = D3DBLEND_ONE;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      case 2:   // Alpha
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//        m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      case 3:   // Modulate
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//        m_pRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      case 4:   // Modulate Inv
//        m_pRender->m_bAlphaBlendenable = true;
//        m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//        m_pRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
//        m_pRender->m_bZWriteenable = false;
//        break;
//      }
//
//      int iPredictVertexSize = m_sSuperSprayInfo.m_dwBirthRate * ( m_sSuperSprayInfo.m_dwLife / 160 ) * 4;
//      m_pRender->m_dwRenderMeshSize = ( iPredictVertexSize + 15 ) & ~0xF;  // 16计
//      m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[m_pRender->m_dwRenderMeshSize];
//    }
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleSuperSpray::FrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  float fFogAlphaFactor = ComputeFogAlphaFactor();
//  if( fFogAlphaFactor <= 0.0f )
//  {
//    return;
//  }
//
//  // 070213 cyhsieh Particle Emit Time
//  m_bEmitParticles = ( ( m_fCurAnimTime >= m_sSuperSprayInfo.m_dwEmitStart ) &&
//                       ( m_fCurAnimTime <= m_sSuperSprayInfo.m_dwEmitStop ) );
//
//  if( m_bEmitParticles )
//  {
//    m_fTimeSinceLastEmit += fElapsedTime;
//    if( m_fTimeSinceLastEmit > 160.0f )
//    {
//      for( unsigned int i = 0; i < m_sSuperSprayInfo.m_dwBirthRate; i++ )
//      {
//        cParticle* pParticle = m_pParticlePool->NewParticle();
//        if( pParticle )
//        {
//          pParticle->m_vPos = m_vCurPosition;
//                    
//          float fRandomLifeTime = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//          float fRandomSize = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//          
//          // 070731 cyhsieh particle initial rotation
//          if( m_sSuperSprayInfo.m_fSpinPhaseDeg == 0 )
//          {
//            pParticle->m_fAngle = 0.0f;
//          }
//          else
//          {
//            float fRandom = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//            pParticle->m_fAngle = m_sSuperSprayInfo.m_fSpinPhaseDeg * ( 1.0f + fRandom * m_sSuperSprayInfo.m_fSpinPhaseDegVariation );
//          }
//          
//          pParticle->m_fSize = m_sSuperSprayInfo.m_fSize * ( 1.0f + m_sSuperSprayInfo.m_fSizeVariation * fRandomSize );
//          pParticle->m_fTotalLifeTime = (float)m_sSuperSprayInfo.m_dwLife + m_sSuperSprayInfo.m_dwLifeVariation * fRandomLifeTime;
//          pParticle->m_fLifeTime = pParticle->m_fTotalLifeTime;
//          pParticle->m_vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//
//          D3DXVECTOR3 vRandomDir;
//          float fRandomV = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//          float fRandomH = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//          float fThetaV = m_sSuperSprayInfo.m_fOffAxisDeg + m_sSuperSprayInfo.m_fOffAxisSpreadDeg * fRandomV;
//          float fThetaH = m_sSuperSprayInfo.m_fOffPlaneDeg + m_sSuperSprayInfo.m_fOffPlaneSpreadDeg * fRandomH;
//          float fSinV = sin( fThetaV );
//          float fCosV = cos( fThetaV );
//          vRandomDir.z = fSinV * sin( fThetaH );
//          vRandomDir.x = fSinV * cos( fThetaH );
//          vRandomDir.y = fCosV;
//          D3DXVec3TransformNormal( &vRandomDir, &vRandomDir, &m_matTransForm );
//          
//          float fRandomSpeed = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
//          fRandomSpeed = m_sSuperSprayInfo.m_fMoveSpeed * ( 1.0f + m_sSuperSprayInfo.m_fMoveSpeedVariation * fRandomSpeed );
//
//          // 070215 cyhsieh SuperSpray Speed
//          pParticle->m_vDir = vRandomDir * fRandomSpeed / 160.0f;
//          pParticle->m_dwTextureAniIndex = 0;
//          pParticle->m_fTextureAniTime = 0.0f;
//          AddParticle( pParticle );
//        }
//      }
//
//      m_fTimeSinceLastEmit = 0.0f;
//    }
//  }
//
//  float fAngleChangedPS;
//  if( m_sSuperSprayInfo.m_dwSpinTime == 0 )
//  {
//    fAngleChangedPS = 0.0f;
//  }
//  else
//  {
//    fAngleChangedPS = ( 2.0f * D3DX_PI ) / m_sSuperSprayInfo.m_dwSpinTime;
//  }
//
//  cParticle* pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNext = pParticle->m_pNextParticle;
//
//    if( ( pParticle->m_fLifeTime -= fElapsedTime ) >= 0.0f )
//    {
//      pParticle->m_vPos += pParticle->m_vDir * fElapsedTime;
//      pParticle->m_fAngle += fAngleChangedPS * fElapsedTime;
//      
//      if( m_sSuperSprayInfo.m_dwLife - pParticle->m_fLifeTime < m_sSuperSprayInfo.m_dwGrowTime )
//      {
//        pParticle->m_fSize = ( ( ( m_sSuperSprayInfo.m_dwLife - pParticle->m_fLifeTime ) / m_sSuperSprayInfo.m_dwGrowTime ) * 0.9f + 0.1f ) * m_sSuperSprayInfo.m_fSize;
//      }
//      if( pParticle->m_fLifeTime < m_sSuperSprayInfo.m_dwFadeTime )
//      {
//        pParticle->m_fSize = ( ( pParticle->m_fLifeTime / m_sSuperSprayInfo.m_dwFadeTime ) * 0.9f + 0.1f ) * m_sSuperSprayInfo.m_fSize;
//      }
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNext;
//  }
//
//  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
//  if( pRenderMesh == NULL )
//  {
//    return;
//  }
//
//  cMaterial* pMaterial = NULL;
//  if( m_pRender->iMaterialSize > 0 )
//  {
//    pMaterial = &m_pRender->m_pMaterial[0];
//  }
//
//  const D3DXMATRIXA16& matView = c3dsMaxParticleManager::GetCameraViewMatrix();
//
//  D3DXVECTOR3 vRight = D3DXVECTOR3( matView._11, matView._21, matView._31 );
//  D3DXVECTOR3 vUp = D3DXVECTOR3( matView._12, matView._22, matView._32 );
//  D3DXVECTOR3 vDir = D3DXVECTOR3( matView._13, matView._23, matView._33 );
//  D3DXVECTOR3 vA = -vRight - vUp;
//	D3DXVECTOR3 vB = -vRight + vUp;
//	D3DXVECTOR3 vC =  vRight - vUp;
//	D3DXVECTOR3 vD =  vRight + vUp;
//
//  DWORD dwVertexSize = 0;
//  pParticle = m_pFirstUsedParticle;
//  DWORD dwTotalCount = 0;
//  while( pParticle )
//  {
//    dwTotalCount++;
//    if( dwTotalCount * 4 > m_pRender->m_dwRenderMeshSize )
//    {
//      break;
//    }
//
//    if( pParticle->m_fAngle != 0 )
//    {
//      D3DXMATRIXA16 matRotate;
//      D3DXMatrixRotationAxis( &matRotate, &vDir, pParticle->m_fAngle );
//      D3DXVECTOR3 vRotateRight, vRotateUp;
//      D3DXVec3TransformNormal( &vRotateRight, &vRight, &matRotate );
//      D3DXVec3TransformNormal( &vRotateUp, &vUp, &matRotate );
//      D3DXVECTOR3 vRotateA = -vRotateRight - vRotateUp;
//	    D3DXVECTOR3 vRotateB = -vRotateRight + vRotateUp;
//	    D3DXVECTOR3 vRotateC =  vRotateRight - vRotateUp;
//	    D3DXVECTOR3 vRotateD =  vRotateRight + vRotateUp;
//      pRenderMesh[dwVertexSize + 0].p = pParticle->m_vPos + vRotateA * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 1].p = pParticle->m_vPos + vRotateB * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 2].p = pParticle->m_vPos + vRotateC * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 3].p = pParticle->m_vPos + vRotateD * pParticle->m_fSize;
//    }
//    else
//    {
//      pRenderMesh[dwVertexSize + 0].p = pParticle->m_vPos + vA * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 1].p = pParticle->m_vPos + vB * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 2].p = pParticle->m_vPos + vC * pParticle->m_fSize;
//      pRenderMesh[dwVertexSize + 3].p = pParticle->m_vPos + vD * pParticle->m_fSize;
//    }
//    
//    D3DCOLOR color = 0xFFFFFF | ( ( (DWORD)( fFogAlphaFactor * 255.0f ) ) << 24 );
//    if( pMaterial )
//    {
//      pMaterial->SetAniTime( pParticle->m_fTotalLifeTime - pParticle->m_fLifeTime );
//      const D3DXCOLOR* pColor = pMaterial->GetDiffuseOpacity();
//      if( pColor )
//      {
//        color = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a * fFogAlphaFactor );
//      }
//    }
//    pRenderMesh[dwVertexSize + 0].color = color;
//    pRenderMesh[dwVertexSize + 1].color = color;
//    pRenderMesh[dwVertexSize + 2].color = color;
//    pRenderMesh[dwVertexSize + 3].color = color;
//
//    // 070205 cyhsieh texture animation row & col
//    pParticle->m_fTextureAniTime += fElapsedTime;
//    if( pParticle->m_fTextureAniTime > 160.0f )
//    {
//      pParticle->m_fTextureAniTime -= 160.0f;
//      DWORD dwRepeat = 0;
//      if( pMaterial && ( dwRepeat = pMaterial->GetTextureAniRepeat() ) > 0 )
//      {
//        DWORD dwTextureAniRows = pMaterial->GetTextureAniRows();
//        DWORD dwTextureAniCols = pMaterial->GetTextureAniCols();
//        DWORD dwRowCols = dwTextureAniRows * dwTextureAniCols;
//
//        DWORD dwIndex = pParticle->m_dwTextureAniIndex % ( dwRowCols );
//        float fUDiff = 1.0f / dwTextureAniRows;
//        float fVDiff = 1.0f / dwTextureAniCols;
//
//        float fStartU = ( dwIndex % dwTextureAniRows ) * fUDiff;
//        float fStartV = ( dwIndex % dwTextureAniCols ) * fVDiff;
//
//        pRenderMesh[dwVertexSize + 0].tu = fStartU;
//        pRenderMesh[dwVertexSize + 0].tv = fStartV + fVDiff;
//        pRenderMesh[dwVertexSize + 1].tu = fStartU;
//        pRenderMesh[dwVertexSize + 1].tv = fStartV;
//        pRenderMesh[dwVertexSize + 2].tu = fStartU + fUDiff;
//        pRenderMesh[dwVertexSize + 2].tv = fStartV + fVDiff;
//        pRenderMesh[dwVertexSize + 3].tu = fStartU + fUDiff;
//        pRenderMesh[dwVertexSize + 3].tv = fStartV;
//
//        if( dwRepeat == 999 || pParticle->m_dwTextureAniIndex < dwRepeat * dwRowCols - 1 )
//        {
//          pParticle->m_dwTextureAniIndex++;
//        }
//      }
//      else
//      {
//        pRenderMesh[dwVertexSize + 0].tu = 0.0f;
//        pRenderMesh[dwVertexSize + 0].tv = 1.0f;
//        pRenderMesh[dwVertexSize + 1].tu = 0.0f;
//        pRenderMesh[dwVertexSize + 1].tv = 0.0f;
//        pRenderMesh[dwVertexSize + 2].tu = 1.0f;
//        pRenderMesh[dwVertexSize + 2].tv = 1.0f;
//        pRenderMesh[dwVertexSize + 3].tu = 1.0f;
//        pRenderMesh[dwVertexSize + 3].tv = 0.0f;
//      }
//    }
//    
//
//    dwVertexSize += 4;
//
//    pParticle = pParticle->m_pNextParticle;
//  }
//
//  m_pRender->m_dwVertexSize = dwVertexSize;
//  m_pRender->bEnabled = true;
//}
//
////-----------------------------------------------------------------------------
//// Name: DisableFrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleSuperSpray::DisableFrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  cParticle* pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNext = pParticle->m_pNextParticle;
//
//    if( ( pParticle->m_fLifeTime -= fElapsedTime ) >= 0.0f )
//    {
//      pParticle->m_vPos += pParticle->m_vDir * fElapsedTime;
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNext;
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleSuperSpray::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
//{
//  m_vCurPosition.x = pTransform->_41;
//  m_vCurPosition.y = pTransform->_42;
//  m_vCurPosition.z = pTransform->_43;
//
//  D3DXVECTOR3 vRight( 1.0f, 0.0f, 0.0f );
//  D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
//  D3DXVECTOR3 vForward( 0.0f, 0.0f, 1.0f );
//  D3DXVec3TransformNormal( &m_vEmitDirRight, &vRight, pTransform );
//  D3DXVec3TransformNormal( &m_vEmitDirForward, &vForward, pTransform );
//  D3DXVec3TransformNormal( &m_vEmitForceDir, &vUp, pTransform );
//
//  // 070214 cyhsieh spray喷射方向
//  m_matTransForm = *pTransform;
//}

//
////-----------------------------------------------------------------------------
//// Name: cParticleRibbon
//// Desc: 
////-----------------------------------------------------------------------------
//cParticleRibbon::cParticleRibbon( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer )
//:cParticleEffectBase( pParticlePool ),
// m_fAddSegmentLength(1.0f), m_fMaxRibbonLength(10.0f), m_fRibbonWidth(0.5f)
//{
//  if( pSubMeshSerializer == NULL )
//  {
//    return;
//  }
//  
//  SetLinkName( pSubMeshSerializer->getSubMeshName() );
//
//  m_fRibbonWidth = pSubMeshSerializer->GetRibbonWidth();
//  m_fMaxRibbonLength = pSubMeshSerializer->GetRibbonLength();
//
//  m_fAddSegmentLength = m_fRibbonWidth;
//
//  c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
//  if( pRender == NULL )
//  {
//    return;
//  }
//
//  m_pRender = (c3dsMaxParticleRender*)pRender->Create();
//  m_pRender->iMaterialSize = pSubMeshSerializer->getSubMeshMaterialSize();
//  if( m_pRender->iMaterialSize > 0 )
//  {
//    cMaterial* pMaterial;
//    m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
//    for( int i = 0; i < m_pRender->iMaterialSize; i++ )
//    {
//      pMaterial = pSubMeshSerializer->getSubMeshMaterial( i );
//      if( pMaterial )
//      {
//        m_pRender->m_pMaterial[i].m_iNameSize = pMaterial->m_iNameSize;
//        if( pMaterial->m_iNameSize > 0 )
//        {
//          m_pRender->m_pMaterial[i].m_pszTextName = _NEW char[pMaterial->m_iNameSize + 1];
//          if( m_pRender->m_pMaterial[i].m_pszTextName )
//          {
//            strcpy( m_pRender->m_pMaterial[i].m_pszTextName, pMaterial->m_pszTextName );
//          }
//        }
//      }
//    }
//  }
//
//  m_pRender->iRenderType = 4;
//  m_pRender->m_dwCullMode = D3DCULL_NONE;
//  m_pRender->m_dwColorArg1 = D3DTA_TEXTURE;
//  m_pRender->m_dwColorOp = D3DTOP_SELECTARG1;
//
//  // !!!To do... set correct rendermeshsize
//  m_pRender->m_dwRenderMeshSize = 128;
//  m_pRender->m_dwVertexSize = 0;
//  m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[pRender->m_dwRenderMeshSize];
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleRibbon::FrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  cParticle* pParticle;
//  
//  float fDiffLength = D3DXVec3Length( &( m_vCurPosition - m_vLastPos ) );
//
//  pParticle = m_pFirstUsedParticle;
//  if( pParticle )
//  {
//    if( ( pParticle->m_fSize + fDiffLength ) > m_fAddSegmentLength )
//    {
//      pParticle = m_pParticlePool->NewParticle();
//      if( pParticle )
//      {
//        pParticle->m_vPos = m_vCurPosition;
//        pParticle->m_vDir = m_vDirUp;
//        pParticle->m_fSize = fDiffLength;
//        pParticle->m_fLifeTime = 1.0f;
//        AddParticleFirst( pParticle );
//      }
//    }
//    else
//    {
//      pParticle->m_vPos = m_vCurPosition;
//      pParticle->m_vDir = m_vDirUp;
//      pParticle->m_fSize += fDiffLength;
//      pParticle->m_fLifeTime = 1.0f;
//    }
//  }
//
//  bool bFade = false;
//  float fOneOverElapsedTime = 0.0f;
//  if( fDiffLength < m_fAddSegmentLength * 0.5f )
//  {
//    bFade = true;
//    fOneOverElapsedTime = fElapsedTime / 4800.0f;
//  }
//
//  bool bErase = false;
//  float fLength = 0.0f;
//  pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNextParticle = pParticle->m_pNextParticle;
//
//    if( !bErase )
//    {
//      fLength += pParticle->m_fSize;
//
//      if( bFade )
//      {
//        if( pParticle->m_fLifeTime > 0.0f )
//        {
//          pParticle->m_fLifeTime -= fOneOverElapsedTime;
//          if( pParticle->m_fLifeTime < 0.0f )
//          {
//            pParticle->m_fLifeTime = 0.0f;
//          }
//        }
//      }
//
//      if( fLength > m_fMaxRibbonLength )
//      {
//        pParticle->m_fSize = fLength - m_fMaxRibbonLength;
//        bErase = true;
//      }
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNextParticle;
//  }
//
//  m_vLastPos = m_vCurPosition;
//
//  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
//  if( pRenderMesh == NULL )
//  {
//    return;
//  }
//
//  DWORD dwVertexSize = 0;
//  float fTU;
//  fLength = 0.0f;
//  pParticle = m_pFirstUsedParticle;
//  DWORD dwTotalCount = 0;
//  while( pParticle )
//  {
//    dwTotalCount++;
//    if( dwTotalCount * 2 > m_pRender->m_dwRenderMeshSize )
//    {
//      break;
//    }
//
//    fTU = 1.0f - fLength / m_fMaxRibbonLength;
//
//    D3DCOLOR color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, pParticle->m_fLifeTime );
//
//    pRenderMesh[dwVertexSize].p = pParticle->m_vPos + pParticle->m_vDir * m_fRibbonWidth;
//    pRenderMesh[dwVertexSize].tu = fTU;
//    pRenderMesh[dwVertexSize].tv = 0.0f;
//    pRenderMesh[dwVertexSize].color = color;
//    dwVertexSize++;
//    pRenderMesh[dwVertexSize].p = pParticle->m_vPos - pParticle->m_vDir * m_fRibbonWidth;
//    pRenderMesh[dwVertexSize].tu = fTU;
//    pRenderMesh[dwVertexSize].tv = 1.0f;
//    pRenderMesh[dwVertexSize].color = color;
//    dwVertexSize++;
//
//    fLength += pParticle->m_fSize;
//
//    pParticle = pParticle->m_pNextParticle;
//  }
//
//  m_pRender->m_dwVertexSize = dwVertexSize;
//  m_pRender->bEnabled = true;
//}
//
////-----------------------------------------------------------------------------
//// Name: DisableFrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleRibbon::DisableFrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  bool bErase = false;
//  float fLength = 0.0f;
//  cParticle* pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNextParticle = pParticle->m_pNextParticle;
//
//    if( !bErase )
//    {
//      fLength += pParticle->m_fSize;
//
//      if( pParticle->m_fLifeTime > 0.0f )
//      {
//        pParticle->m_fLifeTime -= fElapsedTime;
//        if( pParticle->m_fLifeTime < 0.0f )
//        {
//          pParticle->m_fLifeTime = 0.0f;
//        }
//      }
//
//      if( fLength > m_fMaxRibbonLength )
//      {
//        pParticle->m_fSize = fLength - m_fMaxRibbonLength;
//        bErase = true;
//      }
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNextParticle;
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: InitTransformMatrix
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleRibbon::InitTransformMatrix( D3DXMATRIXA16* pTransform )
//{
//  m_vCurPosition.x = pTransform->_41;
//  m_vCurPosition.y = pTransform->_42;
//  m_vCurPosition.z = pTransform->_43;
//  m_vLastPos = m_vCurPosition;
//
//  if( m_pParticlePool )
//  {
//    cParticle* pParticle = m_pParticlePool->NewParticle();
//    if( pParticle )
//    {
//      D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
//      D3DXVec3TransformNormal( &m_vDirUp, &vUp, pTransform );
//
//      pParticle->m_vPos = m_vCurPosition;
//      pParticle->m_vDir = m_vDirUp;
//      pParticle->m_fSize = 0.0f;
//      pParticle->m_fLifeTime = 1.0f;
//
//      AddParticle( pParticle );
//    }
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: ApplyTransformMatrix
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleRibbon::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
//{
//  m_vCurPosition.x = pTransform->_41;
//  m_vCurPosition.y = pTransform->_42;
//  m_vCurPosition.z = pTransform->_43;
//
//  D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
//  D3DXVec3TransformNormal( &m_vDirUp, &vUp, pTransform );
//}
//
//
////-----------------------------------------------------------------------------
//// Name: cParticleWeaponRibbon
//// Desc: 
////-----------------------------------------------------------------------------
////  武器刀光
//cParticleWeaponRibbon::cParticleWeaponRibbon( cParticlePool* pParticlePool, bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos )
//:cParticleEffectBase( pParticlePool ),
// m_vOrigMinPos(vMinPos), m_vOrigMaxPos(vMaxPos), m_vMinPos(vMinPos), m_vMaxPos(vMaxPos), m_bAddSegment(false),
// m_pLerpInfo(NULL)
//{
//  if( bDoLerp )
//  {
//    m_pLerpInfo = _NEW sLerpInfo;
//  }
//
//  c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
//  m_pRender = (c3dsMaxParticleRender*)pRender->Create();
//  if( m_pRender )
//  {
//    m_pRender->iMaterialSize = 1;
//    if( m_pRender->iMaterialSize > 0 )
//    {
//      m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
//      m_pRender->m_pMaterial[0].m_pszTextName = _NEW char[15 + 1];
//      strcpy( m_pRender->m_pMaterial[0].m_pszTextName, "weaponribbon000" );
//    }
//
//    m_pRender->iRenderType = 4;
//    m_pRender->m_dwCullMode = D3DCULL_NONE;
//    
//    m_pRender->m_dwAddressU = D3DTADDRESS_CLAMP;
//    m_pRender->m_dwAddressV = D3DTADDRESS_CLAMP;
//
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
//
//    // !!!To do... set correct rendermeshsize
//    m_pRender->m_dwRenderMeshSize = 36;
//    m_pRender->m_dwVertexSize = 0;
//    m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[pRender->m_dwRenderMeshSize];
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: ~cParticleWeaponRibbon
//// Desc: 
////-----------------------------------------------------------------------------
//cParticleWeaponRibbon::~cParticleWeaponRibbon()
//{
//  SAFE_DELETE( m_pLerpInfo );
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleWeaponRibbon::FrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  cParticle* pParticle;
//
//  float fTotalTime = 0.2f * 4800.0f;
//
//  if( m_bAddSegment )
//  {
//    if( m_pLerpInfo )
//    {
//		// 为了让刀光在frame rate不高时能看起来比较圆滑，在这次和前次拿到的transform作lerp的动作
//		// lerp取样的数量为这次和上次的距离除以0.5
//		// !!!To do...Optimize
//      D3DXVECTOR3     vCurScale;
//      D3DXQUATERNION  quatCurRotation;
//      D3DXVECTOR3     vCurTranslation;
//
//      D3DXMatrixDecompose( &vCurScale, &quatCurRotation, &vCurTranslation, &m_pLerpInfo->m_matCurTransform );
//
//      pParticle = m_pFirstUsedParticle;
//      if( pParticle )
//      {
//        cParticle* pNextParticle = pParticle->m_pNextParticle;
//        float fLength = D3DXVec3Length( &( m_vMaxPos - m_pLerpInfo->m_vLastMaxPos ) );
//        if( pNextParticle && ( fLength >= 0.5f ) )
//        {
//          D3DXMATRIXA16   matLerpTranslation;
//          D3DXMATRIXA16   matLerpRotation;
//          D3DXMATRIXA16   matLerpTransform;
//          D3DXQUATERNION  quatLerpRotation;
//          D3DXVECTOR3     vLerpTranslation;
//
//          int iDivide = (int)( fLength / 0.5f ) + 1;
//          if( iDivide > 12 )
//          {
//            iDivide = 12;
//          }
//          float fLerpStep = 1.0f / iDivide;
//          for( int i = 1; i < iDivide; i++ )
//          {
//            float fLerp = i * fLerpStep;
//            D3DXQuaternionSlerp( &quatLerpRotation, &m_pLerpInfo->m_quatLastRotation, &quatCurRotation, fLerp );
//            D3DXVec3Lerp( &vLerpTranslation, &m_pLerpInfo->m_vLastTranslation, &vCurTranslation, fLerp );
//            D3DXMatrixRotationQuaternion( &matLerpRotation, &quatLerpRotation );
//            D3DXMatrixTranslation( &matLerpTranslation, vLerpTranslation.x, vLerpTranslation.y, vLerpTranslation.z );
//            matLerpTransform = matLerpRotation * matLerpTranslation;
//
//            float fLifeTime = fTotalTime * fLerp + pNextParticle->m_fLifeTime * ( 1.0f - fLerp );
//
//            pParticle = m_pParticlePool->NewParticle();
//            if( pParticle )
//            {
//              pParticle->m_fLifeTime = fLifeTime;
//              D3DXVec3TransformCoord( &pParticle->m_vPos, &m_vOrigMinPos, &matLerpTransform );
//              D3DXVec3TransformCoord( &pParticle->m_vDir, &m_vOrigMaxPos, &matLerpTransform );
//
//              AddParticleFirst( pParticle );
//            }
//          }
//        }
//      }
//      
//      pParticle = m_pParticlePool->NewParticle();
//      if( pParticle )
//      {
//        pParticle->m_fLifeTime = fTotalTime;
//        pParticle->m_vPos = m_vMinPos;
//        pParticle->m_vDir = m_vMaxPos;
//
//        AddParticleFirst( pParticle );
//
//        m_pLerpInfo->m_vLastMaxPos = m_vMaxPos;
//        m_pLerpInfo->m_vLastScale = vCurScale;
//        m_pLerpInfo->m_quatLastRotation = quatCurRotation;
//        m_pLerpInfo->m_vLastTranslation = vCurTranslation;
//      }
//    }
//    else
//    {
//      pParticle = m_pParticlePool->NewParticle();
//      if( pParticle )
//      {
//        pParticle->m_fLifeTime = fTotalTime;
//        pParticle->m_vPos = m_vMinPos;
//        pParticle->m_vDir = m_vMaxPos;
//
//        AddParticleFirst( pParticle );
//      }
//    }
//  } // if( m_bAddSegment )
//  
//  DWORD dwTotalCount = 0;
//  bool bErase = false;
//  DWORD dwMaxCount = m_pRender->m_dwRenderMeshSize >> 1;
//  pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNextParticle = pParticle->m_pNextParticle;
//
//    if( !bErase )
//    {
//      dwTotalCount++;
//
//      if( dwTotalCount > dwMaxCount )
//      {
//        bErase = true;
//        FreeParticle( pParticle );
//      }
//      else
//      {
//        pParticle->m_fLifeTime -= fElapsedTime;
//        if( pParticle->m_fLifeTime < 0.0f )
//        {
//          pParticle->m_fLifeTime = 0.0f;
//          bErase = true;
//          FreeParticle( pParticle );
//        }
//      }
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNextParticle;
//  }
//
//  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
//  if( pRenderMesh == NULL )
//  {
//    return;
//  }
//
//  DWORD dwVertexSize = 0;
//  float fTU;
//  pParticle = m_pFirstUsedParticle;
//  dwTotalCount = 0;
//  while( pParticle )
//  {
//    dwTotalCount++;
//    if( dwTotalCount * 2 > m_pRender->m_dwRenderMeshSize )
//    {
//      break;
//    }
//
//    fTU = 1.0f - (float)dwTotalCount / dwMaxCount;
//
//    D3DCOLOR color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, pParticle->m_fLifeTime / fTotalTime );
//    pRenderMesh[dwVertexSize].p = pParticle->m_vPos;
//    pRenderMesh[dwVertexSize].tu = fTU;
//    pRenderMesh[dwVertexSize].tv = 0.0f;
//    pRenderMesh[dwVertexSize].color = color;
//    dwVertexSize++;
//    pRenderMesh[dwVertexSize].p = pParticle->m_vDir;
//    pRenderMesh[dwVertexSize].tu = fTU;
//    pRenderMesh[dwVertexSize].tv = 1.0f;
//    pRenderMesh[dwVertexSize].color = color;
//    dwVertexSize++;
//
//    pParticle = pParticle->m_pNextParticle;
//  }
//
//  m_pRender->m_dwVertexSize = dwVertexSize;
//  m_pRender->bEnabled = true;
//}
//
////-----------------------------------------------------------------------------
//// Name: DisableFrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleWeaponRibbon::DisableFrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  DWORD dwTotalCount = 0;
//  bool bErase = false;
//  DWORD dwMaxCount = m_pRender->m_dwRenderMeshSize >> 1;
//  cParticle* pParticle = m_pFirstUsedParticle;
//  while( pParticle )
//  {
//    cParticle* pNextParticle = pParticle->m_pNextParticle;
//
//    if( !bErase )
//    {
//      dwTotalCount++;
//
//      if( dwTotalCount > dwMaxCount )
//      {
//        bErase = true;
//        FreeParticle( pParticle );
//      }
//      else
//      {
//        pParticle->m_fLifeTime -= fElapsedTime;
//        if( pParticle->m_fLifeTime < 0.0f )
//        {
//          pParticle->m_fLifeTime = 0.0f;
//          bErase = true;
//          FreeParticle( pParticle );
//        }
//      }
//    }
//    else
//    {
//      FreeParticle( pParticle );
//    }
//
//    pParticle = pNextParticle;
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: ApplyTransformMatrix
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleWeaponRibbon::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
//{
//  if( pTransform == NULL )
//  {
//    m_bAddSegment = false;
//  }
//  else
//  {
//    if( m_pLerpInfo )
//    {
//      m_pLerpInfo->m_matCurTransform = *pTransform;
//    }
//
//    D3DXVec3TransformCoord( &m_vMinPos, &m_vOrigMinPos, pTransform );
//    D3DXVec3TransformCoord( &m_vMaxPos, &m_vOrigMaxPos, pTransform );
//    m_bAddSegment = true;
//  }
//}

//
////-----------------------------------------------------------------------------
//// Name: cParticleBillboard
//// Desc: 
////-----------------------------------------------------------------------------
//cParticleBillboard::cParticleBillboard( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer, DWORD dwBillboardType )
//:cParticleEffectBase( pParticlePool ), m_fWidth(0.0f), m_dwBillboardType(dwBillboardType),
// m_fCurAnimTime(0.0f)
//{
//  if( pSubMeshSerializer == NULL )
//  {
//    return;
//  }
//
//  SetLinkName( pSubMeshSerializer->getSubMeshName() );
//
//  MDLVTX* pVertex = pSubMeshSerializer->getSubMeshVerticesBuffer();
//  for( int i = 0; i < 3; i++ )
//  {
//    m_vCorner[i] = pVertex[i].p;
//  }
//
//  DWORD dwRenderAlphaBlendType = 0;
//  c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
//  if( pRender == NULL )
//  {
//    return;
//  }
//
//  m_pRender = (c3dsMaxParticleRender*)pRender->Create();
//  m_pRender->iMaterialSize = pSubMeshSerializer->getSubMeshMaterialSize();
//  if( m_pRender->iMaterialSize > 0 )
//  {
//    cMaterial* pMaterial;
//    cMaterialAnimator* pMaterialAnimator;
//    m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
//    for( int i = 0; i < m_pRender->iMaterialSize; i++ )
//    {
//      pMaterial = pSubMeshSerializer->getSubMeshMaterial( i );
//      if( pMaterial )
//      {
//        m_pRender->m_pMaterial[i].m_iNameSize = pMaterial->m_iNameSize;
//        if( pMaterial->m_iNameSize > 0 )
//        {
//          m_pRender->m_pMaterial[i].m_pszTextName = _NEW char[pMaterial->m_iNameSize + 1];
//          if( m_pRender->m_pMaterial[i].m_pszTextName )
//          {
//            strcpy( m_pRender->m_pMaterial[i].m_pszTextName, pMaterial->m_pszTextName );
//          }
//        }
//        m_pRender->m_pMaterial[i].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
//        m_pRender->m_pMaterial[i].SetTextureAniRows( pMaterial->GetTextureAniRows() );
//        m_pRender->m_pMaterial[i].SetTextureAniCols( pMaterial->GetTextureAniCols() );
//        m_pRender->m_pMaterial[i].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );
//
//        if( i == 0 )
//        {
//          dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
//        }
//      }
//
//      pMaterialAnimator = pSubMeshSerializer->GetMaterialAnimator( i );
//      if( pMaterialAnimator )
//      {
//        m_pRender->m_pMaterial[i].SetMaterialAnimator( pMaterialAnimator );
//
//        // 070212 cyhsieh extended material animation texture address
//        if( m_pRender->m_pMaterial[i].GetTextureTransformMatrix() )
//        {
//          m_pRender->m_pMaterial[i].GetAnimatorAddress( m_pRender->m_dwAddressU, m_pRender->m_dwAddressV );
//        }
//      }
//    }
//  }
//
//  m_pRender->iRenderType = 4;
//  m_pRender->m_dwCullMode = D3DCULL_NONE;
//
//  switch( dwRenderAlphaBlendType )
//  {
//  case 0:   // Opaque
//    m_pRender->m_bAlphaBlendenable = false;
//    m_pRender->m_bZWriteenable = true;
//    break;
//  case 1:   // Additive
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//    m_pRender->m_dwDestBlend = D3DBLEND_ONE;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  case 2:   // Alpha
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  case 3:   // Modulate
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//    m_pRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  case 4:   // Modulate Inv
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  }
//
//  m_pRender->m_dwRenderMeshSize = 4;
//  m_pRender->m_dwVertexSize = 0;
//  m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[pRender->m_dwRenderMeshSize];
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleBillboard::FrameMove( float fElapsedTime )
//{
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  float fFogAlphaFactor = ComputeFogAlphaFactor();
//  if( fFogAlphaFactor <= 0.0f )
//  {
//    return;
//  }
//
//  cParticle* pParticle = m_pFirstUsedParticle;
//  if( pParticle == NULL )
//  {
//    pParticle = m_pParticlePool->NewParticle();
//    if( pParticle )
//    {
//      // initialize
//      pParticle->m_dwTextureAniIndex = 0;
//      pParticle->m_fTextureAniTime = 0.0f;
//      pParticle->m_fLifeTime = 0;
//      AddParticle( pParticle );
//    }
//    else
//    {
//      return;
//    }
//  }
//
//  pParticle->m_vPos = m_vCurPosition;
//  pParticle->m_fSize = m_fWidth;
//  
//  // 070628 cyhsieh Billboard Animation Time
//  pParticle->m_fLifeTime = m_fCurAnimTime;  // 设定和Mesh的动画时间相
//
//  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
//  if( pRenderMesh == NULL )
//  {
//    return;
//  }
//
//  cMaterial* pMaterial = NULL;
//  if( m_pRender->iMaterialSize > 0 )
//  {
//    pMaterial = &m_pRender->m_pMaterial[0];
//  }
//
//  const D3DXMATRIXA16& matView = c3dsMaxParticleManager::GetCameraViewMatrix();
//
//  D3DXVECTOR3 vRight;
//  D3DXVECTOR3 vUp;
//  D3DXVECTOR3 vDir = D3DXVECTOR3( matView._13, matView._23, matView._33 );
//
//  switch( m_dwBillboardType )
//  {
//  case 0:
//    vRight = D3DXVECTOR3( matView._11, matView._21, matView._31 );
//    vUp = D3DXVECTOR3( matView._12, matView._22, matView._32 );
//    break;
//  case 1:
//    vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
//    D3DXVec3Cross( &vRight, &vUp, &vDir );
//    break;
//  }
//  
//  D3DXVECTOR3 vA = -vRight - vUp;
//	D3DXVECTOR3 vB = -vRight + vUp;
//	D3DXVECTOR3 vC =  vRight - vUp;
//	D3DXVECTOR3 vD =  vRight + vUp;
//
//  pRenderMesh[0].p = pParticle->m_vPos + vA * pParticle->m_fSize;
//  pRenderMesh[1].p = pParticle->m_vPos + vB * pParticle->m_fSize;
//  pRenderMesh[2].p = pParticle->m_vPos + vC * pParticle->m_fSize;
//  pRenderMesh[3].p = pParticle->m_vPos + vD * pParticle->m_fSize;
//
//  // set color
//  D3DCOLOR color = 0xFFFFFF | ( ( (DWORD)( fFogAlphaFactor * 255.0f ) ) << 24 );
//  if( pMaterial )
//  {
//    pMaterial->SetAniTime( pParticle->m_fLifeTime );
//    const D3DXCOLOR* pColor = pMaterial->GetDiffuseOpacity();
//    if( pColor )
//    {
//      color = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a * fFogAlphaFactor );
//    }
//  }
//  pRenderMesh[0].color = color;
//  pRenderMesh[1].color = color;
//  pRenderMesh[2].color = color;
//  pRenderMesh[3].color = color;
//
//  // 070205 cyhsieh texture animation row & col
//  pParticle->m_fTextureAniTime += fElapsedTime;
//  if( pParticle->m_fTextureAniTime > 160.0f )
//  {
//    pParticle->m_fTextureAniTime -= 160.0f;
//    DWORD dwRepeat = 0;
//    if( pMaterial && ( dwRepeat = pMaterial->GetTextureAniRepeat() ) > 0 )
//    {
//      DWORD dwTextureAniRows = pMaterial->GetTextureAniRows();
//      DWORD dwTextureAniCols = pMaterial->GetTextureAniCols();
//      DWORD dwRowCols = dwTextureAniRows * dwTextureAniCols;
//
//      DWORD dwIndex = pParticle->m_dwTextureAniIndex % ( dwRowCols );
//      float fUDiff = 1.0f / dwTextureAniRows;
//      float fVDiff = 1.0f / dwTextureAniCols;
//
//      float fStartU = ( dwIndex % dwTextureAniRows ) * fUDiff;
//      float fStartV = ( dwIndex % dwTextureAniCols ) * fVDiff;
//
//      pRenderMesh[0].tu = fStartU;
//      pRenderMesh[0].tv = fStartV + fVDiff;
//      pRenderMesh[1].tu = fStartU;
//      pRenderMesh[1].tv = fStartV;
//      pRenderMesh[2].tu = fStartU + fUDiff;
//      pRenderMesh[2].tv = fStartV + fVDiff;
//      pRenderMesh[3].tu = fStartU + fUDiff;
//      pRenderMesh[3].tv = fStartV;
//
//      if( dwRepeat == 999 || pParticle->m_dwTextureAniIndex < dwRepeat * dwRowCols - 1 )
//      {
//        pParticle->m_dwTextureAniIndex++;
//      }
//    }
//    else
//    {
//      pRenderMesh[0].tu = 0.0f;
//      pRenderMesh[0].tv = 1.0f;
//      pRenderMesh[1].tu = 0.0f;
//      pRenderMesh[1].tv = 0.0f;
//      pRenderMesh[2].tu = 1.0f;
//      pRenderMesh[2].tv = 1.0f;
//      pRenderMesh[3].tu = 1.0f;
//      pRenderMesh[3].tv = 0.0f;
//    }
//  }
//
//  m_pRender->m_dwVertexSize = 4;
//  m_pRender->bEnabled = true;
//}
//
////-----------------------------------------------------------------------------
//// Name: ApplyTransformMatrix
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleBillboard::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
//{
//  D3DXVECTOR3 vTransformed[3];
//
//  for( int i = 0; i < 3; i++ )
//  {
//    D3DXVec3TransformCoord( &vTransformed[i], &m_vCorner[i], pTransform );
//  }
//
//  D3DXVECTOR3 vDiff10 = vTransformed[1] - vTransformed[0];
//  D3DXVECTOR3 vDiff20 = vTransformed[2] - vTransformed[0];
//
//  m_vCurPosition = ( vTransformed[1] + vTransformed[2] ) * 0.5f;
//
//  m_fWidth = D3DXVec3Length( &( vTransformed[1] - vTransformed[0] ) );
//}
//
//
////-----------------------------------------------------------------------------
//// Name: cParticleZodiac
//// Desc: 
////-----------------------------------------------------------------------------
//cParticleZodiac::cParticleZodiac( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer )
//:cParticleEffectBase( pParticlePool ), m_fHalfWidth(0.0f), m_fCurAnimTime(0.0f)
//{
//  if( pSubMeshSerializer == NULL )
//  {
//    return;
//  }
//  
//  SetLinkName( pSubMeshSerializer->getSubMeshName() );
//
//  MDLVTX* pVertex = pSubMeshSerializer->getSubMeshVerticesBuffer();
//  for( int i = 0; i < 3; i++ )
//  {
//    m_vCorner[i] = pVertex[i].p;
//  }
//
//  DWORD dwRenderAlphaBlendType = 0;
//  c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
//  if( pRender == NULL )
//  {
//    return;
//  }
//
//  m_pRender = (c3dsMaxParticleRender*)pRender->Create();
//  m_pRender->iMaterialSize = pSubMeshSerializer->getSubMeshMaterialSize();
//  if( m_pRender->iMaterialSize > 0 )
//  {
//    cMaterial* pMaterial;
//    cMaterialAnimator* pMaterialAnimator;
//    m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
//    for( int i = 0; i < m_pRender->iMaterialSize; i++ )
//    {
//      pMaterial = pSubMeshSerializer->getSubMeshMaterial( i );
//      if( pMaterial )
//      {
//        m_pRender->m_pMaterial[i].m_iNameSize = pMaterial->m_iNameSize;
//        if( pMaterial->m_iNameSize > 0 )
//        {
//          m_pRender->m_pMaterial[i].m_pszTextName = _NEW char[pMaterial->m_iNameSize + 1];
//          if( m_pRender->m_pMaterial[i].m_pszTextName )
//          {
//            strcpy( m_pRender->m_pMaterial[i].m_pszTextName, pMaterial->m_pszTextName );
//          }
//        }
//        m_pRender->m_pMaterial[i].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
//        m_pRender->m_pMaterial[i].SetTextureAniRows( pMaterial->GetTextureAniRows() );
//        m_pRender->m_pMaterial[i].SetTextureAniCols( pMaterial->GetTextureAniCols() );
//        m_pRender->m_pMaterial[i].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );
//
//        if( i == 0 )
//        {
//          dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
//        }
//      }
//
//      pMaterialAnimator = pSubMeshSerializer->GetMaterialAnimator( i );
//      if( pMaterialAnimator )
//      {
//        m_pRender->m_pMaterial[i].SetMaterialAnimator( pMaterialAnimator );
//      }
//      
//      // Force Clamp
//      m_pRender->m_dwAddressU = D3DTADDRESS_CLAMP;
//      m_pRender->m_dwAddressV = D3DTADDRESS_CLAMP;
//    }
//  }
//
//  switch( dwRenderAlphaBlendType )
//  {
//  case 0:   // Opaque
//    m_pRender->m_bAlphaBlendenable = false;
//    m_pRender->m_bZWriteenable = true;
//    break;
//  case 1:   // Additive
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//    m_pRender->m_dwDestBlend = D3DBLEND_ONE;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  case 2:   // Alpha
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
//    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  case 3:   // Modulate
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//    m_pRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  case 4:   // Modulate Inv
//    m_pRender->m_bAlphaBlendenable = true;
//    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
//    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
//    m_pRender->m_bZWriteenable = false;
//    break;
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleZodiac::FrameMove( float fElapsedTime )
//{
//  // !!!To do...Optimize
//
//  if( m_pRender == NULL )
//  {
//    return;
//  }
//
//  float fFogAlphaFactor = ComputeFogAlphaFactor();
//  if( fFogAlphaFactor <= 0.0f )
//  {
//    return;
//  }
//
//  if( g_pGameTerrain == NULL ) return;
//
//  DWORD dwVertexSize = 0;
//
//  cThing* pFloorThing = NULL;
//
//  float fStartX = m_vCurPosition.x - m_fHalfWidth;
//  float fStartZ = m_vCurPosition.z - m_fHalfWidth;
//  float fEndX = m_vCurPosition.x + m_fHalfWidth;
//  float fEndZ = m_vCurPosition.z + m_fHalfWidth;
//
//  float fOffset = g_pGameTerrain->GetTilePosOffset();
//
//  int iTileX = (int)( ( m_vCurPosition.x + fOffset ) / COMP );
//  int iTileZ = (int)( ( m_vCurPosition.z + fOffset ) / COMP );
//  cTile* pTile = g_pGameTerrain->GetTile( iTileX, iTileZ );
//  if( pTile && pTile->m_pThing && pTile->m_pThing->getFloorMesh() )
//  {
//    pFloorThing = pTile->m_pThing;
//  }
//
//  float fOneOverDifferenceX = 1.0f / ( fEndX - fStartX );
//  float fOneOverDifferenceZ = 1.0f / ( fEndZ - fStartZ );
//  float fTU, fTV;
//
//  cMaterial* pMaterial = NULL;
//  if( m_pRender->iMaterialSize > 0 )
//  {
//    pMaterial = &m_pRender->m_pMaterial[0];
//  }
//
//  D3DCOLOR color = 0xFFFFFF | ( ( (DWORD)( fFogAlphaFactor * 255.0f ) ) << 24 );
//  if( pMaterial )
//  {
//    pMaterial->SetAniTime( m_fCurAnimTime );
//    const D3DXCOLOR* pColor = pMaterial->GetDiffuseOpacity();
//    if( pColor )
//    {
//      color = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a * fFogAlphaFactor );
//    }
//  }
//
//  if(( pFloorThing )&&(pFloorThing->getFloorMesh()->GetFaceCount()>0)) // 位置在可站地上物上面，计算地上物Mesh的Decal(目前只计算正站在上面的地上物)
//  {
//    D3DXVECTOR3 vNewVertex[7];  // Clipping a convex polygon having n vertices to a plane
//                                // results in a new convex plygon having at most n + 1 vertices
//
//    // Calculate boundary planes
//    m_planeLeft.d = m_fHalfWidth - m_vCurPosition.x;
//    m_planeRight.d = m_fHalfWidth + m_vCurPosition.x;
//    m_planeBack.d = m_fHalfWidth - m_vCurPosition.z;
//    m_planeFront.d = m_fHalfWidth + m_vCurPosition.z;
//
//    m_dwDecalVertexCount = 0;
//    m_dwDecalIndexCount = 0;
//
//    // Clip one triangle at a time
//    D3DXVECTOR3 vCross;
//		D3DXVECTOR3 v0,v1,v2,v01,v02;
//		DWORD iFaceCount=pFloorThing->getFloorMesh()->GetFaceCount();
//    for( DWORD i = 0; i < iFaceCount; i++ )
//    {
//			pFloorThing->getFloorMesh()->GetFaceTriangle(i,&v0,&v1,&v2);
//			v01=v1-v0;
//			v02=v2-v0;
//
//      D3DXVec3Cross( &vCross, &v01, &v02 );
//
//      // vCross dot vDecalNormal(0,1,0) = vCross.y
//      if( vCross.y > ( DECAL_EPSILON * D3DXVec3Length( &vCross ) ) )
//      {
//        vNewVertex[0] = v0;
//        vNewVertex[1] = v1;
//        vNewVertex[2] = v2;
//
//        int iCount = ClipDecalPolygon( 3, vNewVertex, vNewVertex );
//        if( ( iCount != 0 ) && ( AddDecalPolygon( iCount, vNewVertex ) == false ) )
//        {
//          break;
//        }
//      }
//    }
//
//    if( m_pRender->m_dwRenderMeshSize < m_dwDecalVertexCount )
//    {
//      SAFE_DELETE( m_pRender->m_pRenderMesh );
//      m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[m_dwDecalVertexCount];
//      if( m_pRender->m_pRenderMesh == NULL )
//      {
//        m_pRender->m_dwVertexSize = 0;
//        m_pRender->m_dwIndexSize = 0;
//        m_pRender->m_dwRenderMeshSize = 0;
//        return;
//      }
//      m_pRender->m_dwRenderMeshSize = m_dwDecalVertexCount;
//    }
//    if( m_pRender->m_dwRenderMeshIndexSize < m_dwDecalIndexCount )
//    {
//      SAFE_DELETE( m_pRender->m_pRenderMeshIndex );
//      m_pRender->m_pRenderMeshIndex = _NEW WORD[m_dwDecalIndexCount];
//      if( m_pRender->m_pRenderMeshIndex == NULL )
//      {
//        m_pRender->m_dwVertexSize = 0;
//        m_pRender->m_dwIndexSize = 0;
//        m_pRender->m_dwRenderMeshIndexSize = 0;
//        return;
//      }
//      m_pRender->m_dwRenderMeshIndexSize = m_dwDecalIndexCount;
//    }
//
//    EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
//
//    // Copy Decal Vertices
//    dwVertexSize = 0;
//    for( unsigned int i = 0; i < m_dwDecalVertexCount; i++ )
//    {
//      pRenderMesh[dwVertexSize].p = m_vDecalVertices[i];
//      pRenderMesh[dwVertexSize].p.y += 0.1f;
//      fTU = ( ( pRenderMesh[dwVertexSize].p.x - fStartX ) * fOneOverDifferenceX );
//      fTV = ( ( pRenderMesh[dwVertexSize].p.z - fStartZ ) * fOneOverDifferenceZ );
//      pRenderMesh[dwVertexSize].tu = fTU;
//      pRenderMesh[dwVertexSize].tv = 1.0f - fTV;
//      pRenderMesh[dwVertexSize].color = color;
//      dwVertexSize++;
//    }
//
//    // Copy Decal Indices
//    memcpy( m_pRender->m_pRenderMeshIndex, m_wDecalIndices, sizeof(WORD) * m_dwDecalIndexCount );
//    m_pRender->m_dwIndexSize = m_dwDecalIndexCount;
//
//    m_pRender->iRenderType = RENDER_TYPE_5; // Special Render Type
//  }
//  else  // 位置在地表上面
//  {
//    int iScale = g_pGameTerrain->GetMeshScale();
//    float fSize = (float)( COMP * iScale );
//    float fNodeX = ( fStartX + fOffset ) / fSize;
//    float fNodeZ = ( fStartZ + fOffset ) / fSize;
//    int iNodeStartX = (int)( floor( fNodeX ) );
//    int iNodeStartZ = (int)( floor( fNodeZ ) );
//
//    fNodeX = ( fEndX + fOffset ) / fSize;
//    fNodeZ = ( fEndZ + fOffset ) / fSize;
//    int iNodeEndX = (int)( ceil( fNodeX ) );
//    int iNodeEndZ = (int)( ceil( fNodeZ ) );
//
//    int iNodeSizeX  = iNodeEndX - iNodeStartX;
//    int iNodeSizeZ  = iNodeEndZ - iNodeStartZ;
//
//    dwVertexSize = iNodeSizeX * iNodeSizeZ * 4;
//    if( m_pRender->m_dwRenderMeshSize < dwVertexSize )
//    {
//      SAFE_DELETE( m_pRender->m_pRenderMesh );
//      m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[dwVertexSize];
//      if( m_pRender->m_pRenderMesh == NULL )
//      {
//        m_pRender->m_dwVertexSize = 0;
//        m_pRender->m_dwRenderMeshSize = 0;
//        return;
//      }
//      m_pRender->m_dwRenderMeshSize = dwVertexSize;
//    }
//
//    EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
//
//    dwVertexSize = 0;
//    for( int j = 0; j < iNodeSizeZ; j++ )
//    {
//      for( int i = 0; i < iNodeSizeX; i++ )
//      {
//        pRenderMesh[dwVertexSize + 0].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i, iNodeStartZ + j );
//        pRenderMesh[dwVertexSize + 0].p.y += 0.1f;
//        fTU = ( ( pRenderMesh[dwVertexSize + 0].p.x - fStartX ) * fOneOverDifferenceX );
//        fTV = ( ( pRenderMesh[dwVertexSize + 0].p.z - fStartZ ) * fOneOverDifferenceZ );
//        pRenderMesh[dwVertexSize + 0].tu = fTU;
//        pRenderMesh[dwVertexSize + 0].tv = 1.0f - fTV;
//        pRenderMesh[dwVertexSize + 0].color = color;
//
//        pRenderMesh[dwVertexSize + 1].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i, iNodeStartZ + j + 1 );
//        pRenderMesh[dwVertexSize + 1].p.y += 0.1f;
//        fTU = ( ( pRenderMesh[dwVertexSize + 1].p.x - fStartX ) * fOneOverDifferenceX );
//        fTV = ( ( pRenderMesh[dwVertexSize + 1].p.z - fStartZ ) * fOneOverDifferenceZ );
//        pRenderMesh[dwVertexSize + 1].tu = fTU;
//        pRenderMesh[dwVertexSize + 1].tv = 1.0f - fTV;
//        pRenderMesh[dwVertexSize + 1].color = color;
//
//        pRenderMesh[dwVertexSize + 2].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i + 1, iNodeStartZ + j );
//        pRenderMesh[dwVertexSize + 2].p.y += 0.1f;
//        fTU = ( ( pRenderMesh[dwVertexSize + 2].p.x - fStartX ) * fOneOverDifferenceX );
//        fTV = ( ( pRenderMesh[dwVertexSize + 2].p.z - fStartZ ) * fOneOverDifferenceZ );
//        pRenderMesh[dwVertexSize + 2].tu = fTU;
//        pRenderMesh[dwVertexSize + 2].tv = 1.0f - fTV;
//        pRenderMesh[dwVertexSize + 2].color = color;
//
//        pRenderMesh[dwVertexSize + 3].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i + 1, iNodeStartZ + j + 1 );
//        pRenderMesh[dwVertexSize + 3].p.y += 0.1f;
//        fTU = ( ( pRenderMesh[dwVertexSize + 3].p.x - fStartX ) * fOneOverDifferenceX );
//        fTV = ( ( pRenderMesh[dwVertexSize + 3].p.z - fStartZ ) * fOneOverDifferenceZ );
//        pRenderMesh[dwVertexSize + 3].tu = fTU;
//        pRenderMesh[dwVertexSize + 3].tv = 1.0f - fTV;
//        pRenderMesh[dwVertexSize + 3].color = color;
//
//        dwVertexSize += 4;
//      }
//    }
//
//    m_pRender->iRenderType = RENDER_TYPE_0;
//    m_pRender->m_dwIndexSize = 0;
//  }
//
//  m_pRender->m_dwVertexSize = dwVertexSize;
//  m_pRender->bEnabled = true;
//}
//
////-----------------------------------------------------------------------------
//// Name: ApplyTransformMatrix
//// Desc: 
////-----------------------------------------------------------------------------
//void cParticleZodiac::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
//{
//  D3DXVECTOR3 vTransformed[3];
//
//  for( int i = 0; i < 3; i++ )
//  {
//    D3DXVec3TransformCoord( &vTransformed[i], &m_vCorner[i], pTransform );
//  }
//
//  D3DXVECTOR3 vDiff10 = vTransformed[1] - vTransformed[0];
//  D3DXVECTOR3 vDiff20 = vTransformed[2] - vTransformed[0];
//
//  m_vCurPosition = ( vTransformed[1] + vTransformed[2] ) * 0.5f;
//
//  m_fHalfWidth = D3DXVec3Length( &( vTransformed[1] - vTransformed[0] ) ) * 0.5f;
//}
//
////-----------------------------------------------------------------------------
//// Name: ClipDecalPolygon
//// Desc: 计算Plane切割结果
////-----------------------------------------------------------------------------
//int cParticleZodiac::ClipDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex )
//{
//  D3DXVECTOR3 vTempVertex[7];
//	
//	// Clip against planes
//	int iCount = ClipDecalPolygonAgainstPlane( m_planeLeft, iVertexCount, pVertex, vTempVertex );
//	if( iCount != 0 )
//	{
//		iCount = ClipDecalPolygonAgainstPlane( m_planeRight, iCount, vTempVertex, pNewVertex );
//		if( iCount != 0 )
//		{
//			iCount = ClipDecalPolygonAgainstPlane( m_planeBack, iCount, pNewVertex, vTempVertex );
//			if( iCount != 0 )
//			{
//				iCount = ClipDecalPolygonAgainstPlane( m_planeFront, iCount, vTempVertex, pNewVertex );
//			}
//		}
//	}
//	
//	return iCount;
//}
//
////-----------------------------------------------------------------------------
//// Name: ClipDecalPolygonAgainstPlane
//// Desc: 计算Plane切出来的Decal Polygon
////-----------------------------------------------------------------------------
//int cParticleZodiac::ClipDecalPolygonAgainstPlane( const D3DXPLANE& plane, int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex )
//{
//  bool bNegative[7];
//	
//	// Classify vertices
//	int iNegativeCount = 0;
//	for( int i = 0; i < iVertexCount; i++ )
//	{
//		bool bNeg = D3DXPlaneDotCoord( &plane, &pVertex[i] ) < 0.0f;
//		bNegative[i] = bNeg;
//		iNegativeCount += bNeg;
//	}
//	
//	// Discard this polygon if it's completely culled
//	if( iNegativeCount == iVertexCount )
//  {
//    return 0;
//  }
//
//	int iCount = 0;
//	for( int i = 0; i < iVertexCount; i++ )
//	{
//		// the index of the previous vertex
//		int iPre = ( i != 0 ) ? i - 1 : iVertexCount - 1;
//		
//		if( bNegative[i] )
//		{
//			if( !bNegative[iPre] )
//			{
//				// Current vertex is on negative side of plane,
//				// but previous vertex is on positive side.
//				const D3DXVECTOR3& v1 = pVertex[iPre];
//				const D3DXVECTOR3& v2 = pVertex[i];
//				float fT = D3DXPlaneDotCoord( &plane, &v1 ) / ( plane.a * ( v1.x - v2.x ) + plane.b * ( v1.y - v2.y ) + plane.c * ( v1.z - v2.z ) );
//				pNewVertex[iCount] = v1 * ( 1.0f - fT ) + v2 * fT;
//				iCount++;
//			}
//		}
//		else
//		{
//			if( bNegative[iPre] )
//			{
//				// Current vertex is on positive side of plane,
//				// but previous vertex is on negative side.
//				const D3DXVECTOR3& v1 = pVertex[i];
//				const D3DXVECTOR3& v2 = pVertex[iPre];
//				float fT = D3DXPlaneDotCoord( &plane, &v1 ) / ( plane.a * ( v1.x - v2.x ) + plane.b * ( v1.y - v2.y ) + plane.c * ( v1.z - v2.z ) );
//				pNewVertex[iCount] = v1 * ( 1.0f - fT ) + v2 * fT;
//				iCount++;
//			}
//			
//			// Include current vertex
//			pNewVertex[iCount] = pVertex[i];
//			iCount++;
//		}
//	}
//	
//	// Return number of vertices in clipped polygon
//	return iCount;
//}
//
////-----------------------------------------------------------------------------
//// Name: AddDecalPolygon
//// Desc: 增加Decal Polygon
////-----------------------------------------------------------------------------
//bool cParticleZodiac::AddDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex )
//{
//  int iCount = m_dwDecalVertexCount;
//	if( iCount + iVertexCount >= MAX_DECAL_VERTICES )
//  {
//    return false;
//  }
//	
//	// Add polygon as a triangle fan
//  int iIndex = m_dwDecalIndexCount;
//  m_dwDecalIndexCount += ( iVertexCount - 2 ) * 3;
//  for( int i = 2; i < iVertexCount; i++ )
//  {
//    m_wDecalIndices[iIndex++] = iCount;
//    m_wDecalIndices[iIndex++] = iCount + i - 1;
//    m_wDecalIndices[iIndex++] = iCount + i;
//  }
//	
//	for( int i = 0; i < iVertexCount; i++ )
//	{
//		m_vDecalVertices[iCount] = pVertex[i];
//		iCount++;
//	}
//	
//	m_dwDecalVertexCount = iCount;
//
//  return true;
//}

//
////-----------------------------------------------------------------------------
//// Name: c3dsMaxParticleManager
//// Desc: 
////-----------------------------------------------------------------------------
//c3dsMaxParticleManager::c3dsMaxParticleManager()
//:m_bEnable(false), m_pRenderBuffer(NULL), m_pTextureDB(NULL), m_pParticlePool(NULL), m_pDecalIndexBuffer(NULL),
// m_pParticleEffectList(NULL), m_pParticleEffectSortList(NULL), m_pForceDrawParticleEffectList(NULL)
//{
//}
//
////-----------------------------------------------------------------------------
//// Name: ~c3dsMaxParticleManager
//// Desc: 
////-----------------------------------------------------------------------------
//c3dsMaxParticleManager::~c3dsMaxParticleManager()
//{
//  SAFE_DELETE( m_pForceDrawParticleEffectList );
//  SAFE_DELETE( m_pParticleEffectSortList )
//  if( m_pParticleEffectList )
//  {
//    cParticleEffectBase* pParticleEffect;
//    int iCount = m_pParticleEffectList->GetCount();
//    for( int i = 0; i < iCount; i++ )
//    {
//      pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
//      SAFE_DELETE( pParticleEffect );
//    }
//    delete m_pParticleEffectList;
//  }
//
//  SAFE_DELETE( m_pParticlePool );
//  SAFE_DELETE( m_pTextureDB );
//  SAFE_RELEASE( m_pRenderBuffer );
//  SAFE_RELEASE( m_pDecalIndexBuffer );
//}
//
////-----------------------------------------------------------------------------
//// Name: InitializeResource
//// Desc: 
////-----------------------------------------------------------------------------
//void c3dsMaxParticleManager::InitializeResource( LPDIRECT3DDEVICE9 pD3DDevice )
//{
//  int iMaxParticleSize = 5120;
//
//  m_pRenderBuffer = g_pRBManage->CreateNewBuffer();
//  if( m_pRenderBuffer )
//  {
//    int iVertexTotalSize = iMaxParticleSize * 4;
//    int iIndexTotalSize = iMaxParticleSize * 6;
//    m_pRenderBuffer->SetVertexBufferSize( pD3DDevice, cRenderBuffer::vtDyEFFECT_MDLVTX, iVertexTotalSize );
//    m_pRenderBuffer->SetIndexBufferSize( pD3DDevice, cRenderBuffer::itStaticWORD, iIndexTotalSize );
//
//    unsigned int iLockIndex = 0;
//    int iStartIndex = 0;
//    int iStartVertex = 0;
//    WORD* pIndex = m_pRenderBuffer->m_pWStaticIB->Lock( iIndexTotalSize, iLockIndex );
//    if( pIndex )
//    {
//      for( int i = 0; i < iMaxParticleSize; i++ )
//      {
//        pIndex[iStartIndex + 0] = iStartVertex + 0;
//        pIndex[iStartIndex + 1] = iStartVertex + 1;
//        pIndex[iStartIndex + 2] = iStartVertex + 2;
//        pIndex[iStartIndex + 3] = iStartVertex + 1;
//        pIndex[iStartIndex + 4] = iStartVertex + 3;
//        pIndex[iStartIndex + 5] = iStartVertex + 2;
//        iStartIndex += 6;
//        iStartVertex += 4;
//      }
//      m_pRenderBuffer->m_pWStaticIB->Unlock();
//    }
//  }
//
//  m_pDecalIndexBuffer = g_pRBManage->CreateNewBuffer();
//  if( m_pDecalIndexBuffer )
//  {
//    m_pDecalIndexBuffer->SetIndexBufferSize( pD3DDevice, cRenderBuffer::itDynamicWORD, 2560 );
//  }
//
//  m_pTextureDB = _NEW cTextureDB();
//  if( m_pTextureDB )
//  {
//    m_pTextureDB->InitDeviceObjects( pD3DDevice );
//  }
//
//  m_pParticlePool = _NEW cParticlePool( iMaxParticleSize, NULL, sizeof(cParticle) );
//
//  m_pParticleEffectList = _NEW BTList;
//  m_pParticleEffectSortList = _NEW BTList;
//  m_pForceDrawParticleEffectList = _NEW BTList;
//}
//
////-----------------------------------------------------------------------------
//// Name: CreateParticleEffectByEmitData
//// Desc: 产生Max粒子特效
////-----------------------------------------------------------------------------
//cParticleEffectBase* c3dsMaxParticleManager::CreateParticleEffectByEmitData( c3dsMaxEmitDataBase* pEmitData, char* pTexurePath )
//{
//  if( m_pParticleEffectList == NULL )
//  {
//    return NULL;
//  }
//
//	static char* path_id[2]={ EFFECT_STUFF_PATH, MESH_STUFF_PATH };
//
//  cParticleEffectBase* pParticleEffect = NULL;
//  
//  if( pEmitData )
//  {
//    DWORD dwDataType = pEmitData->GetDataType();
//    switch( dwDataType )
//    {
//    case 1:
//      {
//        c3dsMaxEmitDataCloud* pEmitDataCloud = (c3dsMaxEmitDataCloud*)pEmitData;
//        pParticleEffect = _NEW cParticleCloud( m_pParticlePool, pEmitDataCloud );
//      }
//      break;
//
//    // 070205 cyhsieh particle superspray
//    case 2:
//      {
//        c3dsMaxEmitDataSuperSpray* pEmitDataSuperSpray = (c3dsMaxEmitDataSuperSpray*)pEmitData;
//        pParticleEffect = _NEW cParticleSuperSpray( m_pParticlePool, pEmitDataSuperSpray );
//      }
//      break;
//    }
//  }
//
//  if( pParticleEffect )
//  {
//    c3dsMaxParticleRender* pRender = pParticleEffect->GetRender();
//    if( pRender )
//    {
//      pRender->m_pRenderBuffer = m_pRenderBuffer;
//
//      if( pRender->iMaterialSize )
//      {
//        // 地物特效
//        EZString szTexturePath;
//				char* stuff_path;
//        if( pTexurePath == NULL )
//        {
//          szTexturePath = "Effect\\efm\\";
//					stuff_path=path_id[0];
//        }
//        else
//        {
//          szTexturePath = pTexurePath;
//					stuff_path=path_id[1];
//        }
//
//        for( int i = 0; i < pRender->iMaterialSize; i++ )
//        {
//          cMaterial& material = pRender->m_pMaterial[i];
//          if( material.m_pszTextName )
//          {
//			  // 地物特效
//            material.m_pTexture = m_pTextureDB->QueryTexture(material.m_pszTextName,szTexturePath.c_str(),stuff_path,sTextureQueryParam( true, 0 )); 
//          }
//        }
//      }
//    }
//
//    m_pParticleEffectList->Add( pParticleEffect );
//  }
//
//  return pParticleEffect;
//}
//
////-----------------------------------------------------------------------------
//// Name: CreateParticleEffectBySubMeshSerializer
//// Desc: 产生Billboard粒子特效
////-----------------------------------------------------------------------------
//cParticleEffectBase* c3dsMaxParticleManager::CreateParticleEffectBySubMeshSerializer( cSubMeshSerializer* pSubMeshSerializer, char* pTexurePath )
//{
//  if( m_pParticleEffectList == NULL )
//  {
//    return NULL;
//  }
//
//	static char* path_id[2]={ EFFECT_STUFF_PATH, MESH_STUFF_PATH };
//
//  cParticleEffectBase* pParticleEffect = NULL;
//  
//  if( pSubMeshSerializer )
//  {
//    DWORD dwMeshType = pSubMeshSerializer->GetMeshType();
//    switch( dwMeshType )
//    {
//    case 1: // Billboard
//      pParticleEffect = _NEW cParticleBillboard( m_pParticlePool, pSubMeshSerializer, 0 );
//      break;
//    case 2: // Billboard Y
//      pParticleEffect = _NEW cParticleBillboard( m_pParticlePool, pSubMeshSerializer, 1 );
//      break;
//    case 3: // Ribbon
//      pParticleEffect = _NEW cParticleRibbon( m_pParticlePool, pSubMeshSerializer );
//      break;
//    case 4: // Zodiac
//      pParticleEffect = _NEW cParticleZodiac( m_pParticlePool, pSubMeshSerializer );
//      break;
//    }
//  }
//
//  if( pParticleEffect )
//  {
//    c3dsMaxParticleRender* pRender = pParticleEffect->GetRender();
//    if( pRender )
//    {
//      pRender->m_pRenderBuffer = m_pRenderBuffer;
//      pRender->m_pDecalIndexBuffer = m_pDecalIndexBuffer;
//
//      if( pRender->iMaterialSize )
//      {
//        // 地物特效
//        EZString szTexturePath;
//				char* stuff_path;
//        if( pTexurePath == NULL )
//        {
//          szTexturePath = "Effect\\efm\\";
//					stuff_path=path_id[0];
//        }
//        else
//        {
//          szTexturePath = pTexurePath;
//					stuff_path=path_id[1];
//        }
//
//        for( int i = 0; i < pRender->iMaterialSize; i++ )
//        {
//          cMaterial& material = pRender->m_pMaterial[i];
//          if( material.m_pszTextName )
//          {
//            //地物特效
//            material.m_pTexture = m_pTextureDB->QueryTexture(material.m_pszTextName,szTexturePath.c_str(),stuff_path,sTextureQueryParam( true, 0 ));
//          }
//        }
//      }
//    }
//
//    m_pParticleEffectList->Add( pParticleEffect );
//  }
//
//  return pParticleEffect;
//}
//
////-----------------------------------------------------------------------------
//// Name: CreateParticleEffectWeaponRibbon
//// Desc: 产生刀光特效
////-----------------------------------------------------------------------------
//// 070424 cyhsieh 猌竟
//cParticleEffectBase* c3dsMaxParticleManager::CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath )
//{
//  if( m_pParticleEffectList == NULL )
//  {
//    return NULL;
//  }
//
//  cParticleEffectBase* pParticleEffect = _NEW cParticleWeaponRibbon( m_pParticlePool, bDoLerp, vMinPos, vMaxPos );
//	static char* path_id[2]={ EFFECT_STUFF_PATH, MESH_STUFF_PATH };
//  
//  if( pParticleEffect )
//  {
//    c3dsMaxParticleRender* pRender = pParticleEffect->GetRender();
//    if( pRender )
//    {
//      pRender->m_pRenderBuffer = m_pRenderBuffer;
//
//      if( pRender->iMaterialSize )
//      {
//        EZString szTexturePath;
//				char* stuff_path;
//        if( pTexurePath == NULL )
//        {
//          szTexturePath = "Effect\\efm\\";
//					stuff_path=path_id[0];
//        }
//        else
//        {
//          szTexturePath = pTexurePath;
//					stuff_path=path_id[1];
//        }
//
//        for( int i = 0; i < pRender->iMaterialSize; i++ )
//        {
//          cMaterial& material = pRender->m_pMaterial[i];
//          if( material.m_pszTextName )
//          {
//            material.m_pTexture = m_pTextureDB->QueryTexture(material.m_pszTextName,szTexturePath.c_str(),stuff_path,sTextureQueryParam( true, 0 ));
//          }
//        }
//      }
//    }
//
//    m_pParticleEffectList->Add( pParticleEffect );
//  }
//
//  return pParticleEffect;
//}
//
////-----------------------------------------------------------------------------
//// Name: ReleaseParticleEffect
//// Desc: 睦采疭
////-----------------------------------------------------------------------------
//void c3dsMaxParticleManager::ReleaseParticleEffect( cParticleEffectBase* pRelease )
//{
//  if( m_pParticleEffectList == NULL )
//  {
//    return;
//  }
//
//  cParticleEffectBase* pParticleEffect = NULL;
//  int iCount;
//  
//  iCount = m_pParticleEffectList->GetCount();
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
//    if( pParticleEffect == pRelease )
//    {
//      SAFE_DELETE( pParticleEffect );
//      m_pParticleEffectList->Exchange( i, iCount - 1 );
//      m_pParticleEffectList->Delete( iCount - 1 );
//      return;
//    }
//  }
//
//  iCount = m_pParticleEffectList->GetCount();
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
//    if( pParticleEffect == pRelease )
//    {
//      SAFE_DELETE( pParticleEffect );
//      m_pParticleEffectList->Exchange( i, iCount - 1 );
//      m_pParticleEffectList->Delete( iCount - 1 );
//      return;
//    }
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: FrameMove
//// Desc: Framemove
////-----------------------------------------------------------------------------
//void c3dsMaxParticleManager::FrameMove( float fElapsedTime )
//{
//  // 070412 cyhsieh 疭
//  if( m_bEnable == false )
//  {
//    return;
//  }
//
//  float fAdjustElapsedTime = fElapsedTime * 4800.0f;
//  cParticleEffectBase* pParticleEffect = NULL;
//  int iCount = m_pParticleEffectList->GetCount();
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
//    if( pParticleEffect && ( pParticleEffect->GetRenderEnabled() == false ) )
//    {
//      pParticleEffect->DisableFrameMove( fAdjustElapsedTime );
//    }
//  }
//
//  if( m_pTextureDB )
//  {
//    m_pTextureDB->FrameMove( fElapsedTime );
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: ParticleEffectDistCompare
//// Desc: 
////-----------------------------------------------------------------------------
//int ParticleEffectDistCompare( void* pArg0, void* pArg1 )
//{
//  cParticleEffectBase* pParticleEffect0 = (cParticleEffectBase *)pArg0;
//  cParticleEffectBase* pParticleEffect1 = (cParticleEffectBase *)pArg1;
//
//  const D3DXVECTOR3& vCameraPos = c3dsMaxParticleManager::GetCameraPos();
//	D3DXVECTOR3 vDif0 = pParticleEffect0->GetCurPosition() - vCameraPos;
//	D3DXVECTOR3 vDif1 = pParticleEffect1->GetCurPosition() - vCameraPos;
//
//	float fDist0 = D3DXVec3LengthSq( &vDif0 );
//	float fDist1 = D3DXVec3LengthSq( &vDif1 );
//
//	return int( fDist1 - fDist0 );
//}
//
////-----------------------------------------------------------------------------
//// Name: Draw
//// Desc: Draw
////-----------------------------------------------------------------------------
//void c3dsMaxParticleManager::Draw( LPDIRECT3DDEVICE9 pD3DDevice )
//{
//	// 不打光、不打雾在呼叫此函式前已设定
//
//	// 070412 cyhsieh 长效特效
//  if( m_bEnable == false )
//  {
//    return;
//  }
//
//  m_pRenderBuffer->SetStreamSource( pD3DDevice );
//  m_pRenderBuffer->SetIndices( pD3DDevice );
//  m_pRenderBuffer->SetVertexType( pD3DDevice );
//
//  D3DXMATRIXA16 matWorld;
//  D3DXMatrixIdentity( &matWorld );
//  pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
//
//  cParticleEffectBase* pParticleEffect = NULL;
//  int iCount;
//  iCount = m_pParticleEffectList->GetCount();
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
//    if( pParticleEffect && pParticleEffect->GetRenderEnabled() )
//    {
//      if( pParticleEffect->GetRenderAlphaBlendType() > 0 )
//      {
//        m_pParticleEffectSortList->Add( pParticleEffect );
//      }
//      else
//      {
//        pParticleEffect->Draw( pD3DDevice );
//      }
//    }
//  }
//
//  iCount = m_pParticleEffectSortList->GetCount();
//  if( iCount > 0 )
//  {
//    m_pParticleEffectSortList->Sort( ParticleEffectDistCompare );
//
//    for( int i = 0; i < iCount; i++ )
//    {
//      pParticleEffect = (cParticleEffectBase*)m_pParticleEffectSortList->GetItem( i );
//      if( pParticleEffect )
//      {
//        pParticleEffect->Draw( pD3DDevice );
//      }
//    }
//
//    m_pParticleEffectSortList->Clear();
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: AddForceDrawParticleEffect
//// Desc: 增加照片用粒子特效
////-----------------------------------------------------------------------------
//// 071225 cyhsieh 疭
//void c3dsMaxParticleManager::AddForceDrawParticleEffect( cParticleEffectBase* pParticleEffect )
//{
//  if( m_pForceDrawParticleEffectList && pParticleEffect )
//  {
//    m_pForceDrawParticleEffectList->Add( pParticleEffect );
//  }
//}
//
////-----------------------------------------------------------------------------
//// Name: ForceDraw
//// Desc: 强制画(给照片用)
////-----------------------------------------------------------------------------
//// 071218 cyhsieh 人物照片
//void c3dsMaxParticleManager::ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice )
//{
//  if( ( m_pForceDrawParticleEffectList == NULL ) || ( m_pParticleEffectSortList == NULL ) )
//  {
//    return;
//  }
//
//  int iCount = m_pForceDrawParticleEffectList->GetCount();
//  if( iCount == 0 )
//  {
//    return;
//  }
//
//  // 不打光、不打雾在呼叫此函式前已设定
//
//  m_pRenderBuffer->SetStreamSource( pD3DDevice );
//  m_pRenderBuffer->SetIndices( pD3DDevice );
//  m_pRenderBuffer->SetVertexType( pD3DDevice );
//
//  D3DXMATRIXA16 matWorld;
//  D3DXMatrixIdentity( &matWorld );
//  pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
//
//  cParticleEffectBase* pParticleEffect = NULL;
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (cParticleEffectBase*)m_pForceDrawParticleEffectList->GetItem( i );
//    if( pParticleEffect && pParticleEffect->GetRenderEnabled() )
//    {
//      if( pParticleEffect->GetRenderAlphaBlendType() > 0 )
//      {
//        m_pParticleEffectSortList->Add( pParticleEffect );
//      }
//      else
//      {
//        pParticleEffect->Draw( pD3DDevice );
//      }
//    }
//  }
//  m_pForceDrawParticleEffectList->Clear();
//
//  iCount = m_pParticleEffectSortList->GetCount();
//  if( iCount > 0 )
//  {
//    m_pParticleEffectSortList->Sort( ParticleEffectDistCompare );
//
//    for( int i = 0; i < iCount; i++ )
//    {
//      pParticleEffect = (cParticleEffectBase*)m_pParticleEffectSortList->GetItem( i );
//      if( pParticleEffect )
//      {
//        pParticleEffect->Draw( pD3DDevice );
//      }
//    }
//
//    m_pParticleEffectSortList->Clear();
//  }
//}

}