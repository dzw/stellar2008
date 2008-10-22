#pragma once
#ifndef KOK_BILLBOARDPARTICLEEMITTER_H
#define KOK_BILLBOARDPARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
	Á£×Ó·¢ÉäÆ÷

	(C) 2008 cTuo
*/
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"
#include "kok/effect/particledata.h"
#include "kok/model/kokshapenode.h"
#include "kok/effect/particleemitter.h"

namespace KOK
{
// billboard
class BillboardParticleEmitter : public ParticleEmitter
{
public:
  BillboardParticleEmitter( cParticlePool* pParticlePool, const Ptr<KokShapeNode>& pSubMeshSerializer, DWORD dwBillboardType );

  virtual void FrameMove( float fElapsedTime );

  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );

  // Billboard Animation Time
  virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }

protected:
  D3DXVECTOR3 m_vCorner[3];
  float       m_fWidth;
  DWORD       m_dwBillboardType;

  // Billboard Animation Time
  float       m_fCurAnimTime;
};

}

#endif