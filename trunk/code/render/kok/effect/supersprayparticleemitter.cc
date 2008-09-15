//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/particledata.h"
#include "kok/effect/supersprayparticleemitter.h"
#include "math/matrix44.h"

namespace KOK
{
using namespace IO;
using namespace Memory;
using namespace Math;

// particle superspray
SuperSprayParticleEmitter::SuperSprayParticleEmitter( cParticlePool* pParticlePool, SuperSprayEmitterData* pEmitDataSuperSpray )
:ParticleEmitter( pParticlePool ),
 m_fTimeSinceLastEmit(0.0f), m_fCurAnimTime(0.0f), m_bEmitParticles(true),
 m_vEmitDirRight(1.0f, 0.0f, 0.0f), m_vEmitDirForward(0.0f, 0.0f, 1.0f), m_vEmitForceDir(0.0f, 0.0f, 0.0f)
{
  if( pEmitDataSuperSpray )
  {
		m_sSuperSprayInfo = pEmitDataSuperSpray->GetSuperSprayInfo();
		SetLinkName( pEmitDataSuperSpray->GetLinkName() );

		DWORD dwRenderAlphaBlendType = 0;
		//c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
		//m_pRender = (c3dsMaxParticleRender*)pRender->Create();
		//if( m_pRender )
		//{
		//  m_pRender->iMaterialSize = pEmitDataSuperSpray->GetMaterialSize();
		//  if( m_pRender->iMaterialSize > 0 )
		//  {
		//    cMaterial* pMaterial;
		//    cMaterialAnimator*  pMaterialAnimator;
		//    m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
		//    for( int i = 0; i < m_pRender->iMaterialSize; i++ )
		//    {
		//      pMaterial = pEmitDataSuperSpray->GetMaterial( i );
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

		//        if( i == 0 )
		//        {
		//          dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
		//        }
		//      }
		//      pMaterialAnimator = pEmitDataSuperSpray->GetMaterialAnimator( i );
		//      if( pMaterialAnimator )
		//      {
		//        m_pRender->m_pMaterial[i].SetMaterialAnimator( pMaterialAnimator );

		//        // 070212 cyhsieh extended material animation texture address
		//        if( m_pRender->m_pMaterial[i].GetTextureTransformMatrix() )
		//        {
		//          m_pRender->m_pMaterial[i].GetAnimatorAddress( m_pRender->m_dwAddressU, m_pRender->m_dwAddressV );
		//        }
		//      }
		//    }
		//  }
	
		cMaterial* pMaterial = pEmitDataSuperSpray->GetMaterial( 0 );
		dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();

		this->isAlphaMode = true;
		switch( dwRenderAlphaBlendType )
		{
		case 0:   // Opaque
			this->isAlphaMode = false;
			break;
		case 1:   // Additive
			this->srcBlend = Blend_SrcAlpha;
			this->destBlend = Blend_One;
			break;
		case 2:   // Alpha
			this->srcBlend = Blend_SrcAlpha;
			this->destBlend = Blend_InvSrcAlpha;
			break;
		case 3:   // Modulate
			this->srcBlend = Blend_Zero;
			this->destBlend = Blend_SrcColor;
			break;
		case 4:   // Modulate Inv
			this->srcBlend = Blend_Zero;
			this->destBlend = Blend_InvSrcColor;
			break;
		}

		//int iPredictVertexSize = m_sSuperSprayInfo.m_dwBirthRate * ( m_sSuperSprayInfo.m_dwLife / 160 ) * 4;
		//m_pRender->m_dwRenderMeshSize = ( iPredictVertexSize + 15 ) & ~0xF;  // 16™∫≠øº∆
    //}
  }
}

void 
SuperSprayParticleEmitter::FrameMove( float fElapsedTime )
{
  float fFogAlphaFactor = ComputeFogAlphaFactor();
  if( fFogAlphaFactor <= 0.0f )
  {
    return;
  }

  //  Particle Emit Time
  m_bEmitParticles = ( ( m_fCurAnimTime >= m_sSuperSprayInfo.m_dwEmitStart ) &&
                       ( m_fCurAnimTime <= m_sSuperSprayInfo.m_dwEmitStop ) );

  if( m_bEmitParticles )
  {
    m_fTimeSinceLastEmit += fElapsedTime;
    if( m_fTimeSinceLastEmit > 160.0f )
    {
      for( unsigned int i = 0; i < m_sSuperSprayInfo.m_dwBirthRate; i++ )
      {
        cParticle* pParticle = m_pParticlePool->NewParticle();
        if( pParticle )
        {
          pParticle->m_vPos = m_vCurPosition;
                    
          float fRandomLifeTime = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
          float fRandomSize = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
          
          // 070731 cyhsieh particle initial rotation
          if( m_sSuperSprayInfo.m_fSpinPhaseDeg == 0 )
          {
            pParticle->m_fAngle = 0.0f;
          }
          else
          {
            float fRandom = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
            pParticle->m_fAngle = m_sSuperSprayInfo.m_fSpinPhaseDeg * ( 1.0f + fRandom * m_sSuperSprayInfo.m_fSpinPhaseDegVariation );
          }
          
          pParticle->m_fSize = m_sSuperSprayInfo.m_fSize * ( 1.0f + m_sSuperSprayInfo.m_fSizeVariation * fRandomSize );
          pParticle->m_fTotalLifeTime = (float)m_sSuperSprayInfo.m_dwLife + m_sSuperSprayInfo.m_dwLifeVariation * fRandomLifeTime;
          pParticle->m_fLifeTime = pParticle->m_fTotalLifeTime;
          pParticle->m_vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

          D3DXVECTOR3 vRandomDir;
          float fRandomV = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
          float fRandomH = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
          float fThetaV = m_sSuperSprayInfo.m_fOffAxisDeg + m_sSuperSprayInfo.m_fOffAxisSpreadDeg * fRandomV;
          float fThetaH = m_sSuperSprayInfo.m_fOffPlaneDeg + m_sSuperSprayInfo.m_fOffPlaneSpreadDeg * fRandomH;
          float fSinV = sin( fThetaV );
          float fCosV = cos( fThetaV );
          vRandomDir.z = fSinV * sin( fThetaH );
          vRandomDir.x = fSinV * cos( fThetaH );
          vRandomDir.y = fCosV;
          D3DXVec3TransformNormal( &vRandomDir, &vRandomDir, &m_matTransForm );
          
          float fRandomSpeed = ( (float)rand() / RAND_MAX ) * 2.0f - 1.0f;
          fRandomSpeed = m_sSuperSprayInfo.m_fMoveSpeed * ( 1.0f + m_sSuperSprayInfo.m_fMoveSpeedVariation * fRandomSpeed );

          // 070215 cyhsieh SuperSpray Speed
          pParticle->m_vDir = vRandomDir * fRandomSpeed / 160.0f;
          pParticle->m_dwTextureAniIndex = 0;
          pParticle->m_fTextureAniTime = 0.0f;
          AddParticle( pParticle );
        }
      }

      m_fTimeSinceLastEmit = 0.0f;
    }
  }

  float fAngleChangedPS;
  if( m_sSuperSprayInfo.m_dwSpinTime == 0 )
  {
    fAngleChangedPS = 0.0f;
  }
  else
  {
    fAngleChangedPS = ( 2.0f * D3DX_PI ) / m_sSuperSprayInfo.m_dwSpinTime;
  }

  cParticle* pParticle = m_pFirstUsedParticle;
  while( pParticle )
  {
    cParticle* pNext = pParticle->m_pNextParticle;

    if( ( pParticle->m_fLifeTime -= fElapsedTime ) >= 0.0f )
    {
      pParticle->m_vPos += pParticle->m_vDir * fElapsedTime;
      pParticle->m_fAngle += fAngleChangedPS * fElapsedTime;
      
      if( m_sSuperSprayInfo.m_dwLife - pParticle->m_fLifeTime < m_sSuperSprayInfo.m_dwGrowTime )
      {
        pParticle->m_fSize = ( ( ( m_sSuperSprayInfo.m_dwLife - pParticle->m_fLifeTime ) / m_sSuperSprayInfo.m_dwGrowTime ) * 0.9f + 0.1f ) * m_sSuperSprayInfo.m_fSize;
      }
      if( pParticle->m_fLifeTime < m_sSuperSprayInfo.m_dwFadeTime )
      {
        pParticle->m_fSize = ( ( pParticle->m_fLifeTime / m_sSuperSprayInfo.m_dwFadeTime ) * 0.9f + 0.1f ) * m_sSuperSprayInfo.m_fSize;
      }
    }
    else
    {
      FreeParticle( pParticle );
    }

    pParticle = pNext;
  }

}

void 
SuperSprayParticleEmitter::DisableFrameMove( float fElapsedTime )
{
  cParticle* pParticle = m_pFirstUsedParticle;
  while( pParticle )
  {
    cParticle* pNext = pParticle->m_pNextParticle;

    if( ( pParticle->m_fLifeTime -= fElapsedTime ) >= 0.0f )
    {
      pParticle->m_vPos += pParticle->m_vDir * fElapsedTime;
    }
    else
    {
      FreeParticle( pParticle );
    }

    pParticle = pNext;
  }
}

void 
SuperSprayParticleEmitter::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
{
  m_vCurPosition.x = pTransform->_41;
  m_vCurPosition.y = pTransform->_42;
  m_vCurPosition.z = pTransform->_43;

  D3DXVECTOR3 vRight( 1.0f, 0.0f, 0.0f );
  D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
  D3DXVECTOR3 vForward( 0.0f, 0.0f, 1.0f );
  D3DXVec3TransformNormal( &m_vEmitDirRight, &vRight, pTransform );
  D3DXVec3TransformNormal( &m_vEmitDirForward, &vForward, pTransform );
  D3DXVec3TransformNormal( &m_vEmitForceDir, &vUp, pTransform );

  // spray≈Á…‰∑ΩœÚ
  m_matTransForm = *pTransform;
}

int 
SuperSprayParticleEmitter::RenderParticles(float* dstVertices, int maxVertices)
{
	if (dstVertices == NULL || maxVertices <= 0)
		return 0;

	ParticleFVF* pRenderMesh = (ParticleFVF*)dstVertices;

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

	const matrix44& matView = TransformDevice::Instance()->GetViewTransform();
	float4 r0 = matView.getrow0();
	float4 r1 = matView.getrow1();
	float4 r2 = matView.getrow2();

	//const D3DXMATRIXA16& matView = c3dsMaxParticleManager::GetCameraViewMatrix();

	D3DXVECTOR3 vRight = D3DXVECTOR3( r0.x(), r1.x(), r2.x() );
	D3DXVECTOR3 vUp = D3DXVECTOR3( r0.y(), r1.y(), r2.y() );
	D3DXVECTOR3 vDir = D3DXVECTOR3( r0.z(), r1.z(), r2.z() );
	D3DXVECTOR3 vA = -vRight - vUp;
	D3DXVECTOR3 vB = -vRight + vUp;
	D3DXVECTOR3 vC =  vRight - vUp;
	D3DXVECTOR3 vD =  vRight + vUp;

	DWORD dwVertexSize = 0;
	cParticle* pParticle = m_pFirstUsedParticle;
	DWORD dwTotalCount = 0;
	while( pParticle )
	{
		dwTotalCount++;
		if( dwTotalCount * 4 > m_pRender->m_dwRenderMeshSize )
		{
			break;
		}

		if( pParticle->m_fAngle != 0 )
		{
			D3DXMATRIXA16 matRotate;
			D3DXMatrixRotationAxis( &matRotate, &vDir, pParticle->m_fAngle );
			D3DXVECTOR3 vRotateRight, vRotateUp;
			D3DXVec3TransformNormal( &vRotateRight, &vRight, &matRotate );
			D3DXVec3TransformNormal( &vRotateUp, &vUp, &matRotate );
			D3DXVECTOR3 vRotateA = -vRotateRight - vRotateUp;
			D3DXVECTOR3 vRotateB = -vRotateRight + vRotateUp;
			D3DXVECTOR3 vRotateC =  vRotateRight - vRotateUp;
			D3DXVECTOR3 vRotateD =  vRotateRight + vRotateUp;
			pRenderMesh[dwVertexSize + 0].p = pParticle->m_vPos + vRotateA * pParticle->m_fSize;
			pRenderMesh[dwVertexSize + 1].p = pParticle->m_vPos + vRotateB * pParticle->m_fSize;
			pRenderMesh[dwVertexSize + 2].p = pParticle->m_vPos + vRotateC * pParticle->m_fSize;
			pRenderMesh[dwVertexSize + 3].p = pParticle->m_vPos + vRotateD * pParticle->m_fSize;
		}
		else
		{
			pRenderMesh[dwVertexSize + 0].p = pParticle->m_vPos + vA * pParticle->m_fSize;
			pRenderMesh[dwVertexSize + 1].p = pParticle->m_vPos + vB * pParticle->m_fSize;
			pRenderMesh[dwVertexSize + 2].p = pParticle->m_vPos + vC * pParticle->m_fSize;
			pRenderMesh[dwVertexSize + 3].p = pParticle->m_vPos + vD * pParticle->m_fSize;
		}

		D3DCOLOR color = 0xFFFFFF | ( ( (DWORD)( fFogAlphaFactor * 255.0f ) ) << 24 );
		if( pMaterial )
		{
			pMaterial->SetAniTime( pParticle->m_fTotalLifeTime - pParticle->m_fLifeTime );
			const D3DXCOLOR* pColor = pMaterial->GetDiffuseOpacity();
			if( pColor )
			{
				color = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a * fFogAlphaFactor );
			}
		}
		pRenderMesh[dwVertexSize + 0].color = color;
		pRenderMesh[dwVertexSize + 1].color = color;
		pRenderMesh[dwVertexSize + 2].color = color;
		pRenderMesh[dwVertexSize + 3].color = color;

		// texture animation row & col
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

				pRenderMesh[dwVertexSize + 0].tu = fStartU;
				pRenderMesh[dwVertexSize + 0].tv = fStartV + fVDiff;
				pRenderMesh[dwVertexSize + 1].tu = fStartU;
				pRenderMesh[dwVertexSize + 1].tv = fStartV;
				pRenderMesh[dwVertexSize + 2].tu = fStartU + fUDiff;
				pRenderMesh[dwVertexSize + 2].tv = fStartV + fVDiff;
				pRenderMesh[dwVertexSize + 3].tu = fStartU + fUDiff;
				pRenderMesh[dwVertexSize + 3].tv = fStartV;

				if( dwRepeat == 999 || pParticle->m_dwTextureAniIndex < dwRepeat * dwRowCols - 1 )
				{
					pParticle->m_dwTextureAniIndex++;
				}
			}
			else
			{
				pRenderMesh[dwVertexSize + 0].tu = 0.0f;
				pRenderMesh[dwVertexSize + 0].tv = 1.0f;
				pRenderMesh[dwVertexSize + 1].tu = 0.0f;
				pRenderMesh[dwVertexSize + 1].tv = 0.0f;
				pRenderMesh[dwVertexSize + 2].tu = 1.0f;
				pRenderMesh[dwVertexSize + 2].tv = 1.0f;
				pRenderMesh[dwVertexSize + 3].tu = 1.0f;
				pRenderMesh[dwVertexSize + 3].tv = 0.0f;
			}
		}


		dwVertexSize += 4;

		pParticle = pParticle->m_pNextParticle;
	}

	m_pRender->m_dwVertexSize = dwVertexSize;
	m_pRender->bEnabled = true;
}

}