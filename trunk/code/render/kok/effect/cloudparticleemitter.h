#pragma once
#ifndef KOK_CLOUDPARTICLEEMITTER_H
#define KOK_CLOUDPARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
	Á£×Ó·¢ÉäÆ÷

	(C) 2008 cTuo
*/
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"
#include "kok/effect/particledata.h"
#include "kok/effect/particleemitter.h"

namespace KOK
{
class CloudParticleEmitter : public ParticleEmitter
{
public:
	CloudParticleEmitter( cParticlePool* pParticlePool, CloudEmitterData* pEmitDataCloud );

	virtual void FrameMove( float fElapsedTime );
	virtual void DisableFrameMove( float fElapsedTime );
	virtual int RenderParticles(float* dstVertices, int maxVertices);

	virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );
	virtual void ApplyBoundingBox( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax );

	// 070213 cyhsieh Particle Emit Time
	virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }

	protected:
	sPCloudInfo m_sPCloudInfo;

	float       m_fTimeSinceLastEmit;

	// 070213 cyhsieh Particle Emit Time
	float       m_fCurAnimTime;

	bool        m_bEmitParticles;

	D3DXVECTOR3 m_vEmitDirRight;
	D3DXVECTOR3 m_vEmitDirUp;
	D3DXVECTOR3 m_vEmitDirForward;
	D3DXVECTOR3 m_vEmitForceDir;
	D3DXVECTOR3 m_vCenterOffset;

	D3DXMATRIXA16 m_matRotate;
};

}

#endif