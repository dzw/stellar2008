//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/particleserver.h"
#include "kok/effect/cloudparticleemitter.h"
#include "kok/effect/Billboardparticleemitter.h"
#include "kok/effect/Ribbonparticleemitter.h"
#include "kok/effect/supersprayparticleemitter.h"
#include "kok/effect/WeaponRibbonparticleemitter.h"
#include "kok/effect/Zodiacparticleemitter.h"
#include "memory/memory.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "resources/resourceloader.h"

namespace KOK
{
ImplementClass(KOK::ParticleServer, 'PTSR', Core::RefCounted);
ImplementSingleton(KOK::ParticleServer);

using namespace CoreGraphics;
using namespace Util;
using namespace Resources;

ParticleServer::ParticleServer()
:m_pParticlePool(NULL)
//, m_pDecalIndexBuffer(NULL)
{
	ConstructSingleton;
}

ParticleServer::~ParticleServer()
{
	DestructSingleton;

	this->indexBuffer = 0;
	this->vertexPool = 0;
	if (m_pParticlePool)
		n_delete(m_pParticlePool);
}

void ParticleServer::InitializeResource()
{
	int iMaxParticleSize = 5120;

    int iVertexTotalSize = iMaxParticleSize * 4;
    int iIndexTotalSize = iMaxParticleSize * 6;
    
    unsigned int iLockIndex = 0;
    int iStartIndex = 0;
    int iStartVertex = 0;
	
	Memory::MemoryMark memMark(Memory::MemStack);

	WORD* pIndex = new(Memory::MemStack, iIndexTotalSize)WORD; 
    n_assert(pIndex != NULL);
    
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

	this->indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, iIndexTotalSize, pIndex, iIndexTotalSize*sizeof(WORD));
	this->indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->indexBuffer->Load();
	this->indexBuffer->SetLoader(0);
	n_assert(this->indexBuffer->GetState() == IndexBuffer::Loaded);


	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	vertexComponents.Append(CoreGraphics::VertexComponent(CoreGraphics::VertexComponent::Position,0,CoreGraphics::VertexComponent::Float3));
	vertexComponents.Append(CoreGraphics::VertexComponent(CoreGraphics::VertexComponent::Color,0,CoreGraphics::VertexComponent::Float4));
	vertexComponents.Append(CoreGraphics::VertexComponent(CoreGraphics::VertexComponent::TexCoord,0,CoreGraphics::VertexComponent::Float2));

	this->vertexPool = VertexBufferPool::Create();
	this->vertexPool->Reset(sizeof(ParticleFVF), iMaxParticleSize,vertexComponents);

	/*m_pDecalIndexBuffer = g_pRBManage->CreateNewBuffer();
	if( m_pDecalIndexBuffer )
	{
		m_pDecalIndexBuffer->SetIndexBufferSize( pD3DDevice, cRenderBuffer::itDynamicWORD, 2560 );
	}*/

	m_pParticlePool = n_new(cParticlePool( iMaxParticleSize, NULL, sizeof(cParticle) ));

	/*m_pParticleEffectList = _NEW BTList;
	m_pParticleEffectSortList = _NEW BTList;
	m_pForceDrawParticleEffectList = _NEW BTList;*/
}

//-----------------------------------------------------------------------------
// Name: CreateParticleEffectByEmitData
// Desc: 产生Max粒子特效
//-----------------------------------------------------------------------------
ParticleEmitter* ParticleServer::CreateParticleEffectByEmitData( EmitterData* pEmitData, char* pTexurePath )
{
  
  ParticleEmitter* pParticleEffect = NULL;
  
  if( pEmitData )
  {
    DWORD dwDataType = pEmitData->GetDataType();
    switch( dwDataType )
    {
    case 1:
      {
        CloudEmitterData* pEmitDataCloud = (CloudEmitterData*)pEmitData;
        pParticleEffect = n_new(CloudParticleEmitter( m_pParticlePool, pEmitDataCloud ));
      }
      break;
    case 2:
      {
        SuperSprayEmitterData* pEmitDataSuperSpray = (SuperSprayEmitterData*)pEmitData;
        pParticleEffect = n_new(SuperSprayParticleEmitter( m_pParticlePool, pEmitDataSuperSpray ));
      }
      break;
    }
  }

  return pParticleEffect;
}

//-----------------------------------------------------------------------------
// Name: CreateParticleEffectBySubMeshSerializer
// Desc: 产生Billboard粒子特效
//-----------------------------------------------------------------------------
ParticleEmitter* ParticleServer::CreateParticleEffectBySubMeshSerializer( const Ptr<KokShapeNode>& pSubMeshSerializer, char* pTexurePath )
{
  ParticleEmitter* pParticleEffect = NULL;
  
  if( pSubMeshSerializer.isvalid() )
  {
    DWORD dwMeshType = pSubMeshSerializer->GetEffectType();
    switch( dwMeshType )
    {
    case 1: // Billboard
      pParticleEffect = n_new(BillboardParticleEmitter( m_pParticlePool, pSubMeshSerializer, 0 ));
      break;
    case 2: // Billboard Y
      pParticleEffect = n_new(BillboardParticleEmitter( m_pParticlePool, pSubMeshSerializer, 1 ));
      break;
    case 3: // Ribbon
      pParticleEffect = n_new(RibbonParticleEmitter( m_pParticlePool, pSubMeshSerializer ));
      break;
    case 4: // Zodiac
      pParticleEffect = n_new(ZodiacParticleEmitter( m_pParticlePool, pSubMeshSerializer ));
      break;
    }
  }

  return pParticleEffect;
}

//-----------------------------------------------------------------------------
// Name: CreateParticleEffectWeaponRibbon
// Desc: 产生刀光特效
//-----------------------------------------------------------------------------
ParticleEmitter* 
ParticleServer::CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath )
{
  ParticleEmitter* pParticleEffect = n_new(WeaponRibbonParticleEmitter( m_pParticlePool, bDoLerp, vMinPos, vMaxPos ));
 
  return pParticleEffect;
}

//-----------------------------------------------------------------------------
// Name: ReleaseParticleEffect
// Desc: 睦采疭
//-----------------------------------------------------------------------------
//void ParticleServer::ReleaseParticleEffect( ParticleEmitter* pRelease )
//{
//  if( m_pParticleEffectList == NULL )
//  {
//    return;
//  }
//
//  ParticleEmitter* pParticleEffect = NULL;
//  int iCount;
//  
//  iCount = m_pParticleEffectList->GetCount();
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (ParticleEmitter*)m_pParticleEffectList->GetItem( i );
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
//    pParticleEffect = (ParticleEmitter*)m_pParticleEffectList->GetItem( i );
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
//void ParticleServer::FrameMove( float fElapsedTime )
//{
//  // 070412 cyhsieh 疭
//  if( m_bEnable == false )
//  {
//    return;
//  }
//
//  float fAdjustElapsedTime = fElapsedTime * 4800.0f;
//  ParticleEmitter* pParticleEffect = NULL;
//  int iCount = m_pParticleEffectList->GetCount();
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (ParticleEmitter*)m_pParticleEffectList->GetItem( i );
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
//  ParticleEmitter* pParticleEffect0 = (ParticleEmitter *)pArg0;
//  ParticleEmitter* pParticleEffect1 = (ParticleEmitter *)pArg1;
//
//  const D3DXVECTOR3& vCameraPos = ParticleServer::GetCameraPos();
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
//void ParticleServer::Draw( LPDIRECT3DDEVICE9 pD3DDevice )
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
//  ParticleEmitter* pParticleEffect = NULL;
//  int iCount;
//  iCount = m_pParticleEffectList->GetCount();
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (ParticleEmitter*)m_pParticleEffectList->GetItem( i );
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
//      pParticleEffect = (ParticleEmitter*)m_pParticleEffectSortList->GetItem( i );
//      if( pParticleEffect )
//      {
//        pParticleEffect->Draw( pD3DDevice );
//      }
//    }
//
//    m_pParticleEffectSortList->Clear();
//  }
//}

//-----------------------------------------------------------------------------
// Name: AddForceDrawParticleEffect
// Desc: 增加照片用粒子特效
//-----------------------------------------------------------------------------
//void ParticleServer::AddForceDrawParticleEffect( ParticleEmitter* pParticleEffect )
//{
//  if( m_pForceDrawParticleEffectList && pParticleEffect )
//  {
//    m_pForceDrawParticleEffectList->Add( pParticleEffect );
//  }
//}

//-----------------------------------------------------------------------------
// Name: ForceDraw
// Desc: 强制画(给照片用)
//-----------------------------------------------------------------------------
// 071218 cyhsieh 人物照片
//void ParticleServer::ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice )
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
//  ParticleEmitter* pParticleEffect = NULL;
//  for( int i = 0; i < iCount; i++ )
//  {
//    pParticleEffect = (ParticleEmitter*)m_pForceDrawParticleEffectList->GetItem( i );
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
//      pParticleEffect = (ParticleEmitter*)m_pParticleEffectSortList->GetItem( i );
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