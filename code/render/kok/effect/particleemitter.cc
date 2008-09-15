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

ParticleEmitter::ParticleEmitter( cParticlePool* pParticlePool )
:m_pParticlePool(pParticlePool), m_pFirstUsedParticle(NULL), m_pLastUsedParticle(NULL),
 m_pLinkName(NULL), m_vCurPosition( 0.0f, 0.0f, 0.0f )
{
	//创建mesh,没设置缓冲大小，暂时直接使用nDynamicMesh中定义的大小
	this->particleMesh = ParticleSystem::DynamicMesh::Create();
	n_assert(particleMesh);

	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	vertexComponents.Append(CoreGraphics::VertexComponent(CoreGraphics::VertexComponent::Position,0,CoreGraphics::VertexComponent::Float3));
	vertexComponents.Append(CoreGraphics::VertexComponent(CoreGraphics::VertexComponent::Color,0,CoreGraphics::VertexComponent::Float4));
	vertexComponents.Append(CoreGraphics::VertexComponent(CoreGraphics::VertexComponent::TexCoord,0,CoreGraphics::VertexComponent::Float2));

	//create resource ID
	Util::String resourceID = "particle_";
	resourceID.AppendInt((int)this);
	Util::Atom<Util::String> atomResourceID(resourceID);

	particleMesh->Initialize(atomResourceID,CoreGraphics::PrimitiveTopology::TriangleList,vertexComponents,false);

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

    D3DXVECTOR3 vToCamera = c3dsMaxParticleManager::GetCameraPos() - m_vCurPosition;
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
	this->particleMesh->Begin(dstVertices, maxVertices);
	remVertices = RenderParticles(dstVertices, maxVertices);
	this->particleMesh->End(remVertices);
}

}