#pragma once
#ifndef KOK_WEAPONPARTICLEEMITTER_H
#define KOK_WEAPONPARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
	���ӷ�����

	(C) 2008 cTuo
*/
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"
#include "kok/effect/particledata.h"
#include "kok/effect/particleemitter.h"

namespace KOK
{
////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
// �����������ã�������������������õ��������Ƶ㣬�����������Ƶ�����Strip
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
    D3DXMATRIXA16   m_matCurTransform;  // Ŀǰ��transform
    D3DXVECTOR3     m_vLastMaxPos;      // �ϴε�λ��
    D3DXVECTOR3     m_vLastScale;       // �ϴ�transform��scale
    D3DXQUATERNION  m_quatLastRotation; // �ϴ�transform��rotation
    D3DXVECTOR3     m_vLastTranslation; // �ϴ�transform��translation
  };
  sLerpInfo*  m_pLerpInfo;  // Lerp����Ѷ
};

}

#endif