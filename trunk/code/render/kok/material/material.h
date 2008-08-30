#ifndef KOK_MATERIAL_H
#define KOK_MATERIAL_H
//------------------------------------------------------------------------------
/**
	@class KOK::cMaterial

	(C) 2008 cTuo
*/

// extended material animation
#include "cMaterialAnimator.h"
#include "util/string.h"
#include "resources/managedtexture.h"
#include "coregraphics/texture.h"

namespace KOK
{

class cMaterial
{
// extended material animation
protected:
  float              m_fMaterialAniTime;
  cTransform33*      m_pTextureTransform;
  cMaterialAnimator* m_pMaterialAnimator;
  // 070205 cyhsieh extended material animation diffuse color
  D3DXCOLOR*         m_pDiffuseOpacity;
  // 070205 cyhsieh extended material property
  DWORD m_dwAlphaBlendType;
  DWORD m_dwTextureAniRows;
  DWORD m_dwTextureAniCols;
  DWORD m_dwTextureAniRepeat;

public:
	D3DMATERIAL9 m_D3DMaterial;                               // 材质
	Util::String m_pszTextName;                               // 贴图档名
	int  m_iNameSize;                                         // 贴图档名长度
	unsigned int iNo;                                         // 贴图的编号
	//cTexture *m_pTexture;                                   // 贴图的指标
	Ptr<Resources::ManagedTexture> m_pTexture;

  cMaterial();
  ~cMaterial();

  // 070205 extended material animation
  void FrameMove( float fTime );
  void SetMaterialAnimator( cMaterialAnimator* pMaterialAnimator );
  const D3DXMATRIXA16* GetTextureTransformMatrix( void );
  // 070205 extended material animation diffuse color
  const D3DXCOLOR* GetDiffuseOpacity( void ) { return m_pDiffuseOpacity; }
  void SetAniTime( float fTime );

  // 070205 extended material property
  DWORD GetAlphaBlendType( void ) { return m_dwAlphaBlendType; }
  void SetAlphaBlendType( DWORD dwType ) { m_dwAlphaBlendType = dwType; }
  DWORD GetTextureAniRows( void ) { return m_dwTextureAniRows; }
  void SetTextureAniRows( DWORD dwAniRows ) { m_dwTextureAniRows = dwAniRows; }
  DWORD GetTextureAniCols( void ) { return m_dwTextureAniCols; }
  void SetTextureAniCols( DWORD dwAniCols ) { m_dwTextureAniCols = dwAniCols; }
  DWORD GetTextureAniRepeat( void ) { return m_dwTextureAniRepeat; }
  void SetTextureAniRepeat( DWORD dwAniRepeat ) { m_dwTextureAniRepeat = dwAniRepeat; }

  // 070212 cyhsieh extended material animation texture address
  void GetAnimatorAddress( DWORD& dwAddressU, DWORD& dwAddressV );

  const Ptr<CoreGraphics::Texture>& GetTexture()const;
  void LoadTexture(const Util::String& filePath);
};

inline const Ptr<CoreGraphics::Texture>& 
cMaterial::GetTexture()const
{
	static Ptr<CoreGraphics::Texture> tmp;

	if (m_pTexture.isvalid())
		return m_pTexture->GetTexture();
	return tmp;
}

}

#endif