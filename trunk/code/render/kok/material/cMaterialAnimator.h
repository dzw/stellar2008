#ifndef cMaterialAnimatorH
#define cMaterialAnimatorH
//------------------------------------------------------------------------------
/**
@class KOK::cMaterial

(C) 2008 cTuo
*/

#include "util/array.h"

namespace KOK
{

// extended material animation
class cTransform33
{
public:
  cTransform33();
  void SetTranslation( const D3DXVECTOR2& vTranslation );
  void SetRotateZ( float fRotateZ );
  void SetScale( const D3DXVECTOR2& vScale );
  const D3DXMATRIXA16& GetMatrix44( void );

  void SetUseTranslation( void ) { m_flags |= eTranslation; }
  void SetUseRotateZ( void ) { m_flags |= eRotateZ; }
  void SetUseScale( void ) { m_flags |= eScale; }
protected:
  void Update();

  enum
  {
    eTranslation  = ( 1 << 0 ),
    eRotateZ      = ( 1 << 1 ),
    eScale        = ( 1 << 2 ),
  };

  D3DXVECTOR2   m_vTranslation;
  float         m_fRotateZ;
  D3DXVECTOR2   m_vScale;
  D3DXMATRIXA16 m_mat;
  char          m_flags;
};

template<class TYPE>
inline void lerp( TYPE& result, const TYPE& val0, const TYPE& val1, float fLerpVal )
{
}

template<>
inline void lerp<float>( float& fResult, const float& f0, const float& f1, float fLerpVal )
{
    fResult = f0 + ( ( f1 - f0 ) * fLerpVal );
}

template<>
inline void lerp<D3DXVECTOR2>( D3DXVECTOR2& vResult, const D3DXVECTOR2& v0, const D3DXVECTOR2& v1, float fLerpVal )
{
  vResult.x = v0.x + ( ( v1.x - v0.x ) * fLerpVal );
  vResult.y = v0.y + ( ( v1.y - v0.y ) * fLerpVal );
}

// 070205 cyhsieh extended material animation diffuse color
template<>
inline void lerp<D3DXCOLOR>( D3DXCOLOR& colorResult, const D3DXCOLOR& color0, const D3DXCOLOR& color1, float fLerpVal )
{
  D3DXColorLerp( &colorResult, &color0, &color1, fLerpVal );
}

template<class TYPE> class cAnimKey
{
public:
  cAnimKey();
  cAnimKey( float fTime, const TYPE& value );
  void SetTime( float fTime );
  float GetTime( void ) const;
  void SetValue( const TYPE& value );
  const TYPE& GetValue( void ) const;
  void Lerp( const TYPE& key0, const TYPE& key1, float l );
private:
  float m_fTime;
  TYPE  m_value;
};

template<class TYPE>
cAnimKey<TYPE>::cAnimKey()
:m_fTime(0.0f)
{
}

template<class TYPE>
cAnimKey<TYPE>::cAnimKey( float fTime, const TYPE& value )
:m_fTime(fTime), m_value(value)
{
}

template<class TYPE>
void cAnimKey<TYPE>::SetTime( float fTime )
{
  m_fTime = fTime;
}

template<class TYPE>
float cAnimKey<TYPE>::GetTime( void ) const
{
  return m_fTime;
}

template<class TYPE>
void cAnimKey<TYPE>::SetValue( const TYPE& value )
:m_value(value)
{
}

template<class TYPE>
const TYPE& cAnimKey<TYPE>::GetValue( void ) const
{
  return m_value;
}

template<class TYPE>
void cAnimKey<TYPE>::Lerp( const TYPE& key0, const TYPE& key1, float fLerpVal )
{
  lerp<TYPE>( m_value, key0, key1, fLerpVal );
}

template<class TYPE> class cAnimKeyList
{
public:
  cAnimKeyList();
  ~cAnimKeyList();

  void      AddKey( float fTime, const TYPE& value );
  DWORD     GetKeyCnt( void );
  bool      Sample( float fTime, cAnimKey<TYPE>& result );
protected:
	Util::Array<cAnimKey<TYPE>*>   m_pKeyList;
};
template<class TYPE>
cAnimKeyList<TYPE>::cAnimKeyList()
{
  m_pKeyList.Clear();
}
template<class TYPE>
cAnimKeyList<TYPE>::~cAnimKeyList()
{
    int iCount = m_pKeyList.Size();
    cAnimKey<TYPE>* pKey;
    for( int i = 0; i < iCount; i++ )
    {
      pKey = (cAnimKey<TYPE>*)m_pKeyList[i];
      n_delete( pKey );
    }
    m_pKeyList.Clear();
}
template<class TYPE>
void cAnimKeyList<TYPE>::AddKey( float fTime, const TYPE& value )
{
    cAnimKey<TYPE>* pKey = n_new(cAnimKey<TYPE>( fTime, value ));
    m_pKeyList.Append( pKey );
 }
template<class TYPE>
DWORD cAnimKeyList<TYPE>::GetKeyCnt( void )
{
  return m_pKeyList.Size();
}
template<class TYPE>
bool cAnimKeyList<TYPE>::Sample( float fTime, cAnimKey<TYPE>& result )
{
    int iCount = m_pKeyList.Size();
    if( iCount > 1 )
    {
      cAnimKey<TYPE>* pKey1;
      cAnimKey<TYPE>* pKey2;
      pKey2 = (cAnimKey<TYPE>*)m_pKeyList[iCount - 1];
      float fEndTime = pKey2->GetTime();
      fTime = fmod( fTime, fEndTime );

      int i;
      for( i = 0; i < iCount; i++ )
      {
        pKey2 = (cAnimKey<TYPE>*)m_pKeyList[i];
        if( pKey2->GetTime() > fTime )
        {
          break;
        }
      }

      assert( i > 0 && i < iCount );
      pKey1 = (cAnimKey<TYPE>*)m_pKeyList[i - 1];

      float fTime1 = pKey1->GetTime();
      float fTime2 = pKey2->GetTime();
      float fLerpTime = ( fTime - fTime1 ) / ( fTime2 - fTime1 );
      result.Lerp( pKey1->GetValue(), pKey2->GetValue(), fLerpTime );
      result.SetTime( fTime );
      
      return true;
    }
  

  return false;
}

class cMaterialAnimator
{
protected:
  cAnimKeyList<D3DXVECTOR2> m_posKeys;
  cAnimKeyList<float> m_rotateZKeys;
  cAnimKeyList<D3DXVECTOR2> m_scaleKeys;

  // 070205 cyhsieh extended material animation diffuse color
  cAnimKeyList<D3DXCOLOR> m_diffuseOpacityKeys;

  // 070212 cyhsieh extended material animation texture address
  DWORD m_dwAddressFlag;
  
public:
  cMaterialAnimator();

  void AddPosKey( float fTime, D3DXVECTOR2& vVec );
  void AddRotateZKey( float fTime, float fValue );
  void AddScaleKey( float fTime, D3DXVECTOR2& vVec );
  
  // 070205 cyhsieh extended material animation diffuse color
  void AddDiffuseOpacityKey( float fTime, D3DXCOLOR& color );

  DWORD GetPosKeyCnt( void ) { return m_posKeys.GetKeyCnt(); }
  DWORD GetRotateZKeyCnt( void ) { return m_rotateZKeys.GetKeyCnt(); }
  DWORD GetScaleKeyCnt( void ) { return m_scaleKeys.GetKeyCnt(); }

  // 070205 cyhsieh extended material animation diffuse color
  DWORD GetDiffuseOpacityKeyCnt( void ) { return m_diffuseOpacityKeys.GetKeyCnt(); }

  void SampleTextureTransform( float fTime, cTransform33* pTransform );

  // 070205 cyhsieh extended material animation diffuse color
  void SampleDiffuseOpacity( float fTime, D3DXCOLOR* pDiffuseOpacity );

  // 070212 cyhsieh extended material animation texture address
  void SetAddressFlag( DWORD dwFlag ) { m_dwAddressFlag = dwFlag; }
  DWORD GetAddressFlag( void ) { return m_dwAddressFlag; }
};

}

#endif