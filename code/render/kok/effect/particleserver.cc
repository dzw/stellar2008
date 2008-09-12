//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"


namespace KOK
{

c3dsMaxParticleManager::c3dsMaxParticleManager()
:m_bEnable(false), m_pRenderBuffer(NULL), m_pTextureDB(NULL), m_pParticlePool(NULL), m_pDecalIndexBuffer(NULL),
 m_pParticleEffectList(NULL), m_pParticleEffectSortList(NULL), m_pForceDrawParticleEffectList(NULL)
{
}

c3dsMaxParticleManager::~c3dsMaxParticleManager()
{
  SAFE_DELETE( m_pForceDrawParticleEffectList );
  SAFE_DELETE( m_pParticleEffectSortList )
  if( m_pParticleEffectList )
  {
    cParticleEffectBase* pParticleEffect;
    int iCount = m_pParticleEffectList->GetCount();
    for( int i = 0; i < iCount; i++ )
    {
      pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
      SAFE_DELETE( pParticleEffect );
    }
    delete m_pParticleEffectList;
  }

  SAFE_DELETE( m_pParticlePool );
  SAFE_DELETE( m_pTextureDB );
  SAFE_RELEASE( m_pRenderBuffer );
  SAFE_RELEASE( m_pDecalIndexBuffer );
}

void c3dsMaxParticleManager::InitializeResource( LPDIRECT3DDEVICE9 pD3DDevice )
{
  int iMaxParticleSize = 5120;

  m_pRenderBuffer = g_pRBManage->CreateNewBuffer();
  if( m_pRenderBuffer )
  {
    int iVertexTotalSize = iMaxParticleSize * 4;
    int iIndexTotalSize = iMaxParticleSize * 6;
    m_pRenderBuffer->SetVertexBufferSize( pD3DDevice, cRenderBuffer::vtDyEFFECT_MDLVTX, iVertexTotalSize );
    m_pRenderBuffer->SetIndexBufferSize( pD3DDevice, cRenderBuffer::itStaticWORD, iIndexTotalSize );

    unsigned int iLockIndex = 0;
    int iStartIndex = 0;
    int iStartVertex = 0;
    WORD* pIndex = m_pRenderBuffer->m_pWStaticIB->Lock( iIndexTotalSize, iLockIndex );
    if( pIndex )
    {
      for( int i = 0; i < iMaxParticleSize; i++ )
      {
        pIndex[iStartIndex + 0] = iStartVertex + 0;
        pIndex[iStartIndex + 1] = iStartVertex + 1;
        pIndex[iStartIndex + 2] = iStartVertex + 2;
        pIndex[iStartIndex + 3] = iStartVertex + 1;
        pIndex[iStartIndex + 4] = iStartVertex + 3;
        pIndex[iStartIndex + 5] = iStartVertex + 2;
        iStartIndex += 6;
        iStartVertex += 4;
      }
      m_pRenderBuffer->m_pWStaticIB->Unlock();
    }
  }

  m_pDecalIndexBuffer = g_pRBManage->CreateNewBuffer();
  if( m_pDecalIndexBuffer )
  {
    m_pDecalIndexBuffer->SetIndexBufferSize( pD3DDevice, cRenderBuffer::itDynamicWORD, 2560 );
  }

  m_pTextureDB = _NEW cTextureDB();
  if( m_pTextureDB )
  {
    m_pTextureDB->InitDeviceObjects( pD3DDevice );
  }

  m_pParticlePool = _NEW cParticlePool( iMaxParticleSize, NULL, sizeof(cParticle) );

  m_pParticleEffectList = _NEW BTList;
  m_pParticleEffectSortList = _NEW BTList;
  m_pForceDrawParticleEffectList = _NEW BTList;
}

//-----------------------------------------------------------------------------
// Name: CreateParticleEffectByEmitData
// Desc: 产生Max粒子特效
//-----------------------------------------------------------------------------
cParticleEffectBase* c3dsMaxParticleManager::CreateParticleEffectByEmitData( c3dsMaxEmitDataBase* pEmitData, char* pTexurePath )
{
  if( m_pParticleEffectList == NULL )
  {
    return NULL;
  }

	static char* path_id[2]={ EFFECT_STUFF_PATH, MESH_STUFF_PATH };

  cParticleEffectBase* pParticleEffect = NULL;
  
  if( pEmitData )
  {
    DWORD dwDataType = pEmitData->GetDataType();
    switch( dwDataType )
    {
    case 1:
      {
        c3dsMaxEmitDataCloud* pEmitDataCloud = (c3dsMaxEmitDataCloud*)pEmitData;
        pParticleEffect = _NEW cParticleCloud( m_pParticlePool, pEmitDataCloud );
      }
      break;

    // 070205 cyhsieh particle superspray
    case 2:
      {
        c3dsMaxEmitDataSuperSpray* pEmitDataSuperSpray = (c3dsMaxEmitDataSuperSpray*)pEmitData;
        pParticleEffect = _NEW cParticleSuperSpray( m_pParticlePool, pEmitDataSuperSpray );
      }
      break;
    }
  }

  if( pParticleEffect )
  {
    c3dsMaxParticleRender* pRender = pParticleEffect->GetRender();
    if( pRender )
    {
      pRender->m_pRenderBuffer = m_pRenderBuffer;

      if( pRender->iMaterialSize )
      {
        // 地物特效
        EZString szTexturePath;
				char* stuff_path;
        if( pTexurePath == NULL )
        {
          szTexturePath = "Effect\\efm\\";
					stuff_path=path_id[0];
        }
        else
        {
          szTexturePath = pTexurePath;
					stuff_path=path_id[1];
        }

        for( int i = 0; i < pRender->iMaterialSize; i++ )
        {
          cMaterial& material = pRender->m_pMaterial[i];
          if( material.m_pszTextName )
          {
			  // 地物特效
            material.m_pTexture = m_pTextureDB->QueryTexture(material.m_pszTextName,szTexturePath.c_str(),stuff_path,sTextureQueryParam( true, 0 )); 
          }
        }
      }
    }

    m_pParticleEffectList->Add( pParticleEffect );
  }

  return pParticleEffect;
}

//-----------------------------------------------------------------------------
// Name: CreateParticleEffectBySubMeshSerializer
// Desc: 产生Billboard粒子特效
//-----------------------------------------------------------------------------
cParticleEffectBase* c3dsMaxParticleManager::CreateParticleEffectBySubMeshSerializer( cSubMeshSerializer* pSubMeshSerializer, char* pTexurePath )
{
  if( m_pParticleEffectList == NULL )
  {
    return NULL;
  }

	static char* path_id[2]={ EFFECT_STUFF_PATH, MESH_STUFF_PATH };

  cParticleEffectBase* pParticleEffect = NULL;
  
  if( pSubMeshSerializer )
  {
    DWORD dwMeshType = pSubMeshSerializer->GetMeshType();
    switch( dwMeshType )
    {
    case 1: // Billboard
      pParticleEffect = _NEW cParticleBillboard( m_pParticlePool, pSubMeshSerializer, 0 );
      break;
    case 2: // Billboard Y
      pParticleEffect = _NEW cParticleBillboard( m_pParticlePool, pSubMeshSerializer, 1 );
      break;
    case 3: // Ribbon
      pParticleEffect = _NEW cParticleRibbon( m_pParticlePool, pSubMeshSerializer );
      break;
    case 4: // Zodiac
      pParticleEffect = _NEW cParticleZodiac( m_pParticlePool, pSubMeshSerializer );
      break;
    }
  }

  if( pParticleEffect )
  {
    c3dsMaxParticleRender* pRender = pParticleEffect->GetRender();
    if( pRender )
    {
      pRender->m_pRenderBuffer = m_pRenderBuffer;
      pRender->m_pDecalIndexBuffer = m_pDecalIndexBuffer;

      if( pRender->iMaterialSize )
      {
        // 地物特效
        EZString szTexturePath;
				char* stuff_path;
        if( pTexurePath == NULL )
        {
          szTexturePath = "Effect\\efm\\";
					stuff_path=path_id[0];
        }
        else
        {
          szTexturePath = pTexurePath;
					stuff_path=path_id[1];
        }

        for( int i = 0; i < pRender->iMaterialSize; i++ )
        {
          cMaterial& material = pRender->m_pMaterial[i];
          if( material.m_pszTextName )
          {
            //地物特效
            material.m_pTexture = m_pTextureDB->QueryTexture(material.m_pszTextName,szTexturePath.c_str(),stuff_path,sTextureQueryParam( true, 0 ));
          }
        }
      }
    }

    m_pParticleEffectList->Add( pParticleEffect );
  }

  return pParticleEffect;
}

//-----------------------------------------------------------------------------
// Name: CreateParticleEffectWeaponRibbon
// Desc: 产生刀光特效
//-----------------------------------------------------------------------------
// 070424 cyhsieh 猌竟
cParticleEffectBase* c3dsMaxParticleManager::CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath )
{
  if( m_pParticleEffectList == NULL )
  {
    return NULL;
  }

  cParticleEffectBase* pParticleEffect = _NEW cParticleWeaponRibbon( m_pParticlePool, bDoLerp, vMinPos, vMaxPos );
	static char* path_id[2]={ EFFECT_STUFF_PATH, MESH_STUFF_PATH };
  
  if( pParticleEffect )
  {
    c3dsMaxParticleRender* pRender = pParticleEffect->GetRender();
    if( pRender )
    {
      pRender->m_pRenderBuffer = m_pRenderBuffer;

      if( pRender->iMaterialSize )
      {
        EZString szTexturePath;
				char* stuff_path;
        if( pTexurePath == NULL )
        {
          szTexturePath = "Effect\\efm\\";
					stuff_path=path_id[0];
        }
        else
        {
          szTexturePath = pTexurePath;
					stuff_path=path_id[1];
        }

        for( int i = 0; i < pRender->iMaterialSize; i++ )
        {
          cMaterial& material = pRender->m_pMaterial[i];
          if( material.m_pszTextName )
          {
            material.m_pTexture = m_pTextureDB->QueryTexture(material.m_pszTextName,szTexturePath.c_str(),stuff_path,sTextureQueryParam( true, 0 ));
          }
        }
      }
    }

    m_pParticleEffectList->Add( pParticleEffect );
  }

  return pParticleEffect;
}

//-----------------------------------------------------------------------------
// Name: ReleaseParticleEffect
// Desc: 睦采疭
//-----------------------------------------------------------------------------
void c3dsMaxParticleManager::ReleaseParticleEffect( cParticleEffectBase* pRelease )
{
  if( m_pParticleEffectList == NULL )
  {
    return;
  }

  cParticleEffectBase* pParticleEffect = NULL;
  int iCount;
  
  iCount = m_pParticleEffectList->GetCount();
  for( int i = 0; i < iCount; i++ )
  {
    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
    if( pParticleEffect == pRelease )
    {
      SAFE_DELETE( pParticleEffect );
      m_pParticleEffectList->Exchange( i, iCount - 1 );
      m_pParticleEffectList->Delete( iCount - 1 );
      return;
    }
  }

  iCount = m_pParticleEffectList->GetCount();
  for( int i = 0; i < iCount; i++ )
  {
    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
    if( pParticleEffect == pRelease )
    {
      SAFE_DELETE( pParticleEffect );
      m_pParticleEffectList->Exchange( i, iCount - 1 );
      m_pParticleEffectList->Delete( iCount - 1 );
      return;
    }
  }
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Framemove
//-----------------------------------------------------------------------------
void c3dsMaxParticleManager::FrameMove( float fElapsedTime )
{
  // 070412 cyhsieh 疭
  if( m_bEnable == false )
  {
    return;
  }

  float fAdjustElapsedTime = fElapsedTime * 4800.0f;
  cParticleEffectBase* pParticleEffect = NULL;
  int iCount = m_pParticleEffectList->GetCount();
  for( int i = 0; i < iCount; i++ )
  {
    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
    if( pParticleEffect && ( pParticleEffect->GetRenderEnabled() == false ) )
    {
      pParticleEffect->DisableFrameMove( fAdjustElapsedTime );
    }
  }

  if( m_pTextureDB )
  {
    m_pTextureDB->FrameMove( fElapsedTime );
  }
}

//-----------------------------------------------------------------------------
// Name: ParticleEffectDistCompare
// Desc: 
//-----------------------------------------------------------------------------
int ParticleEffectDistCompare( void* pArg0, void* pArg1 )
{
  cParticleEffectBase* pParticleEffect0 = (cParticleEffectBase *)pArg0;
  cParticleEffectBase* pParticleEffect1 = (cParticleEffectBase *)pArg1;

  const D3DXVECTOR3& vCameraPos = c3dsMaxParticleManager::GetCameraPos();
	D3DXVECTOR3 vDif0 = pParticleEffect0->GetCurPosition() - vCameraPos;
	D3DXVECTOR3 vDif1 = pParticleEffect1->GetCurPosition() - vCameraPos;

	float fDist0 = D3DXVec3LengthSq( &vDif0 );
	float fDist1 = D3DXVec3LengthSq( &vDif1 );

	return int( fDist1 - fDist0 );
}

//-----------------------------------------------------------------------------
// Name: Draw
// Desc: Draw
//-----------------------------------------------------------------------------
void c3dsMaxParticleManager::Draw( LPDIRECT3DDEVICE9 pD3DDevice )
{
	// 不打光、不打雾在呼叫此函式前已设定

	// 070412 cyhsieh 长效特效
  if( m_bEnable == false )
  {
    return;
  }

  m_pRenderBuffer->SetStreamSource( pD3DDevice );
  m_pRenderBuffer->SetIndices( pD3DDevice );
  m_pRenderBuffer->SetVertexType( pD3DDevice );

  D3DXMATRIXA16 matWorld;
  D3DXMatrixIdentity( &matWorld );
  pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );

  cParticleEffectBase* pParticleEffect = NULL;
  int iCount;
  iCount = m_pParticleEffectList->GetCount();
  for( int i = 0; i < iCount; i++ )
  {
    pParticleEffect = (cParticleEffectBase*)m_pParticleEffectList->GetItem( i );
    if( pParticleEffect && pParticleEffect->GetRenderEnabled() )
    {
      if( pParticleEffect->GetRenderAlphaBlendType() > 0 )
      {
        m_pParticleEffectSortList->Add( pParticleEffect );
      }
      else
      {
        pParticleEffect->Draw( pD3DDevice );
      }
    }
  }

  iCount = m_pParticleEffectSortList->GetCount();
  if( iCount > 0 )
  {
    m_pParticleEffectSortList->Sort( ParticleEffectDistCompare );

    for( int i = 0; i < iCount; i++ )
    {
      pParticleEffect = (cParticleEffectBase*)m_pParticleEffectSortList->GetItem( i );
      if( pParticleEffect )
      {
        pParticleEffect->Draw( pD3DDevice );
      }
    }

    m_pParticleEffectSortList->Clear();
  }
}

//-----------------------------------------------------------------------------
// Name: AddForceDrawParticleEffect
// Desc: 增加照片用粒子特效
//-----------------------------------------------------------------------------
// 071225 cyhsieh 疭
void c3dsMaxParticleManager::AddForceDrawParticleEffect( cParticleEffectBase* pParticleEffect )
{
  if( m_pForceDrawParticleEffectList && pParticleEffect )
  {
    m_pForceDrawParticleEffectList->Add( pParticleEffect );
  }
}

//-----------------------------------------------------------------------------
// Name: ForceDraw
// Desc: 强制画(给照片用)
//-----------------------------------------------------------------------------
// 071218 cyhsieh 人物照片
void c3dsMaxParticleManager::ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice )
{
  if( ( m_pForceDrawParticleEffectList == NULL ) || ( m_pParticleEffectSortList == NULL ) )
  {
    return;
  }

  int iCount = m_pForceDrawParticleEffectList->GetCount();
  if( iCount == 0 )
  {
    return;
  }

  // 不打光、不打雾在呼叫此函式前已设定

  m_pRenderBuffer->SetStreamSource( pD3DDevice );
  m_pRenderBuffer->SetIndices( pD3DDevice );
  m_pRenderBuffer->SetVertexType( pD3DDevice );

  D3DXMATRIXA16 matWorld;
  D3DXMatrixIdentity( &matWorld );
  pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );

  cParticleEffectBase* pParticleEffect = NULL;
  for( int i = 0; i < iCount; i++ )
  {
    pParticleEffect = (cParticleEffectBase*)m_pForceDrawParticleEffectList->GetItem( i );
    if( pParticleEffect && pParticleEffect->GetRenderEnabled() )
    {
      if( pParticleEffect->GetRenderAlphaBlendType() > 0 )
      {
        m_pParticleEffectSortList->Add( pParticleEffect );
      }
      else
      {
        pParticleEffect->Draw( pD3DDevice );
      }
    }
  }
  m_pForceDrawParticleEffectList->Clear();

  iCount = m_pParticleEffectSortList->GetCount();
  if( iCount > 0 )
  {
    m_pParticleEffectSortList->Sort( ParticleEffectDistCompare );

    for( int i = 0; i < iCount; i++ )
    {
      pParticleEffect = (cParticleEffectBase*)m_pParticleEffectSortList->GetItem( i );
      if( pParticleEffect )
      {
        pParticleEffect->Draw( pD3DDevice );
      }
    }

    m_pParticleEffectSortList->Clear();
  }
}
}