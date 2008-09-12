//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/WeaponRibbonparticleemitter.h"

namespace KOK
{
using namespace IO;
using namespace Memory;

//  武器刀光
WeaponRibbonParticleEmitter::WeaponRibbonParticleEmitter( cParticlePool* pParticlePool, bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos )
:ParticleEmitter( pParticlePool ),
 m_vOrigMinPos(vMinPos), m_vOrigMaxPos(vMaxPos), m_vMinPos(vMinPos), m_vMaxPos(vMaxPos), m_bAddSegment(false),
 m_pLerpInfo(NULL)
{
  if( bDoLerp )
  {
    m_pLerpInfo = _NEW sLerpInfo;
  }

  c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
  m_pRender = (c3dsMaxParticleRender*)pRender->Create();
  if( m_pRender )
  {
    m_pRender->iMaterialSize = 1;
    if( m_pRender->iMaterialSize > 0 )
    {
      m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
      m_pRender->m_pMaterial[0].m_pszTextName = _NEW char[15 + 1];
      strcpy( m_pRender->m_pMaterial[0].m_pszTextName, "weaponribbon000" );
    }

    m_pRender->iRenderType = 4;
    m_pRender->m_dwCullMode = D3DCULL_NONE;
    
    m_pRender->m_dwAddressU = D3DTADDRESS_CLAMP;
    m_pRender->m_dwAddressV = D3DTADDRESS_CLAMP;

    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;

    // !!!To do... set correct rendermeshsize
    m_pRender->m_dwRenderMeshSize = 36;
    m_pRender->m_dwVertexSize = 0;
    m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[pRender->m_dwRenderMeshSize];
  }
}

//-----------------------------------------------------------------------------
// Name: ~WeaponRibbonParticleEmitter
// Desc: 
//-----------------------------------------------------------------------------
WeaponRibbonParticleEmitter::~WeaponRibbonParticleEmitter()
{
  SAFE_DELETE( m_pLerpInfo );
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: 
//-----------------------------------------------------------------------------
void WeaponRibbonParticleEmitter::FrameMove( float fElapsedTime )
{
  if( m_pRender == NULL )
  {
    return;
  }

  cParticle* pParticle;

  float fTotalTime = 0.2f * 4800.0f;

  if( m_bAddSegment )
  {
    if( m_pLerpInfo )
    {
		// 为了让刀光在frame rate不高时能看起来比较圆滑，在这次和前次拿到的transform作lerp的动作
		// lerp取样的数量为这次和上次的距离除以0.5
		// !!!To do...Optimize
      D3DXVECTOR3     vCurScale;
      D3DXQUATERNION  quatCurRotation;
      D3DXVECTOR3     vCurTranslation;

      D3DXMatrixDecompose( &vCurScale, &quatCurRotation, &vCurTranslation, &m_pLerpInfo->m_matCurTransform );

      pParticle = m_pFirstUsedParticle;
      if( pParticle )
      {
        cParticle* pNextParticle = pParticle->m_pNextParticle;
        float fLength = D3DXVec3Length( &( m_vMaxPos - m_pLerpInfo->m_vLastMaxPos ) );
        if( pNextParticle && ( fLength >= 0.5f ) )
        {
          D3DXMATRIXA16   matLerpTranslation;
          D3DXMATRIXA16   matLerpRotation;
          D3DXMATRIXA16   matLerpTransform;
          D3DXQUATERNION  quatLerpRotation;
          D3DXVECTOR3     vLerpTranslation;

          int iDivide = (int)( fLength / 0.5f ) + 1;
          if( iDivide > 12 )
          {
            iDivide = 12;
          }
          float fLerpStep = 1.0f / iDivide;
          for( int i = 1; i < iDivide; i++ )
          {
            float fLerp = i * fLerpStep;
            D3DXQuaternionSlerp( &quatLerpRotation, &m_pLerpInfo->m_quatLastRotation, &quatCurRotation, fLerp );
            D3DXVec3Lerp( &vLerpTranslation, &m_pLerpInfo->m_vLastTranslation, &vCurTranslation, fLerp );
            D3DXMatrixRotationQuaternion( &matLerpRotation, &quatLerpRotation );
            D3DXMatrixTranslation( &matLerpTranslation, vLerpTranslation.x, vLerpTranslation.y, vLerpTranslation.z );
            matLerpTransform = matLerpRotation * matLerpTranslation;

            float fLifeTime = fTotalTime * fLerp + pNextParticle->m_fLifeTime * ( 1.0f - fLerp );

            pParticle = m_pParticlePool->NewParticle();
            if( pParticle )
            {
              pParticle->m_fLifeTime = fLifeTime;
              D3DXVec3TransformCoord( &pParticle->m_vPos, &m_vOrigMinPos, &matLerpTransform );
              D3DXVec3TransformCoord( &pParticle->m_vDir, &m_vOrigMaxPos, &matLerpTransform );

              AddParticleFirst( pParticle );
            }
          }
        }
      }
      
      pParticle = m_pParticlePool->NewParticle();
      if( pParticle )
      {
        pParticle->m_fLifeTime = fTotalTime;
        pParticle->m_vPos = m_vMinPos;
        pParticle->m_vDir = m_vMaxPos;

        AddParticleFirst( pParticle );

        m_pLerpInfo->m_vLastMaxPos = m_vMaxPos;
        m_pLerpInfo->m_vLastScale = vCurScale;
        m_pLerpInfo->m_quatLastRotation = quatCurRotation;
        m_pLerpInfo->m_vLastTranslation = vCurTranslation;
      }
    }
    else
    {
      pParticle = m_pParticlePool->NewParticle();
      if( pParticle )
      {
        pParticle->m_fLifeTime = fTotalTime;
        pParticle->m_vPos = m_vMinPos;
        pParticle->m_vDir = m_vMaxPos;

        AddParticleFirst( pParticle );
      }
    }
  } // if( m_bAddSegment )
  
  DWORD dwTotalCount = 0;
  bool bErase = false;
  DWORD dwMaxCount = m_pRender->m_dwRenderMeshSize >> 1;
  pParticle = m_pFirstUsedParticle;
  while( pParticle )
  {
    cParticle* pNextParticle = pParticle->m_pNextParticle;

    if( !bErase )
    {
      dwTotalCount++;

      if( dwTotalCount > dwMaxCount )
      {
        bErase = true;
        FreeParticle( pParticle );
      }
      else
      {
        pParticle->m_fLifeTime -= fElapsedTime;
        if( pParticle->m_fLifeTime < 0.0f )
        {
          pParticle->m_fLifeTime = 0.0f;
          bErase = true;
          FreeParticle( pParticle );
        }
      }
    }
    else
    {
      FreeParticle( pParticle );
    }

    pParticle = pNextParticle;
  }

  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
  if( pRenderMesh == NULL )
  {
    return;
  }

  DWORD dwVertexSize = 0;
  float fTU;
  pParticle = m_pFirstUsedParticle;
  dwTotalCount = 0;
  while( pParticle )
  {
    dwTotalCount++;
    if( dwTotalCount * 2 > m_pRender->m_dwRenderMeshSize )
    {
      break;
    }

    fTU = 1.0f - (float)dwTotalCount / dwMaxCount;

    D3DCOLOR color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, pParticle->m_fLifeTime / fTotalTime );
    pRenderMesh[dwVertexSize].p = pParticle->m_vPos;
    pRenderMesh[dwVertexSize].tu = fTU;
    pRenderMesh[dwVertexSize].tv = 0.0f;
    pRenderMesh[dwVertexSize].color = color;
    dwVertexSize++;
    pRenderMesh[dwVertexSize].p = pParticle->m_vDir;
    pRenderMesh[dwVertexSize].tu = fTU;
    pRenderMesh[dwVertexSize].tv = 1.0f;
    pRenderMesh[dwVertexSize].color = color;
    dwVertexSize++;

    pParticle = pParticle->m_pNextParticle;
  }

  m_pRender->m_dwVertexSize = dwVertexSize;
  m_pRender->bEnabled = true;
}

//-----------------------------------------------------------------------------
// Name: DisableFrameMove
// Desc: 
//-----------------------------------------------------------------------------
void WeaponRibbonParticleEmitter::DisableFrameMove( float fElapsedTime )
{
  if( m_pRender == NULL )
  {
    return;
  }

  DWORD dwTotalCount = 0;
  bool bErase = false;
  DWORD dwMaxCount = m_pRender->m_dwRenderMeshSize >> 1;
  cParticle* pParticle = m_pFirstUsedParticle;
  while( pParticle )
  {
    cParticle* pNextParticle = pParticle->m_pNextParticle;

    if( !bErase )
    {
      dwTotalCount++;

      if( dwTotalCount > dwMaxCount )
      {
        bErase = true;
        FreeParticle( pParticle );
      }
      else
      {
        pParticle->m_fLifeTime -= fElapsedTime;
        if( pParticle->m_fLifeTime < 0.0f )
        {
          pParticle->m_fLifeTime = 0.0f;
          bErase = true;
          FreeParticle( pParticle );
        }
      }
    }
    else
    {
      FreeParticle( pParticle );
    }

    pParticle = pNextParticle;
  }
}

//-----------------------------------------------------------------------------
// Name: ApplyTransformMatrix
// Desc: 
//-----------------------------------------------------------------------------
void WeaponRibbonParticleEmitter::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
{
  if( pTransform == NULL )
  {
    m_bAddSegment = false;
  }
  else
  {
    if( m_pLerpInfo )
    {
      m_pLerpInfo->m_matCurTransform = *pTransform;
    }

    D3DXVec3TransformCoord( &m_vMinPos, &m_vOrigMinPos, pTransform );
    D3DXVec3TransformCoord( &m_vMaxPos, &m_vOrigMaxPos, pTransform );
    m_bAddSegment = true;
  }
}
}