//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/Billboardparticleemitter.h"

namespace KOK
{
using namespace IO;
using namespace Memory;

BillboardParticleEmitter::BillboardParticleEmitter( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer, DWORD dwBillboardType )
:ParticleEmitter( pParticlePool ), m_fWidth(0.0f), m_dwBillboardType(dwBillboardType),
 m_fCurAnimTime(0.0f)
{
  if( pSubMeshSerializer == NULL )
  {
    return;
  }

  SetLinkName( pSubMeshSerializer->getSubMeshName() );

  MDLVTX* pVertex = pSubMeshSerializer->getSubMeshVerticesBuffer();
  for( int i = 0; i < 3; i++ )
  {
    m_vCorner[i] = pVertex[i].p;
  }

  DWORD dwRenderAlphaBlendType = 0;
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
    cMaterialAnimator* pMaterialAnimator;
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
        m_pRender->m_pMaterial[i].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
        m_pRender->m_pMaterial[i].SetTextureAniRows( pMaterial->GetTextureAniRows() );
        m_pRender->m_pMaterial[i].SetTextureAniCols( pMaterial->GetTextureAniCols() );
        m_pRender->m_pMaterial[i].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );

        if( i == 0 )
        {
          dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
        }
      }

      pMaterialAnimator = pSubMeshSerializer->GetMaterialAnimator( i );
      if( pMaterialAnimator )
      {
        m_pRender->m_pMaterial[i].SetMaterialAnimator( pMaterialAnimator );

        // 070212 cyhsieh extended material animation texture address
        if( m_pRender->m_pMaterial[i].GetTextureTransformMatrix() )
        {
          m_pRender->m_pMaterial[i].GetAnimatorAddress( m_pRender->m_dwAddressU, m_pRender->m_dwAddressV );
        }
      }
    }
  }

  m_pRender->iRenderType = 4;
  m_pRender->m_dwCullMode = D3DCULL_NONE;

  switch( dwRenderAlphaBlendType )
  {
  case 0:   // Opaque
    m_pRender->m_bAlphaBlendenable = false;
    m_pRender->m_bZWriteenable = true;
    break;
  case 1:   // Additive
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
    m_pRender->m_dwDestBlend = D3DBLEND_ONE;
    m_pRender->m_bZWriteenable = false;
    break;
  case 2:   // Alpha
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
    m_pRender->m_bZWriteenable = false;
    break;
  case 3:   // Modulate
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
    m_pRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
    m_pRender->m_bZWriteenable = false;
    break;
  case 4:   // Modulate Inv
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
    m_pRender->m_bZWriteenable = false;
    break;
  }

  m_pRender->m_dwRenderMeshSize = 4;
  m_pRender->m_dwVertexSize = 0;
  m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[pRender->m_dwRenderMeshSize];
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: 
//-----------------------------------------------------------------------------
void BillboardParticleEmitter::FrameMove( float fElapsedTime )
{
  if( m_pRender == NULL )
  {
    return;
  }

  float fFogAlphaFactor = ComputeFogAlphaFactor();
  if( fFogAlphaFactor <= 0.0f )
  {
    return;
  }

  cParticle* pParticle = m_pFirstUsedParticle;
  if( pParticle == NULL )
  {
    pParticle = m_pParticlePool->NewParticle();
    if( pParticle )
    {
      // initialize
      pParticle->m_dwTextureAniIndex = 0;
      pParticle->m_fTextureAniTime = 0.0f;
      pParticle->m_fLifeTime = 0;
      AddParticle( pParticle );
    }
    else
    {
      return;
    }
  }

  pParticle->m_vPos = m_vCurPosition;
  pParticle->m_fSize = m_fWidth;
  
  // 070628 cyhsieh Billboard Animation Time
  pParticle->m_fLifeTime = m_fCurAnimTime;  // 设定和Mesh的动画时间相

  EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;
  if( pRenderMesh == NULL )
  {
    return;
  }

  cMaterial* pMaterial = NULL;
  if( m_pRender->iMaterialSize > 0 )
  {
    pMaterial = &m_pRender->m_pMaterial[0];
  }

  const D3DXMATRIXA16& matView = ParticleServer::GetCameraViewMatrix();

  D3DXVECTOR3 vRight;
  D3DXVECTOR3 vUp;
  D3DXVECTOR3 vDir = D3DXVECTOR3( matView._13, matView._23, matView._33 );

  switch( m_dwBillboardType )
  {
  case 0:
    vRight = D3DXVECTOR3( matView._11, matView._21, matView._31 );
    vUp = D3DXVECTOR3( matView._12, matView._22, matView._32 );
    break;
  case 1:
    vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXVec3Cross( &vRight, &vUp, &vDir );
    break;
  }
  
  D3DXVECTOR3 vA = -vRight - vUp;
	D3DXVECTOR3 vB = -vRight + vUp;
	D3DXVECTOR3 vC =  vRight - vUp;
	D3DXVECTOR3 vD =  vRight + vUp;

  pRenderMesh[0].p = pParticle->m_vPos + vA * pParticle->m_fSize;
  pRenderMesh[1].p = pParticle->m_vPos + vB * pParticle->m_fSize;
  pRenderMesh[2].p = pParticle->m_vPos + vC * pParticle->m_fSize;
  pRenderMesh[3].p = pParticle->m_vPos + vD * pParticle->m_fSize;

  // set color
  D3DCOLOR color = 0xFFFFFF | ( ( (DWORD)( fFogAlphaFactor * 255.0f ) ) << 24 );
  if( pMaterial )
  {
    pMaterial->SetAniTime( pParticle->m_fLifeTime );
    const D3DXCOLOR* pColor = pMaterial->GetDiffuseOpacity();
    if( pColor )
    {
      color = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a * fFogAlphaFactor );
    }
  }
  pRenderMesh[0].color = color;
  pRenderMesh[1].color = color;
  pRenderMesh[2].color = color;
  pRenderMesh[3].color = color;

  // 070205 cyhsieh texture animation row & col
  pParticle->m_fTextureAniTime += fElapsedTime;
  if( pParticle->m_fTextureAniTime > 160.0f )
  {
    pParticle->m_fTextureAniTime -= 160.0f;
    DWORD dwRepeat = 0;
    if( pMaterial && ( dwRepeat = pMaterial->GetTextureAniRepeat() ) > 0 )
    {
      DWORD dwTextureAniRows = pMaterial->GetTextureAniRows();
      DWORD dwTextureAniCols = pMaterial->GetTextureAniCols();
      DWORD dwRowCols = dwTextureAniRows * dwTextureAniCols;

      DWORD dwIndex = pParticle->m_dwTextureAniIndex % ( dwRowCols );
      float fUDiff = 1.0f / dwTextureAniRows;
      float fVDiff = 1.0f / dwTextureAniCols;

      float fStartU = ( dwIndex % dwTextureAniRows ) * fUDiff;
      float fStartV = ( dwIndex % dwTextureAniCols ) * fVDiff;

      pRenderMesh[0].tu = fStartU;
      pRenderMesh[0].tv = fStartV + fVDiff;
      pRenderMesh[1].tu = fStartU;
      pRenderMesh[1].tv = fStartV;
      pRenderMesh[2].tu = fStartU + fUDiff;
      pRenderMesh[2].tv = fStartV + fVDiff;
      pRenderMesh[3].tu = fStartU + fUDiff;
      pRenderMesh[3].tv = fStartV;

      if( dwRepeat == 999 || pParticle->m_dwTextureAniIndex < dwRepeat * dwRowCols - 1 )
      {
        pParticle->m_dwTextureAniIndex++;
      }
    }
    else
    {
      pRenderMesh[0].tu = 0.0f;
      pRenderMesh[0].tv = 1.0f;
      pRenderMesh[1].tu = 0.0f;
      pRenderMesh[1].tv = 0.0f;
      pRenderMesh[2].tu = 1.0f;
      pRenderMesh[2].tv = 1.0f;
      pRenderMesh[3].tu = 1.0f;
      pRenderMesh[3].tv = 0.0f;
    }
  }

  m_pRender->m_dwVertexSize = 4;
  m_pRender->bEnabled = true;
}

//-----------------------------------------------------------------------------
// Name: ApplyTransformMatrix
// Desc: 
//-----------------------------------------------------------------------------
void BillboardParticleEmitter::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
{
  D3DXVECTOR3 vTransformed[3];

  for( int i = 0; i < 3; i++ )
  {
    D3DXVec3TransformCoord( &vTransformed[i], &m_vCorner[i], pTransform );
  }

  D3DXVECTOR3 vDiff10 = vTransformed[1] - vTransformed[0];
  D3DXVECTOR3 vDiff20 = vTransformed[2] - vTransformed[0];

  m_vCurPosition = ( vTransformed[1] + vTransformed[2] ) * 0.5f;

  m_fWidth = D3DXVec3Length( &( vTransformed[1] - vTransformed[0] ) );
}
}