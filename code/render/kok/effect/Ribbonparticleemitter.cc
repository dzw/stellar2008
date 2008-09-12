//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/Ribbonparticleemitter.h"

namespace KOK
{
using namespace IO;
using namespace Memory;

RibbonParticleEmitter::RibbonParticleEmitter( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer )
:ParticleEmitter( pParticlePool ),
 m_fAddSegmentLength(1.0f), m_fMaxRibbonLength(10.0f), m_fRibbonWidth(0.5f)
{
  if( pSubMeshSerializer == NULL )
  {
    return;
  }
  
  SetLinkName( pSubMeshSerializer->getSubMeshName() );

  m_fRibbonWidth = pSubMeshSerializer->GetRibbonWidth();
  m_fMaxRibbonLength = pSubMeshSerializer->GetRibbonLength();

  m_fAddSegmentLength = m_fRibbonWidth;

  c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
  if( pRender == NULL )
  {
    return;
  }

  m_pRender = (c3dsMaxParticleRender*)pRender->Create();
  m_pRender->iMaterialSize = pSubMeshSerializer->getSubMeshMaterialSize();
  if( m_pRender->iMaterialSize > 0 )
  {
    cMaterial* pMaterial;
    m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
    for( int i = 0; i < m_pRender->iMaterialSize; i++ )
    {
      pMaterial = pSubMeshSerializer->getSubMeshMaterial( i );
      if( pMaterial )
      {
        m_pRender->m_pMaterial[i].m_iNameSize = pMaterial->m_iNameSize;
        if( pMaterial->m_iNameSize > 0 )
        {
          m_pRender->m_pMaterial[i].m_pszTextName = _NEW char[pMaterial->m_iNameSize + 1];
          if( m_pRender->m_pMaterial[i].m_pszTextName )
          {
            strcpy( m_pRender->m_pMaterial[i].m_pszTextName, pMaterial->m_pszTextName );
          }
        }
      }
    }
  }

  m_pRender->iRenderType = 4;
  m_pRender->m_dwCullMode = D3DCULL_NONE;
  m_pRender->m_dwColorArg1 = D3DTA_TEXTURE;
  m_pRender->m_dwColorOp = D3DTOP_SELECTARG1;

  // !!!To do... set correct rendermeshsize
  m_pRender->m_dwRenderMeshSize = 128;
  m_pRender->m_dwVertexSize = 0;
  m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[pRender->m_dwRenderMeshSize];
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: 
//-----------------------------------------------------------------------------
void RibbonParticleEmitter::FrameMove( float fElapsedTime )
{
  if( m_pRender == NULL )
  {
    return;
  }

  cParticle* pParticle;
  
  float fDiffLength = D3DXVec3Length( &( m_vCurPosition - m_vLastPos ) );

  pParticle = m_pFirstUsedParticle;
  if( pParticle )
  {
    if( ( pParticle->m_fSize + fDiffLength ) > m_fAddSegmentLength )
    {
      pParticle = m_pParticlePool->NewParticle();
      if( pParticle )
      {
        pParticle->m_vPos = m_vCurPosition;
        pParticle->m_vDir = m_vDirUp;
        pParticle->m_fSize = fDiffLength;
        pParticle->m_fLifeTime = 1.0f;
        AddParticleFirst( pParticle );
      }
    }
    else
    {
      pParticle->m_vPos = m_vCurPosition;
      pParticle->m_vDir = m_vDirUp;
      pParticle->m_fSize += fDiffLength;
      pParticle->m_fLifeTime = 1.0f;
    }
  }

  bool bFade = false;
  float fOneOverElapsedTime = 0.0f;
  if( fDiffLength < m_fAddSegmentLength * 0.5f )
  {
    bFade = true;
    fOneOverElapsedTime = fElapsedTime / 4800.0f;
  }

  bool bErase = false;
  float fLength = 0.0f;
  pParticle = m_pFirstUsedParticle;
  while( pParticle )
  {
    cParticle* pNextParticle = pParticle->m_pNextParticle;

    if( !bErase )
    {
      fLength += pParticle->m_fSize;

      if( bFade )
      {
        if( pParticle->m_fLifeTime > 0.0f )
        {
          pParticle->m_fLifeTime -= fOneOverElapsedTime;
          if( pParticle->m_fLifeTime < 0.0f )
          {
            pParticle->m_fLifeTime = 0.0f;
          }
        }
      }

      if( fLength > m_fMaxRibbonLength )
      {
        pParticle->m_fSize = fLength - m_fMaxRibbonLength;
        bErase = true;
      }
    }
    else
    {
      FreeParticle( pParticle );
    }

    pParticle = pNextParticle;
  }

  m_vLastPos = m_vCurPosition;

  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
  if( pRenderMesh == NULL )
  {
    return;
  }

  DWORD dwVertexSize = 0;
  float fTU;
  fLength = 0.0f;
  pParticle = m_pFirstUsedParticle;
  DWORD dwTotalCount = 0;
  while( pParticle )
  {
    dwTotalCount++;
    if( dwTotalCount * 2 > m_pRender->m_dwRenderMeshSize )
    {
      break;
    }

    fTU = 1.0f - fLength / m_fMaxRibbonLength;

    D3DCOLOR color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, pParticle->m_fLifeTime );

    pRenderMesh[dwVertexSize].p = pParticle->m_vPos + pParticle->m_vDir * m_fRibbonWidth;
    pRenderMesh[dwVertexSize].tu = fTU;
    pRenderMesh[dwVertexSize].tv = 0.0f;
    pRenderMesh[dwVertexSize].color = color;
    dwVertexSize++;
    pRenderMesh[dwVertexSize].p = pParticle->m_vPos - pParticle->m_vDir * m_fRibbonWidth;
    pRenderMesh[dwVertexSize].tu = fTU;
    pRenderMesh[dwVertexSize].tv = 1.0f;
    pRenderMesh[dwVertexSize].color = color;
    dwVertexSize++;

    fLength += pParticle->m_fSize;

    pParticle = pParticle->m_pNextParticle;
  }

  m_pRender->m_dwVertexSize = dwVertexSize;
  m_pRender->bEnabled = true;
}

//-----------------------------------------------------------------------------
// Name: DisableFrameMove
// Desc: 
//-----------------------------------------------------------------------------
void RibbonParticleEmitter::DisableFrameMove( float fElapsedTime )
{
  if( m_pRender == NULL )
  {
    return;
  }

  bool bErase = false;
  float fLength = 0.0f;
  cParticle* pParticle = m_pFirstUsedParticle;
  while( pParticle )
  {
    cParticle* pNextParticle = pParticle->m_pNextParticle;

    if( !bErase )
    {
      fLength += pParticle->m_fSize;

      if( pParticle->m_fLifeTime > 0.0f )
      {
        pParticle->m_fLifeTime -= fElapsedTime;
        if( pParticle->m_fLifeTime < 0.0f )
        {
          pParticle->m_fLifeTime = 0.0f;
        }
      }

      if( fLength > m_fMaxRibbonLength )
      {
        pParticle->m_fSize = fLength - m_fMaxRibbonLength;
        bErase = true;
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
// Name: InitTransformMatrix
// Desc: 
//-----------------------------------------------------------------------------
void RibbonParticleEmitter::InitTransformMatrix( D3DXMATRIXA16* pTransform )
{
  m_vCurPosition.x = pTransform->_41;
  m_vCurPosition.y = pTransform->_42;
  m_vCurPosition.z = pTransform->_43;
  m_vLastPos = m_vCurPosition;

  if( m_pParticlePool )
  {
    cParticle* pParticle = m_pParticlePool->NewParticle();
    if( pParticle )
    {
      D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
      D3DXVec3TransformNormal( &m_vDirUp, &vUp, pTransform );

      pParticle->m_vPos = m_vCurPosition;
      pParticle->m_vDir = m_vDirUp;
      pParticle->m_fSize = 0.0f;
      pParticle->m_fLifeTime = 1.0f;

      AddParticle( pParticle );
    }
  }
}

//-----------------------------------------------------------------------------
// Name: ApplyTransformMatrix
// Desc: 
//-----------------------------------------------------------------------------
void RibbonParticleEmitter::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
{
  m_vCurPosition.x = pTransform->_41;
  m_vCurPosition.y = pTransform->_42;
  m_vCurPosition.z = pTransform->_43;

  D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
  D3DXVec3TransformNormal( &m_vDirUp, &vUp, pTransform );
}

}