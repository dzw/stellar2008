#pragma once
#ifndef KOK_RIBBONPARTICLEEMITTER_H
#define KOK_RIBBONPARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
	Á£×Ó·¢ÉäÆ÷

	(C) 2008 cTuo
*/
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"
#include "kok/effect/particledata.h"
#include "kok/effect/particleemitter.h"
#include "kok/model/kokshapenode.h"

namespace KOK
{

// cyhsieh ribbon
class RibbonParticleEmitter : public ParticleEmitter
{
public:
  RibbonParticleEmitter( cParticlePool* pParticlePool, const Ptr<KokShapeNode>& pSubMeshSerializer );

  virtual void FrameMove( float fElapsedTime );
  virtual void DisableFrameMove( float fElapsedTime );

  virtual void InitTransformMatrix( D3DXMATRIXA16* pTransform );
  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );

protected:
  D3DXVECTOR3 m_vDirUp;
  D3DXVECTOR3 m_vLastPos;
  float       m_fAddSegmentLength;
  float       m_fMaxRibbonLength;
  float       m_fRibbonWidth;
};

}

#endif