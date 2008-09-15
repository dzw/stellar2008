#pragma once
#ifndef KOK_SUPERSPRAYPARTICLEEMITTER_H
#define KOK_SUPERSPRAYPARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
	粒子发射器

	(C) 2008 cTuo
*/
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"
#include "kok/effect/particledata.h"
#include "kok/effect/particleemitter.h"

namespace KOK
{

// particle superspray
class SuperSprayParticleEmitter : public ParticleEmitter
{
public:
	SuperSprayParticleEmitter( cParticlePool* pParticlePool, SuperSprayEmitData* pEmitDataSuperSpray );

	virtual void FrameMove( float fElapsedTime );
	virtual void DisableFrameMove( float fElapsedTime );
	virtual int RenderParticles(float* dstVertices, int maxVertices);

	virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );

	// 070213 cyhsieh Particle Emit Time
	virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }

	protected:
	sSuperSprayInfo m_sSuperSprayInfo;

	float       m_fTimeSinceLastEmit;

	// 070213 cyhsieh Particle Emit Time
	float       m_fCurAnimTime;

	bool        m_bEmitParticles;

	D3DXVECTOR3 m_vEmitDirRight;
	D3DXVECTOR3 m_vEmitDirForward;
	D3DXVECTOR3 m_vEmitForceDir;

	// 070214 cyhsieh spray喷射方向
	D3DXMATRIXA16 m_matTransForm;
};

}

#endif