//------------------------------------------------------------------------------
//  cMaterialAnimator.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "cMaterialAnimator.h"

namespace KOK
{

cTransform33::cTransform33()
:m_flags(0), m_vTranslation(0.0f, 0.0f), m_fRotateZ(0.0f), 
 m_vScale(1.0f, 1.0f)
{
}

void cTransform33::SetTranslation( const D3DXVECTOR2& vTranslation )
{
  m_vTranslation = vTranslation;
}

void cTransform33::SetRotateZ( float fRotateZ )
{
  m_fRotateZ = fRotateZ;
  //m_flags |= eDirty;
}

void cTransform33::SetScale( const D3DXVECTOR2& vScale )
{
  m_vScale = vScale;
}

const D3DXMATRIXA16& cTransform33::GetMatrix44( void )
{
  Update();
  return m_mat;
}

void cTransform33::Update()
{
  D3DXMatrixIdentity( &m_mat );
  
  if( ( m_flags & eRotateZ ) || ( m_flags & eScale ) )
  {
    m_mat._31 = -0.5f;
    m_mat._32 = -0.5f;
  }

  if( m_flags & eTranslation )
  {
    m_mat._31 -= m_vTranslation.x;
    m_mat._32 += m_vTranslation.y;
  }

  if( m_flags & eScale )
  {
    m_mat._11 = m_vScale.x;
    m_mat._22 = m_vScale.y;
    m_mat._31 *= m_vScale.x;
    m_mat._32 *= m_vScale.y;
  }

  if( m_flags & eRotateZ )
  {
    float fCos = cosf( m_fRotateZ );
    float fSin = sinf( m_fRotateZ );
    float f11 = m_mat._11;
    float f22 = m_mat._22;
    float f31 = m_mat._31;
    float f32 = m_mat._32;
    m_mat._11 = f11 * fCos;
    m_mat._12 = f11 * fSin;
    m_mat._21 = f22 * -fSin;
    m_mat._22 = f22 * fCos;
    m_mat._31 = f31 * fCos - f32 * fSin;
    m_mat._32 = f31 * fSin + f32 * fCos;
  }

  if( ( m_flags & eRotateZ ) || ( m_flags & eScale ) )
  {
    m_mat._31 += 0.5f;
    m_mat._32 += 0.5f;
  }
}


cMaterialAnimator::cMaterialAnimator()
{
}

void cMaterialAnimator::AddPosKey( float fTime, D3DXVECTOR2& vVec )
{
  m_posKeys.AddKey( fTime, vVec );
}

void cMaterialAnimator::AddRotateZKey( float fTime, float fValue )
{
  m_rotateZKeys.AddKey( fTime, fValue );
}

void cMaterialAnimator::AddScaleKey( float fTime, D3DXVECTOR2& vVec )
{
  m_scaleKeys.AddKey( fTime, vVec );
}

// 070205 cyhsieh extended material animation diffuse color
void cMaterialAnimator::AddDiffuseOpacityKey( float fTime, D3DXCOLOR& color )
{
  m_diffuseOpacityKeys.AddKey( fTime, color );
}

void cMaterialAnimator::SampleTextureTransform( float fTime, cTransform33* pTransform )
{
  if( pTransform == NULL )
  {
    return;
  }

  static cAnimKey<D3DXVECTOR2> posKey;
  static cAnimKey<float> rotateZKey;
  static cAnimKey<D3DXVECTOR2> scaleKey;
  if( m_posKeys.Sample( fTime, posKey ) )
  {
    pTransform->SetTranslation( posKey.GetValue() );
  }
  if( m_rotateZKeys.Sample( fTime, rotateZKey ) )
  {
    pTransform->SetRotateZ( rotateZKey.GetValue() );
  }
  if( m_scaleKeys.Sample( fTime, scaleKey ) )
  {
    pTransform->SetScale( scaleKey.GetValue() );
  }

}

// 070205 cyhsieh extended material animation diffuse color
void cMaterialAnimator::SampleDiffuseOpacity( float fTime, D3DXCOLOR* pDiffuseOpacity )
{
  if( pDiffuseOpacity == NULL )
  {
    return;
  }

  static cAnimKey<D3DXCOLOR> diffuseOpacityKey;
  if( m_diffuseOpacityKeys.Sample( fTime, diffuseOpacityKey ) )
  {
    *pDiffuseOpacity = diffuseOpacityKey.GetValue();
  }
}

}