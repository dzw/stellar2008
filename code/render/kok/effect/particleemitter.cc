//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/particledata.h"
#include "kok/effect/particleemitter.h"

namespace KOK
{
using namespace IO;
using namespace Memory;
using namespace Util;

ParticleEmitter::ParticleEmitter( cParticlePool* pParticlePool, EmitterData* data )
:m_pParticlePool(pParticlePool), m_pFirstUsedParticle(NULL), m_pLastUsedParticle(NULL),
m_pLinkName(NULL), m_vCurPosition( 0.0f, 0.0f, 0.0f ),m_dwRenderMeshSize(0),
m_dwVertexSize(0), m_pRenderMesh(0), emitData(data)
{
}

ParticleEmitter::~ParticleEmitter()
{
	// Free Particle
	cParticle* pParticle = m_pFirstUsedParticle;
	while( pParticle )
	{
		cParticle* pNext = pParticle->m_pNextParticle;
		FreeParticle( pParticle );

		pParticle = pNext;
	}

	if (m_pRenderMesh)
		n_delete_array(m_pRenderMesh);
}

void ParticleEmitter::AddParticle( cParticle* pParticle )
{
  cParticle* pLast = m_pLastUsedParticle;
	if( pLast )
	{
		pLast->m_pNextParticle = pParticle;
		m_pLastUsedParticle = pParticle;
		pParticle->m_pPrevParticle = pLast;
		pParticle->m_pNextParticle = NULL;
	}
	else
	{
		m_pFirstUsedParticle = pParticle;
		m_pLastUsedParticle = pParticle;
		pParticle->m_pPrevParticle = NULL;
		pParticle->m_pNextParticle = NULL;
	}
}

void ParticleEmitter::AddParticleFirst( cParticle* pParticle )
{
  cParticle* pFirst = m_pFirstUsedParticle;
	if( pFirst )
	{
    pFirst->m_pPrevParticle = pParticle;
    m_pFirstUsedParticle = pParticle;
    pParticle->m_pPrevParticle = NULL;
    pParticle->m_pNextParticle = pFirst;
	}
	else
	{
		m_pFirstUsedParticle = pParticle;
		m_pLastUsedParticle = pParticle;
		pParticle->m_pPrevParticle = NULL;
		pParticle->m_pNextParticle = NULL;
	}
}

void ParticleEmitter::FreeParticle( cParticle* pParticle )
{
  cParticle* pPrev = pParticle->m_pPrevParticle;
  cParticle* pNext = pParticle->m_pNextParticle;
		
	if( m_pFirstUsedParticle == pParticle ) m_pFirstUsedParticle = pNext;
	if( m_pLastUsedParticle == pParticle ) m_pLastUsedParticle = pPrev;
	
  if( pPrev ) pPrev->m_pNextParticle = pNext;
	if( pNext ) pNext->m_pPrevParticle = pPrev;
	
	
	cParticle* pLast = m_pParticlePool->m_pLastFreeParticle;
	if( pLast )
	{
		pLast->m_pNextParticle = pParticle;
		m_pParticlePool->m_pLastFreeParticle = pParticle;
		pParticle->m_pPrevParticle = pLast;
		pParticle->m_pNextParticle = NULL;
	}
	else
	{
		m_pParticlePool->m_pFirstFreeParticle = pParticle;
		m_pParticlePool->m_pLastFreeParticle = pParticle;
		pParticle->m_pPrevParticle = NULL;
		pParticle->m_pNextParticle = NULL;
	}
}

//void ParticleEmitter::Draw( LPDIRECT3DDEVICE9 pD3DDevice )
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

void ParticleEmitter::SetLinkName(const String& pName)
{
	m_pLinkName = pName;
}

DWORD ParticleEmitter::GetRenderAlphaBlendType( void )
{
  /*if( m_pRender )
  {
    return m_pRender->GetMaterialAlphaBlendType();
  }*/
  return 0;
}

float ParticleEmitter::ComputeFogAlphaFactor( void )
{
  float fFogAlphaFactor = 0.0f;
  
  /*if( m_pRender && g_mFogManage )
  {
    float fFogStart = g_mFogManage->GetFogStart();
    float fFogEnd = g_mFogManage->GetFogEnd();

    D3DXVECTOR3 vToCamera = ParticleServer::GetCameraPos() - m_vCurPosition;
    float fDistanceToCamera = D3DXVec3Length( &vToCamera );

    if( fDistanceToCamera <= fFogStart )
    {
      fFogAlphaFactor = 1.0f;
    }
    else if( fDistanceToCamera >= fFogEnd )
    {
      fFogAlphaFactor = 0.0f;
    }
    else
    {
      fFogAlphaFactor = ( fFogEnd - fDistanceToCamera ) / ( fFogEnd - fFogStart );
    }
  }*/

  return fFogAlphaFactor;
}

void 
ParticleEmitter::Draw()
{
	float* dstVertices = 0;
	int maxVertices = 0;
	int remVertices = 0;
	remVertices = RenderParticles(dstVertices, maxVertices);
}

}