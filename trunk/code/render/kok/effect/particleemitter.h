#pragma once
#ifndef KOK_PARTICLEEMITTER_H
#define KOK_PARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
	粒子发射器

	(C) 2008 cTuo
*/
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"
#include "kok/effect/particledata.h"
#include "coregraphics/dynamicmesh.h"

namespace KOK
{
struct cParticle
{
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vDir;
	D3DXCOLOR m_color;
	float        m_fAngle;
	float        m_fSize;
	float        m_fLifeTime;
	float        m_fTotalLifeTime;

	DWORD        m_dwTextureAniIndex;
	float        m_fTextureAniTime;

	cParticle*   m_pPrevParticle;
	cParticle*   m_pNextParticle;
};


class cParticlePool
{
	friend class ParticleEmitter;

public:
	cParticlePool( DWORD dwCount, void* pPool, DWORD dwDataSize );
	~cParticlePool();

	cParticle* NewParticle( void );

protected:
	char*       m_pParticlePool;
	DWORD       m_dwParticleDataSize;
	bool		m_bInternallyAllocated;
	DWORD       m_dwTotalParticleCount;
	DWORD       m_dwActiveParticleCount;
	cParticle*  m_pFirstFreeParticle;
	cParticle*  m_pLastFreeParticle;
};

class ParticleEmitter
{
public:
	ParticleEmitter( cParticlePool* pParticlePool );
	virtual ~ParticleEmitter();

	void AddParticle( cParticle* pParticle );
	void AddParticleFirst( cParticle* pParticle );
	void FreeParticle( cParticle* pParticle );

	virtual void FrameMove( float fElapsedTime ) = 0;
	virtual void DisableFrameMove( float fElapsedTime ) {}
	virtual void Draw( LPDIRECT3DDEVICE9 pD3DDevice );

	void SetCurPosition( D3DXVECTOR3& vPos ) { m_vCurPosition = vPos; }
	D3DXVECTOR3& GetCurPosition( void ) { return m_vCurPosition; }
	void SetLinkName( const Util::String& pName );
	const Util::String& GetLinkName( void ) { return m_pLinkName; }
	//c3dsMaxParticleRender* GetRender( void ) { return m_pRender; }

	virtual void InitTransformMatrix( D3DXMATRIXA16* pTransform ) {}
	virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform ) {}
	virtual void ApplyBoundingBox( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax ) {}

	// 070213 cyhsieh Particle Emit Time
	virtual void SetCurAnimTime( float fTime ) {}

	DWORD GetRenderAlphaBlendType( void );

	/*bool GetRenderEnabled( void )
	{
	if( m_pRender ) return m_pRender->bEnabled;
	return false;
	}*/

	float ComputeFogAlphaFactor( void );

protected:
	cParticlePool*  m_pParticlePool;

	cParticle*      m_pFirstUsedParticle;
	cParticle*      m_pLastUsedParticle;

	Util::String    m_pLinkName;
	D3DXVECTOR3     m_vCurPosition;

	//Ptr<CoreGraphics::Mesh> emitterMesh;
	Ptr<CoreGraphics::DynamicMesh> particleMesh;

	int srcBlend;
	int destBlend;

	/// 是否是alpha模式，如果是就需要放到alpha队列
	bool isAlphaMode;
};

}

#endif