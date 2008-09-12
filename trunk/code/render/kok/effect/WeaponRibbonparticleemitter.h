#pragma once
#ifndef KOK_WEAPONPARTICLEEMITTER_H
#define KOK_WEAPONPARTICLEEMITTER_H
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
////////////////////////////////////////////////////////////////////////////////////////////////
// 武器刀光
// 给武器刀光用，武器会有特殊给刀光用的两个控制点，以这两个控制点连成Strip
class WeaponRibbonParticleEmitter : public ParticleEmitter
{
public:
  WeaponRibbonParticleEmitter( cParticlePool* pParticlePool, bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos );
  ~WeaponRibbonParticleEmitter();

  virtual void FrameMove( float fElapsedTime );
  virtual void DisableFrameMove( float fElapsedTime );

  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );

protected:
  bool        m_bAddSegment;
  D3DXVECTOR3 m_vOrigMinPos;
  D3DXVECTOR3 m_vOrigMaxPos;
  D3DXVECTOR3 m_vMinPos;
  D3DXVECTOR3 m_vMaxPos;

  struct sLerpInfo
  {
    D3DXMATRIXA16   m_matCurTransform;  // 目前的transform
    D3DXVECTOR3     m_vLastMaxPos;      // 上次的位置
    D3DXVECTOR3     m_vLastScale;       // 上次transform的scale
    D3DXQUATERNION  m_quatLastRotation; // 上次transform的rotation
    D3DXVECTOR3     m_vLastTranslation; // 上次transform的translation
  };
  sLerpInfo*  m_pLerpInfo;  // Lerp用资讯
};

}

#endif