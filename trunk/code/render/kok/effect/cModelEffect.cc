#include "stdafx.h"

#include "cModelEffect.h"

extern ID3DXMatrixStack *g_pStack;

// 070226 cyhsieh model effect table
cHashMap* cModelEffect::m_pEffectInfoMap = NULL;
// 080328 cyhsieh 天生特效
BTList* cModelEffect::m_pBornEffectList = NULL;

//---------------------------------------------------------------------------
// Name: cModelEffect
// Desc: 
//---------------------------------------------------------------------------
// 070205 cyhsieh model effect
cModelEffect::cModelEffect()
:m_pMeshModel(NULL), m_iRenderArraySize(0), m_pRenderArray(NULL),
 m_fCurrentAnimTime(0.0f), m_pForceRotation(NULL),
 m_dwParticleEffectSize(0), m_ppParticleEffects(NULL),
 m_byMagicLinkID(255), m_dwSpecialType(0),
 m_bEnable(true), m_bDead(false), m_bLoop(false), m_wEffectID(-1),
 m_pGuid(NULL),
 m_pMoveToInfo(NULL),
 m_pAudioManage(NULL),
 m_pNowAudio(NULL),
 // Antony 970625 為了錄影
 m_fMapX( 0.0f ),
 m_fMapZ( 0.0f )
 // Antony End
{
  D3DXMatrixIdentity( &m_matTransform );
}

//---------------------------------------------------------------------------
// Name: ~cModelEffect
// Desc: 
//---------------------------------------------------------------------------
cModelEffect::~cModelEffect()
{
  SAFE_RELEASE(m_pNowAudio); // 讓音效管理自己刪除

  // 070205 cyhsieh test effect target
  SAFE_DELETE( m_pMoveToInfo );

  // 070412 cyhsieh 長效特效
  SAFE_DELETE_ARRAY( m_pGuid );

  SAFE_DELETE( m_pForceRotation );

  if( m_pMeshModel )
  {
    m_pMeshModel->SetNoUse();
    m_pMeshModel = NULL;
  }

  SAFE_DELETE_ARRAY( m_pRenderArray );

  if( g_p3dsMaxParticleManager && m_dwParticleEffectSize > 0 )
  {
    for( unsigned int i = 0; i < m_dwParticleEffectSize; i++ )
    {
      g_p3dsMaxParticleManager->ReleaseParticleEffect( m_ppParticleEffects[i] );
    }
    SAFE_DELETE_ARRAY( m_ppParticleEffects );
  }
}

//---------------------------------------------------------------------------
// Name: LoadMesh
// Desc: 
//---------------------------------------------------------------------------
void cModelEffect::LoadMesh( char* pFileName )
{
  if( g_pEffectModelDB == NULL || m_pMeshModel )
  {
    return;
  }
  
  m_pMeshModel = g_pEffectModelDB->ForageID( pFileName, "Effect\\efm\\" );
  if( m_pMeshModel == NULL )
  {
    return;
  }

  // 070205 cyhsieh ribbon
  int iMeshParticleEffectCount = 0;
  cSubMeshSerializer* pSubMeshSerializer;
  int iSubMeshSerializersCount = m_pMeshModel->getSubMeshSerializersCount();
  for( int i = 0; i < iSubMeshSerializersCount; i++ )
  {
    pSubMeshSerializer = m_pMeshModel->getSubMeshSerializer( i );
    if( pSubMeshSerializer && pSubMeshSerializer->GetMeshType() > 0 )
    {
      iMeshParticleEffectCount++;
    }
  }
  
  if( g_p3dsMaxParticleManager && m_ppParticleEffects == NULL )
  {
    DWORD dwParticleEffectSize = m_pMeshModel->Get3dsMaxEmitDataSize();
    if( dwParticleEffectSize + iMeshParticleEffectCount > 0 )
    {
      m_dwParticleEffectSize = dwParticleEffectSize + iMeshParticleEffectCount;
      m_ppParticleEffects = _NEW cParticleEffectBase*[m_dwParticleEffectSize];
      c3dsMaxEmitDataBase* pEmitData;
	  unsigned int i = 0;
      for( i = 0; i < dwParticleEffectSize; i++ )
      {
        m_ppParticleEffects[i] = NULL;

        pEmitData = m_pMeshModel->Get3dsMaxEmitData( i );
        m_ppParticleEffects[i] = g_p3dsMaxParticleManager->CreateParticleEffectByEmitData( pEmitData );
      }

      // 070205 cyhsieh ribbon
      for( int j = 0; j < iSubMeshSerializersCount; j++ )
      {
        pSubMeshSerializer = m_pMeshModel->getSubMeshSerializer( j );
        if( pSubMeshSerializer && pSubMeshSerializer->GetMeshType() > 0 )
        {
          m_ppParticleEffects[i] = g_p3dsMaxParticleManager->CreateParticleEffectBySubMeshSerializer( pSubMeshSerializer );
          i++;
        }
      }
    }
  }

  if( m_pRenderArray == NULL )
  {
    m_iRenderArraySize = m_pMeshModel->getSubMeshSerializersCount();

    if( m_iRenderArraySize > 0 )
    {
      char szTexturePath[] = "Effect\\efm\\";
      m_pRenderArray = _NEW stEFFECT_RENDER_ARRAY[m_iRenderArraySize];

      for( int i = 0; i < m_iRenderArraySize; i++ )
      {
        // 070205 cyhsieh ribbon
        pSubMeshSerializer = m_pMeshModel->getSubMeshSerializer( i );
        if( pSubMeshSerializer->GetMeshType() > 0 )
        {
          continue;
        }

        cEffectRender* pRender = (cEffectRender*)g_mScene->RenderCreate( RENDER_C_EFFECT );

        // 070205 cyhsieh extended material property
        DWORD dwRenderAlphaBlendType = 0;
        const D3DXCOLOR* pColor = NULL;
        
			  m_pRenderArray[i].m_pRender = (cEffectRender*)pRender->Create();
        
        pRender->m_pRenderBuffer = g_pEffectModelDB->GetRenderBuffer();

        pRender->dRenderMeshSize = pSubMeshSerializer->getSubMeshVerticesBufferSize();
        pRender->dwVertexSize    = pSubMeshSerializer->getSubMeshVerticesBufferSize();
				if( pSubMeshSerializer )
				{
					if( pRender->dRenderMeshSize )
					{
						pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[pRender->dRenderMeshSize];

						for( int unsigned j = 0; j < pSubMeshSerializer->getSubMeshVerticesBufferSize(); j++ )
						{
							MDLVTX* pVertex = pSubMeshSerializer->getSubMeshVerticesBuffer( j );
							if( pVertex )
							{
								pRender->m_pRenderMesh[j].p  = pVertex->p;
								pRender->m_pRenderMesh[j].tu = pVertex->tu; 
								pRender->m_pRenderMesh[j].tv = pVertex->tv;   
								pRender->m_pRenderMesh[j].color = D3DCOLOR_ARGB( 255, 255, 255, 255 );
							}
						}
					}
					//--------------------------------------------------------------------------------------
					pRender->dRenderMeshIndexSize = pSubMeshSerializer->getSubMeshIndexBufferSize();
					pRender->dwIndexSize          = pSubMeshSerializer->getSubMeshIndexBufferSize();

					if( pRender->dRenderMeshIndexSize )
					{
						pRender->m_pRenderMeshIndex = _NEW WORD[pRender->dRenderMeshIndexSize];  

						memcpy( pRender->m_pRenderMeshIndex,
										pSubMeshSerializer->getSubMeshIndexBuffer(),
										sizeof(WORD) * pRender->dRenderMeshIndexSize );
					} 
					//--------------------------------------------------------------------------------------
					pRender->iAttribTableSize = pSubMeshSerializer->getSubMeshAttributeTableSize();
					SAFE_DELETE_ARRAY( pRender->m_pAttribTable );

					if( pRender->iAttribTableSize )
					{
						pRender->m_pAttribTable = _NEW D3DXATTRIBUTERANGE[pRender->iAttribTableSize];

						memcpy( pRender->m_pAttribTable, pSubMeshSerializer->getSubMeshAttributeTable(),
							sizeof(D3DXATTRIBUTERANGE)*(pRender->iAttribTableSize) );

					}
					//--------------------------------------------------------------------------------------
					pRender->iMaterialSize = pSubMeshSerializer->getSubMeshMaterialSize();
					SAFE_DELETE_ARRAY( pRender->m_pMaterial );

					if( pRender->iMaterialSize )
					{
						pRender->m_pMaterial = _NEW cMaterial[pRender->iMaterialSize];   

						for( unsigned int j = 0; j < pSubMeshSerializer->getSubMeshMaterialSize(); j++ )
						{
							cMaterial* pMaterial = pSubMeshSerializer->getSubMeshMaterial( j );
							if( pMaterial )
							{
								pRender->m_pMaterial[j].m_iNameSize = pMaterial->m_iNameSize;

								if( g_pTextureDB && pRender->m_pMaterial[j].m_iNameSize )
								{ 
									SAFE_DELETE_ARRAY( pRender->m_pMaterial[j].m_pszTextName );

									int iNameSize = pMaterial->m_iNameSize + 1;
									pRender->m_pMaterial[j].m_pszTextName = _NEW char[iNameSize];
									strcpy( pRender->m_pMaterial[j].m_pszTextName, pMaterial->m_pszTextName );  

									pRender->m_pMaterial[j].m_pTexture = g_pTextureDB->QueryTexture(pRender->m_pMaterial[j].m_pszTextName,szTexturePath,EFFECT_STUFF_PATH,sTextureQueryParam( true, 0 ));

                  // 070205 cyhsieh extended material property
                  pRender->m_pMaterial[j].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
                  pRender->m_pMaterial[j].SetTextureAniRows( pMaterial->GetTextureAniRows() );
                  pRender->m_pMaterial[j].SetTextureAniCols( pMaterial->GetTextureAniCols() );
                  pRender->m_pMaterial[j].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );
                  if( j == 0 )
                  {
                    dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
                  }
								}
							}

              // 070205 cyhsieh extended material animation
              cMaterialAnimator* pMaterialAnimator = pSubMeshSerializer->GetMaterialAnimator( j );
              if( pMaterialAnimator )
              {
                pRender->m_pMaterial[j].SetMaterialAnimator( pMaterialAnimator );
                
                // 070212 cyhsieh extended material animation texture address
                if( pRender->m_pMaterial[j].GetTextureTransformMatrix() )
                {
                  pRender->m_pMaterial[j].GetAnimatorAddress( pRender->m_dwAddressU, pRender->m_dwAddressV );
                }
                
                if( j == 0 )
                {
                  pColor = pRender->m_pMaterial[j].GetDiffuseOpacity();
                }
              }
						}          
					} 
					//--------------------------------------------------------------------------------------
				}

        // 070205 cyhsieh extended material property
        if( pColor )
        {
          pRender->m_dwAlphaArg1 = D3DTA_TEXTURE;
          pRender->m_dwAlphaArg2 = D3DTA_TFACTOR;
          pRender->m_dwAlphaOp   = D3DTOP_MODULATE;
          pRender->m_dwColorArg1 = D3DTA_TEXTURE;
          pRender->m_dwColorArg2 = D3DTA_TFACTOR;
          pRender->m_dwColorOp   = D3DTOP_MODULATE;
        }
        else
        {
          pRender->m_dwAlphaArg1 = D3DTA_TEXTURE;
          pRender->m_dwAlphaOp   = D3DTOP_SELECTARG1;
          pRender->m_dwColorArg1 = D3DTA_TEXTURE;
          pRender->m_dwColorOp   = D3DTOP_SELECTARG1;
        }
        switch( dwRenderAlphaBlendType )
        {
        case 0:   // Opaque
          pRender->m_bAlphaBlendenable = false;
          pRender->m_bZWriteenable = true;
          break;
        case 1:   // Additive
          pRender->m_bAlphaBlendenable = true;
          pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
          pRender->m_dwDestBlend = D3DBLEND_ONE;
          pRender->m_bZWriteenable = false;
          break;
        case 2:   // Alpha
          pRender->m_bAlphaBlendenable = true;
          pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
          pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
          pRender->m_bZWriteenable = false;
          break;
        case 3:   // Modulate
          pRender->m_bAlphaBlendenable = true;
          pRender->m_dwSrcBlend = D3DBLEND_ZERO;
          pRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
          pRender->m_bZWriteenable = false;
          break;
        case 4:   // Modulate Inv
          pRender->m_bAlphaBlendenable = true;
          pRender->m_dwSrcBlend = D3DBLEND_ZERO;
          pRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
          pRender->m_bZWriteenable = false;
          break;
        }
        
      }

    } // end of if (m_pRenderArray == NULL)
  }
  
  // 070731 cyhsieh particle
  if( m_iRenderArraySize == 0 )
  {
    c3dsMaxEmitDataBase* pEmitData = m_pMeshModel->Get3dsMaxEmitData( 0 );
    if( pEmitData && pEmitData->GetDataType() == 1 )
    {
      sPCloudInfo cloudInfo = ((c3dsMaxEmitDataCloud*)pEmitData)->GetPCloudInfo();
      if( cloudInfo.m_dwFormation == 3 )
      {
        m_dwSpecialType = 1;
      }
    }
  }
}

//---------------------------------------------------------------------------
// Name: ComputeAnimation
// Desc: 
//---------------------------------------------------------------------------
void cModelEffect::ComputeAnimation( float fElapsedTime )
{
  if( m_pMeshModel && m_pMeshModel->m_pSkeletonHierarchyRoot )
  {
    if( m_pMeshModel->m_pAnimaRoot &&
        m_pMeshModel->m_AnimationActionInfoGroup.getAnimationActionInfoCount() > 0 )
    {
      m_fCurrentAnimTime += fElapsedTime;

      // 070213 cyhsieh Particle Emit Time
      cAnimationActionInfo* pActionInfo = m_pMeshModel->m_AnimationActionInfoGroup.getAnimationActionInfo( 0 );
      if( pActionInfo )
      {
        DWORD dwEndTime = pActionInfo->getAnimationActionEndTime();

        // 070226 cyhsieh model effect loop
        //m_fCurrentAnimTime = fmod( m_fCurrentAnimTime, (float)dwEndTime );
        if( m_bLoop )
        {
          m_fCurrentAnimTime = fmod( m_fCurrentAnimTime, (float)dwEndTime );
        }
        else if( m_fCurrentAnimTime >= (float)dwEndTime )
        {
          m_bDead = true;
        }
      }

      g_pStack->Push();
      g_pStack->LoadIdentity();

      sComputeAnimParam param( 0, -1, m_fCurrentAnimTime, 0.0f, 0.0f );
      m_pMeshModel->m_pSkeletonHierarchyRoot->computeSkeletonHierarchyMatrix( &m_pMeshModel->m_AnimationActionInfoGroup, param );

      g_pStack->Pop();
    }
  }
}

//---------------------------------------------------------------------------
// Name: UpdateAnimTimeOnly
// Desc: 當Disable時，僅更新動畫時間
//---------------------------------------------------------------------------
void cModelEffect::UpdateAnimTimeOnly( float fElapsedTime )
{
  if( m_pMeshModel && m_pMeshModel->m_pSkeletonHierarchyRoot )
  {
    if( m_pMeshModel->m_pAnimaRoot &&
        m_pMeshModel->m_AnimationActionInfoGroup.getAnimationActionInfoCount() > 0 )
    {
      m_fCurrentAnimTime += fElapsedTime;

      cAnimationActionInfo* pActionInfo = m_pMeshModel->m_AnimationActionInfoGroup.getAnimationActionInfo( 0 );
      if( pActionInfo )
      {
        DWORD dwEndTime = pActionInfo->getAnimationActionEndTime();

        if( m_bLoop == false )
        {
          if( m_fCurrentAnimTime >= (float)dwEndTime )
          {
            m_bDead = true;
          }
        }
      }
    }
  }
  if( m_pMoveToInfo )
  {
    m_pMoveToInfo->m_fLerp += m_pMoveToInfo->m_fLerpAddPS * fElapsedTime;
    if( m_pMoveToInfo->m_fLerp > 1.0f )
    {
      m_bDead = true;
    }
  }
}

//---------------------------------------------------------------------------
// Name: FrameMove
// Desc: 
//---------------------------------------------------------------------------
void cModelEffect::FrameMove( float fElapsedTime )
{
  float fAnimElapsedTime = fElapsedTime * 4800.0f;

  // 070917 cyhsieh 特效
  if( m_bEnable == false )
  {
    UpdateAnimTimeOnly( fAnimElapsedTime );
    return;
  }

  ComputeAnimation( fAnimElapsedTime );

  // 070205 cyhsieh test effect target
  D3DXMATRIXA16 matTest;
  matTest = m_matTransform;
  if( m_pMoveToInfo )
  {
    m_pMoveToInfo->m_fLerp += m_pMoveToInfo->m_fLerpAddPS * fElapsedTime;
    if( m_pMoveToInfo->m_fLerp > 1.0f )
    {
      switch( m_pMoveToInfo->m_byMoveType )
      {
      case FORTHBACK:       // 去回
      case FORTHBACK_HIDE:  // 去回(隱藏人物)
        {
          // 到目標位置後，改由目標位置往起始位置移動
          m_pMoveToInfo->m_fLerp -= 1.0f;
          m_pMoveToInfo->m_byMoveType = FORTHBACK_RETURN;
          D3DXVECTOR3 vTemp = m_pMoveToInfo->m_vInitialPos;
          m_pMoveToInfo->m_vInitialPos = m_pMoveToInfo->m_vTargetPos;
          m_pMoveToInfo->m_vTargetPos = vTemp;

          m_pMoveToInfo->m_fRotateY += D3DX_PI;
          D3DXMatrixRotationY( &m_matTransform, m_pMoveToInfo->m_fRotateY );
          m_pMoveToInfo->m_vRight = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
          D3DXVec3TransformNormal( &m_pMoveToInfo->m_vRight, &m_pMoveToInfo->m_vRight, &m_matTransform );
        }
        break;
      default:
        m_bDead = true;
        return;
      }
    }
    else
    {
      D3DXVECTOR3 vNewPos;
      switch( m_pMoveToInfo->m_byMoveType )
      {
      case NORMAL:
        {
          float fAdjustY = m_pMoveToInfo->m_fAdjustHeight * sinf( m_pMoveToInfo->m_fLerp * D3DX_PI );
          D3DXVec3Lerp( &vNewPos, &m_pMoveToInfo->m_vInitialPos, &m_pMoveToInfo->m_vTargetPos, m_pMoveToInfo->m_fLerp );
          vNewPos.y += fAdjustY;
        }
        break;
      case STRAIGHT:
      case FORTHBACK:
      case FORTHBACK_HIDE:
      case FORTHBACK_RETURN:
        D3DXVec3Lerp( &vNewPos, &m_pMoveToInfo->m_vInitialPos, &m_pMoveToInfo->m_vTargetPos, m_pMoveToInfo->m_fLerp );
        break;
      }

      D3DXVECTOR3 v1( vNewPos.x - m_pMoveToInfo->m_vCurrentPos.x, vNewPos.y - m_pMoveToInfo->m_vCurrentPos.y, vNewPos.z - m_pMoveToInfo->m_vCurrentPos.z );
      D3DXVECTOR3 v2( vNewPos.x - m_pMoveToInfo->m_vCurrentPos.x, 0.0f, vNewPos.z - m_pMoveToInfo->m_vCurrentPos.z );
      D3DXVec3Normalize( &v1, &v1 );
      D3DXVec3Normalize( &v2, &v2 );

      float fDot = D3DXVec3Dot( &v1, &v2 );
      float fAngle = acosf( fDot );
      if( vNewPos.y < m_pMoveToInfo->m_vCurrentPos.y )
      {
        fAngle = -fAngle;
      }

      m_pMoveToInfo->m_vCurrentPos.x = vNewPos.x;
      m_pMoveToInfo->m_vCurrentPos.y = vNewPos.y; 
      m_pMoveToInfo->m_vCurrentPos.z = vNewPos.z;

      D3DXMatrixRotationAxis( &matTest, &m_pMoveToInfo->m_vRight, fAngle );
      matTest = m_matTransform * matTest;
      matTest._41 = m_pMoveToInfo->m_vCurrentPos.x;
      matTest._42 = m_pMoveToInfo->m_vCurrentPos.y;
      matTest._43 = m_pMoveToInfo->m_vCurrentPos.z;
    }
  }

  if( m_dwParticleEffectSize > 0 )
  {
    cParticleEffectBase* pParticleEffect;
    for( unsigned i = 0; i < m_dwParticleEffectSize; i++ )
    {
      pParticleEffect = m_ppParticleEffects[i];

      // 070213 cyhsieh Particle Emit Time
      pParticleEffect->SetCurAnimTime( m_fCurrentAnimTime );

      cSkeletonHierarchy* pFrame = m_pMeshModel->m_pSkeletonHierarchyRoot->findSkeletonHierarchy( pParticleEffect->GetLinkName() );
      if( pFrame )
      {
        D3DXMATRIXA16 matTransform = ( *pFrame->getSkeletonHierarchyMatrix() ) * matTest;
        pParticleEffect->ApplyTransformMatrix( &matTransform );
      }
      pParticleEffect->FrameMove( fAnimElapsedTime );
    }
  }
  
  if( m_pRenderArray )
  {
    cSubMeshSerializer* pSubMeshSerializer;
    cEffectRender* pEffectRender;
    cSkeletonHierarchy* pFrame;
    D3DXMATRIXA16 mat;

    for( int i = 0; i < m_iRenderArraySize; i++ )
    {
      pEffectRender  = m_pRenderArray[i].m_pRender;

      if( pEffectRender == NULL )
      {
        continue;
      }

      pSubMeshSerializer = m_pMeshModel->getSubMeshSerializer( i );

      if( pSubMeshSerializer )
      {
        pFrame = m_pMeshModel->m_pSkeletonHierarchyRoot->findSkeletonHierarchy( pSubMeshSerializer->getSubMeshName() );
        if( pFrame )
        {
          //mat = *pFrame->getSkeletonHierarchyMatrix() * m_matTransform;
          mat = *pFrame->getSkeletonHierarchyMatrix() * matTest;
        }
      }
      else
      {
        //mat = m_matTransform;
        mat = matTest;
      }
      if( pEffectRender )
      {
        pEffectRender->State = RENDER_STATE_DATA_OK;
        pEffectRender->SetMatrix( &mat );

        // 070205 cyhsieh extended material animation
        // !!!To do... For test, should be set in render's framemove
        const D3DXCOLOR* pColor;
        if( pEffectRender->iMaterialSize > 0 && ( pColor = pEffectRender->m_pMaterial[0].GetDiffuseOpacity() ) )
        {
          pEffectRender->m_dwTextureFactor = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a );
        }

        switch( pEffectRender->GetMaterialAlphaBlendType() )
        {
        case 0:
          g_mScene->AddRender( pEffectRender ); // Opaque
          break;
        default:
          g_mScene->AddRenderWithType( pEffectRender, cScene::E_RENDERTYPE_MODELEFFECT );
          break;
        }
      }
    }
  }

  // 071025 測試不使用3D音效
  //float m_fx = matTest._41;
  //float m_fy = matTest._42;
  //float m_fz = matTest._43;

  //// 音效
  //// 人物距離在COMPUTE_AUDIO_MAXDISTANCE下的才播動作音效和語音
  //bool bDoCompute = true;
  ////if( !m_bPrincipal )
  ////{
  //if (m_pNowAudio)
  //{
  //  float fDiffX = g_pGameTerrain->m_vecCameraLookAt.x - m_fx;
  //  float fDiffZ = g_pGameTerrain->m_vecCameraLookAt.z - m_fz;

  //  if( ( fDiffX * fDiffX + fDiffZ * fDiffZ ) >= COMPUTE_AUDIO_MAXDISTANCESQ )
  //  {
  //    bDoCompute = false;
  //  }
  //  // 控制音效的位移 
  //  if (bDoCompute)
  //    m_pNowAudio->SetPosition(m_fx, m_fy, m_fz);

  //  
  //} // end of if (m_pNowAudio)
 
}

//---------------------------------------------------------------------------
// Name: InitTransform
// Desc: 
//---------------------------------------------------------------------------
void cModelEffect::InitTransform( const D3DXMATRIXA16& matTransform )
{
  m_matTransform = matTransform;

  if( m_dwParticleEffectSize > 0 )
  {
    cParticleEffectBase* pParticleEffect;
    for( unsigned i = 0; i < m_dwParticleEffectSize; i++ )
    {
      pParticleEffect = m_ppParticleEffects[i];

      cSkeletonHierarchy* pFrame = m_pMeshModel->m_pSkeletonHierarchyRoot->findSkeletonHierarchy( pParticleEffect->GetLinkName() );
      if( pFrame )
      {
        D3DXMATRIXA16 matParticleTransform;
        matParticleTransform = *pFrame->getSkeletonHierarchyMatrix() * m_matTransform;

        pParticleEffect->InitTransformMatrix( &matParticleTransform );
      }
    }
  }
}

//---------------------------------------------------------------------------
// Name: ApplyTransformMatrix
// Desc: 
//---------------------------------------------------------------------------
void cModelEffect::ApplyTransformMatrix( float fElapsedTime, D3DXMATRIXA16* pTransform )
{
  if( ( m_dwSpecialType == 1 ) && ( m_dwParticleEffectSize > 0 ) )
  {
    cParticleEffectBase* pParticleEffect;
    for( unsigned int i = 0; i < m_dwParticleEffectSize; i++ )
    {
      pParticleEffect = m_ppParticleEffects[i];
      pParticleEffect->ApplyTransformMatrix( pTransform );
      pParticleEffect->FrameMove( fElapsedTime );
    }
  }
}

//---------------------------------------------------------------------------
// Name: ApplyBoundingBox
// Desc: 
//---------------------------------------------------------------------------
void cModelEffect::ApplyBoundingBox( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax )
{
  if( m_dwSpecialType == 1 )
  {
    // 070731 cyhsieh particle
    cParticleEffectBase* pParticleEffect;
    for( unsigned int i = 0; i < m_dwParticleEffectSize; i++ )
    {
      pParticleEffect = m_ppParticleEffects[i];
      pParticleEffect->ApplyBoundingBox( vMin, vMax );
    }
  }
}


//-----------------------------------------------------------------------------
// Name: SetTargetPos
// Desc: 設定飛行特效目標位置
//-----------------------------------------------------------------------------
float cModelEffect::SetTargetPos( const D3DXVECTOR3& vTargetPos, BYTE byMoveType, float fMoveSpeed )
{
  if( m_pMoveToInfo == NULL )
  {
    m_pMoveToInfo = _NEW sMoveToInfo();
    if( m_pMoveToInfo == NULL )
    {
      return 0.0f;
    }
  }

  m_pMoveToInfo->m_byMoveType = byMoveType;
  m_pMoveToInfo->m_vInitialPos = D3DXVECTOR3( m_matTransform._41, m_matTransform._42, m_matTransform._43 );
  m_pMoveToInfo->m_vCurrentPos = m_pMoveToInfo->m_vInitialPos;
  m_pMoveToInfo->m_vTargetPos = vTargetPos;

  D3DXVECTOR3 vDiff = vTargetPos - m_pMoveToInfo->m_vCurrentPos;
  float fDistance = D3DXVec3Length( &vDiff );

  m_pMoveToInfo->m_fMoveSpeed = fMoveSpeed;
  float fFlyTime = fDistance / fMoveSpeed;
  m_pMoveToInfo->m_fLerp = 0.0f;
  m_pMoveToInfo->m_fLerpAddPS = 1.0f / fFlyTime;

  if( m_pMoveToInfo->m_byMoveType == NORMAL )
  {
    float fScale = ( fDistance - 40.0f ) / 160.0f;
    if( fScale < 0.0f )
    {
      fScale = 0.0f;
      m_pMoveToInfo->m_byMoveType = STRAIGHT;
    }
    else if( fScale > 1.0f )
    {
      fScale = 1.0f;
    }
    m_pMoveToInfo->m_fAdjustHeight = fScale * 15.0f;
  }
  else
  {
    m_pMoveToInfo->m_fAdjustHeight = 0.0f;  // 不需要調整高度
  }

  D3DXVECTOR2 vec( -vDiff.x, -vDiff.z );
  D3DXVec2Normalize( &vec, &vec );
  m_pMoveToInfo->m_fRotateY = acosf( vec.y );
  if( vec.x < 0.0f )
  {
    m_pMoveToInfo->m_fRotateY = 2.0f * D3DX_PI - m_pMoveToInfo->m_fRotateY;
  }
  D3DXMatrixRotationY( &m_matTransform, m_pMoveToInfo->m_fRotateY );

  m_pMoveToInfo->m_vRight = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
  D3DXVec3TransformNormal( &m_pMoveToInfo->m_vRight, &m_pMoveToInfo->m_vRight, &m_matTransform );

  return fFlyTime;
}

//---------------------------------------------------------------------------
// Name: ParesSound
// Desc:
//---------------------------------------------------------------------------
bool cModelEffect::ParseSound( cECReader& Reader, sModelEffectInfo* pMEI )
{
  // 找出'('
  if (Reader.GetToken() != cECReader::ttPARENOPEN) return false;
  // 取出音效檔名
  if (Reader.GetToken() != cECReader::ttSTRING)    return false;
  char szName[32];
  strcpy(szName, Reader.GetTokenStr());
  // 第一個逗號
  if (Reader.GetToken() != cECReader::ttCOMMA)     return false;
  // 取出延遲時間... 
  DWORD dwDelayTime = 0;
  switch (Reader.GetToken())
  {
  case cECReader::ttINT:
    dwDelayTime = (DWORD)atoi(Reader.GetTokenStr());
    break;
  default: return false;
  } // end of switch (GetToken())

  // 找出')'
  if (Reader.GetToken() != cECReader::ttPARENCLOSE) return false;

  // 新增至目前最後一個 EffectItem 內.
  int iLength = strlen( szName );
  if( iLength > 0 )
  {
    pMEI->m_pSoundName = _NEW char[iLength + 5];
    if( pMEI->m_pSoundName )
    {
      sprintf( pMEI->m_pSoundName, "%s.wav", szName );
    }
  }
  
  pMEI->m_dwSoundDelay = dwDelayTime;

  return true;
}

//---------------------------------------------------------------------------
// Name: InitEffectInfoList
// Desc: 
//---------------------------------------------------------------------------
// 070226 cyhsieh model effect table
void cModelEffect::InitEffectInfoList( void )
{
  if( m_pEffectInfoMap )
  {
    return;
  }
  
  m_pEffectInfoMap = _NEW cHashMap();
  if( m_pEffectInfoMap == NULL )
  {
    return;
  }

  EZString szTableName = g_sAppPath + "config\\modelefftbl.txt";

  VFile* pVFile = _NEW VFile( szTableName.c_str() );
  if( ( pVFile == NULL ) || ( pVFile->mem == NULL ) )
  {    
	  SAFE_DELETE( pVFile );
    return;
  }
  
  cECReader ecReader;
  ecReader.LoadFromMemory( (char*)pVFile->mem, pVFile->m_size );
  SAFE_DELETE( pVFile );

  if( ecReader.IsAlready() == false ) return;
  ecReader.Reset();
 
  for( ; ; )
  {
    switch( ecReader.GetToken() )
    {
    case cECReader::ttINT:  // 整數
      if( ParseModelEffectInfo( ecReader ) )
			{
				continue;
			}
      break;
    case cECReader::ttCOMMENT: // 註解
      continue;
    }
    // 離開迴圈
    break;
  }
}

//---------------------------------------------------------------------------
// Name: ParseModelEffectInfo
// Desc: 
//---------------------------------------------------------------------------
bool cModelEffect::ParseModelEffectInfo( cECReader& ecReader )
{
  int iEffectID = ecReader.GetTokenInt(); // 編號

  if( ecReader.GetToken() != cECReader::ttSTRING )
  {
    return false;
  }
  char szEffectName[MAX_PATH];
  strcpy( szEffectName, ecReader.GetTokenStr() ); // 模型特效檔名

  if( ecReader.GetToken() != cECReader::ttINT )
  {
    return false;
  }
  int iLinkID = ecReader.GetTokenInt(); // 連結位置

  if( ecReader.GetToken() != cECReader::ttCOMMA )
  {
    return false;
  }

  if( ecReader.GetToken() != cECReader::ttIDENTIFIER )
  {
    return false;
  }
  if( strcmp( ecReader.GetTokenStr(), "sound" ) != 0 )
  {
    return false;
  }

  sModelEffectInfo* pModelEffectInfo = _NEW sModelEffectInfo();
  if( pModelEffectInfo == NULL )
  {
    return false;
  }

  int iLength = strlen( szEffectName );
  pModelEffectInfo->m_pFileName = _NEW char[iLength + 1];
  if( pModelEffectInfo->m_pFileName )
  {
    strcpy( pModelEffectInfo->m_pFileName, szEffectName );
  }
  pModelEffectInfo->m_byLinkID = (BYTE)iLinkID;
  m_pEffectInfoMap->Insert( iEffectID, pModelEffectInfo );

  return ParseSound( ecReader, pModelEffectInfo );
}

//---------------------------------------------------------------------------
// Name: DeleteEffectInfoList
// Desc: 
//---------------------------------------------------------------------------
// 070226 cyhsieh model effect table
void cModelEffect::DeleteEffectInfoList( void )
{
  if( m_pEffectInfoMap == NULL )
  {
    return;
  }

  sModelEffectInfo* pModelEffectInfo;
  int iCount = m_pEffectInfoMap->GetTotalCount();

  for( int i = 0; i < iCount; i++ )
  {
    pModelEffectInfo = (sModelEffectInfo*)m_pEffectInfoMap->GetPointIndex( i );
    SAFE_DELETE( pModelEffectInfo );
  }

  delete m_pEffectInfoMap;
}

//---------------------------------------------------------------------------
// Name: GetModelEffectInfo
// Desc: 
//---------------------------------------------------------------------------
// 070226 cyhsieh model effect table
sModelEffectInfo* cModelEffect::GetModelEffectInfo( DWORD dwEffectID )
{
  if( m_pEffectInfoMap )
  {
    return (sModelEffectInfo*)m_pEffectInfoMap->GetPoint( dwEffectID );
  }

  return NULL;
}

//---------------------------------------------------------------------------
// Name: SetGuid
// Desc: 
//---------------------------------------------------------------------------
// 070412 cyhsieh 長效特效
void cModelEffect::SetGuid( char* pGuid )
{
  SAFE_DELETE_ARRAY( m_pGuid );

  if( pGuid )
  {
    int iLength = strlen( pGuid );
    m_pGuid = _NEW char[iLength + 1];
    if( m_pGuid )
    {
      strcpy( m_pGuid, pGuid );
    }
  }
}

//---------------------------------------------------------------------------
// Name: GetMeshModelName
// Desc: 
//---------------------------------------------------------------------------
// 071218 cyhsieh 人物照片
char* cModelEffect::GetMeshModelName( void )
{
  if( m_pMeshModel )
  {
    return m_pMeshModel->getMeshSerializerName();
  }
  return NULL;
}

//---------------------------------------------------------------------------
// Name: ForceDrawParticleEffects
// Desc: 
//---------------------------------------------------------------------------
// 071225 cyhsieh 特效
void cModelEffect::ForceDrawParticleEffects( void )
{
  if( g_p3dsMaxParticleManager )
  {
    for( unsigned int i = 0; i < m_dwParticleEffectSize; i++ )
    {
      g_p3dsMaxParticleManager->AddForceDrawParticleEffect( m_ppParticleEffects[i] );
    }
  }
}

//---------------------------------------------------------------------------
// Name: SetForceDir
// Desc: 設定強制旋轉方向
//---------------------------------------------------------------------------
void cModelEffect::SetForceDir( float fDirAngle )
{
  if( m_pForceRotation == NULL )
  {
    m_pForceRotation = new D3DXMATRIXA16;
    if( m_pForceRotation == NULL )
    {
      return;
    }
  }
  D3DXMatrixRotationY( m_pForceRotation, D3DXToRadian( fDirAngle ) );
}

//---------------------------------------------------------------------------
// Name: PlayAudio
// Desc: 
//---------------------------------------------------------------------------
void cModelEffect::PlayAudio()
{
  sModelEffectInfo* pModelEffectInfo = GetModelEffectInfo(m_wEffectID);
  
  if(pModelEffectInfo && pModelEffectInfo->m_pSoundName)
  {
    if (m_pAudioManage)
    {
      cAudioBase* pAudioBase = m_pAudioManage->ForageName(pModelEffectInfo->m_pSoundName);
      if (pAudioBase == NULL)
      {
        m_pAudioManage->SetFilePath("Audio\\sfx\\");
        pAudioBase = m_pAudioManage->ForageName(AUDIO_FT_NORMAL, pModelEffectInfo->m_pSoundName);
      } // end of if (pAudioBase == NULL)

      if (pAudioBase)
      {
        SAFE_RELEASE(m_pNowAudio);
        // 建立新音效
        m_pNowAudio = pAudioBase->GetNewAudio();
        if (m_pNowAudio)
        {
          // 設定起始播放時間
          m_pNowAudio->SetStartTimems(timeGetTime()+pModelEffectInfo->m_dwSoundDelay);

          // 設定影響範圍
          // 071025 測試不使用3D音效
          /*m_pNowAudio->SetMinDistance( COMP );
          m_pNowAudio->SetMaxDistance( COMPUTE_AUDIO_MAXDISTANCE );*/

          // 開始播放
          m_pNowAudio->Play();
        } // end of if (m_pNowAudio)
      } // end of if (pAudioBase)
    } // end of if(m_pAudioManage)
  } // end of if (pModelEffectInfo && pModelEffectInfo->m_pSoundName)
}

//---------------------------------------------------------------------------
// Name: InitBornEffectList
// Desc: 初始化天生特效列表
//---------------------------------------------------------------------------
void cModelEffect::InitBornEffectList( void )
{
  if( m_pBornEffectList )
  {
    return;
  }
  
  m_pBornEffectList = _NEW BTList();
  if( m_pBornEffectList == NULL )
  {
    return;
  }

  EZString szTableName = g_sAppPath + "config\\BornEffectTbl.txt";

  VFile* pVFile = _NEW VFile( szTableName.c_str() );
  if( ( pVFile == NULL ) || ( pVFile->mem == NULL ) )
  {    
	  SAFE_DELETE( pVFile );
    return;
  }
  
  cECReader ecReader;
  ecReader.LoadFromMemory( (char*)pVFile->mem, pVFile->m_size );
  SAFE_DELETE( pVFile );

  if( ecReader.IsAlready() == false ) return;
  ecReader.Reset();
  
  sBornEffectInfo* pBornEffectInfo = NULL;
  char* pFileName = NULL;
  WORD wEffectID;
  BYTE byLinkID;
  int iIndex = 0;
  bool bLoop = true;
  for( ; bLoop ; )
  {
    switch( ecReader.GetToken() )
    {
    case cECReader::ttINT:     // 整數
      if( iIndex == 0 )
      {
        pBornEffectInfo = _NEW sBornEffectInfo();
        if( pBornEffectInfo == NULL )
        {
          bLoop = false;
          break;
        }
      }
      wEffectID = (WORD)ecReader.GetTokenInt();

      if( ecReader.GetToken() != cECReader::ttCOMMA )
      {
        bLoop = false;
        break;
      }

      if( ecReader.GetToken() != cECReader::ttINT )
      {
        bLoop = false;
        break;
      }
      byLinkID = (BYTE)ecReader.GetTokenInt();
      pBornEffectInfo->m_dwBornEffect[iIndex++] = ( wEffectID << 16 ) | byLinkID;
      break;
      
    case cECReader::ttCOMMENT: // 註解
      break;

    case cECReader::ttSEMICOLON:  // 分號
      m_pBornEffectList->Add( pBornEffectInfo );
      pBornEffectInfo = NULL;
      iIndex = 0;
      break;
    
    default:
      bLoop = false;
      break;
    }
  }

  if( pBornEffectInfo )
  {
    delete pBornEffectInfo;
  }
}

//---------------------------------------------------------------------------
// Name: DeleteBornEffectList
// Desc: 刪除天生特效列表
//---------------------------------------------------------------------------
void cModelEffect::DeleteBornEffectList( void )
{
  if( m_pBornEffectList == NULL )
  {
    return;
  }

  sBornEffectInfo* pBornEffectInfo;
  int iCount = m_pBornEffectList->GetCount();

  for( int i = 0; i < iCount; i++ )
  {
    pBornEffectInfo = (sBornEffectInfo*)m_pBornEffectList->GetItem( i );
    SAFE_DELETE( pBornEffectInfo );
  }

  delete m_pBornEffectList;
}

//---------------------------------------------------------------------------
// Name: GetBornEffect
// Desc: 取得天生特效資訊
//---------------------------------------------------------------------------
sBornEffectInfo* cModelEffect::GetBornEffect( DWORD dwBornEffectID )
{
  if( m_pBornEffectList )
  {
    return (sBornEffectInfo*)m_pBornEffectList->GetItem( dwBornEffectID );
  }

  return NULL;
}

// Antony 970625 fMT_CCharBuild 只需執行一次
// 以g_pGameTerrain->m_pEffectFromPackageList->GetItem()取得ModelEffect
#define ANCHOR_EFFECT_GUID  "anchor_fx" // 自動打怪定錨特效Client's GUID

mud_fCallInfo *cModelEffect::OutputCharBuild()
{
	if( m_pGuid == NULL || m_pGuid == ANCHOR_EFFECT_GUID )
		return NULL;

	struct MT_sCCharBuild *pCB = _NEW MT_sCCharBuild;
	
	pCB->guid = _strdup( m_pGuid );	
	pCB->name = NULL;
	pCB->model_id = NULL;	
	pCB->model_name = NULL;
	pCB->model_class = 0;
	pCB->model_face = 0;
	pCB->mien_id = 0;
	pCB->model_pic = 0;
	pCB->state = 0;
	pCB->ani_id = m_wEffectID;

	int i_lMapX, i_lMapY;
	float fOffset = g_pGameTerrain->GetTilePosOffset();

	i_lMapX = (int)( ( m_fMapX + fOffset ) / COMP );
	i_lMapY = (int)( ( m_fMapZ + fOffset ) / COMP );

	int iScale = g_pGameTerrain->GetMeshScale();
    if( iScale != 1 )
	{
		unsigned int iMapWide = g_pGameTerrain->GetMapInfo().GetTileCountX();
		int iOffset = ( ( iScale - 1 ) * iMapWide ) / 2;
		i_lMapX -= iOffset;
		i_lMapY -= iOffset;
	}

	g_pGameTerrain->ConvertClientPosXToServer( i_lMapX );

	pCB->xy = ( i_lMapX << 16 ) + i_lMapY;
	pCB->hp_percent = 0;

	stPacketBufferInfo *pPBI = _NEW stPacketBufferInfo;

	int StrLen = strlen( pCB->guid );
	int length = 0;

	pPBI->iBufSize = StrLen + 4 + sizeof( long ) * 2 + sizeof( unsigned char ) * 6;
	pPBI->pBuffer = _NEW char[pPBI->iBufSize];

	extern void ConvertGBToBig5( char* sz_src, char* sz_dest );

	// 存入TYPE_STRING開始 ====================
	// 判定字串長度.
	if( StrLen > 0 )
	{ // 字串長度大於零, 正常. 
		strcpy( pPBI->pBuffer, pCB->guid );
	} 
	else 
	{ // 字串長度小於 1, 失敗, 修正.
		pPBI->pBuffer[0] = '\0';
	}	

	if( g_Options.GetLanguage() == cOptions::langChSi )
	{
		char* sz = pPBI->pBuffer;
		ConvertGBToBig5( sz, sz );
	}

	pPBI->pBuffer += StrLen + 1;
	length += StrLen + 1;
	// 存入TYPE_STRING結束 ====================

	// 存入TYPE_STRING開始 ====================	
	pPBI->pBuffer[0] = '\0';

	if( g_Options.GetLanguage() == cOptions::langChSi )
	{
		char* sz = pPBI->pBuffer;
		ConvertGBToBig5( sz, sz );
	}

	pPBI->pBuffer += 1;
	length += 1;
	// 存入TYPE_STRING結束 ====================

	// 存入TYPE_STRING開始 ====================	
	pPBI->pBuffer[0] = '\0';

	if( g_Options.GetLanguage() == cOptions::langChSi )
	{
		char* sz = pPBI->pBuffer;
		ConvertGBToBig5( sz, sz );
	}

	pPBI->pBuffer += 1;
	length += 1;
	// 存入TYPE_STRING結束 ====================

	// 存入TYPE_STRING開始 ====================	
	pPBI->pBuffer[0] = '\0';

	if( g_Options.GetLanguage() == cOptions::langChSi )
	{
		char* sz = pPBI->pBuffer;
		ConvertGBToBig5( sz, sz );
	}

	pPBI->pBuffer += 1;
	length += 1;
	// 存入TYPE_STRING結束 ====================

	// 存入TYPE_BYTE開始 ====================
	*(unsigned char *)pPBI->pBuffer = pCB->model_class;	
	pPBI->pBuffer += sizeof( unsigned char );
	length += sizeof( unsigned char );
	// 存入TYPE_BYTE結束 ====================

	// 存入TYPE_BYTE開始 ====================
	*(unsigned char *)pPBI->pBuffer = pCB->model_face;	
	pPBI->pBuffer += sizeof( unsigned char );
	length += sizeof( unsigned char );
	// 存入TYPE_BYTE結束 ====================

	// 存入TYPE_BYTE開始 ====================
	*(unsigned char *)pPBI->pBuffer = pCB->mien_id;	
	pPBI->pBuffer += sizeof( unsigned char );
	length += sizeof( unsigned char );
	// 存入TYPE_BYTE結束 ====================

	// 存入TYPE_BYTE開始 ====================
	*(unsigned char *)pPBI->pBuffer = pCB->model_pic;	
	pPBI->pBuffer += sizeof( unsigned char );
	length += sizeof( unsigned char );
	// 存入TYPE_BYTE結束 ====================

	// 存入TYPE_BYTE開始 ====================
	*(unsigned char *)pPBI->pBuffer = pCB->state;	
	pPBI->pBuffer += sizeof( unsigned char );
	length += sizeof( unsigned char );
	// 存入TYPE_BYTE結束 ====================

	// 存入TYPE_LONG開始 ====================
	memcpy( pPBI->pBuffer, &pCB->ani_id, sizeof( long ) );
	*(u_long *)pPBI->pBuffer = htonl( *(u_long *)pPBI->pBuffer );
	pPBI->pBuffer += sizeof( long );
	length += sizeof( long );
	// 存入TYPE_LONG結束 ====================	

	// 存入TYPE_LONG開始 ====================
	memcpy( pPBI->pBuffer, &pCB->xy, sizeof( long ) );
	*(u_long *)pPBI->pBuffer = htonl( *(u_long *)pPBI->pBuffer );
	pPBI->pBuffer += sizeof( long );
	length += sizeof( long );
	// 存入TYPE_LONG結束 ====================

	// 存入TYPE_BYTE開始 ====================
	*(unsigned char *)pPBI->pBuffer = pCB->hp_percent;	
	pPBI->pBuffer += sizeof( unsigned char );
	length += sizeof( unsigned char );
	// 存入TYPE_BYTE結束 ====================

	SAFE_DELETE_ARRAY( pCB->guid );
	SAFE_DELETE( pCB );

	pPBI->pBuffer -= pPBI->iBufSize;

	mud_fCallInfo *pCallInfo = NULL;

	if( length != pPBI->iBufSize )
	{
		SAFE_DELETE_ARRAY( pPBI->pBuffer );
		SAFE_DELETE( pPBI );
		return NULL;
	}
	else
	{
		pCallInfo = _NEW mud_fCallInfo;
		pCallInfo->idx = g_mGameNet->LocalFunctionIndexLookup( MT_CCharBuild );
		pCallInfo->nArgs = 1;
		pCallInfo->pArgs = NULL;
		pCallInfo->pPacketBufferInfo = pPBI;
	}

	return pCallInfo;
}
// Antony End