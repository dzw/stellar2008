//------------------------------------------------------------------------------
//  cMaterial.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/material/material.h"
#include "resources/managedresource.h"
#include "resources/resourcemanager.h"

#include "io/lpqfs/lpqarchive.h"
#include "io/lpqfs/lpqfilesystem.h"

namespace KOK
{

using namespace Resources;
using namespace CoreGraphics;
using namespace Util;

cMaterial::cMaterial():
	m_pTexture(0),
	fakeReflectTexture(0)
{
  ZeroMemory( &m_D3DMaterial, sizeof(D3DMATERIAL9));
  //m_pszTextName  = NULL;
  //m_pTexture     = NULL;
  m_iNameSize    = 0 ;
  iNo            = 0 ;

  // 070205 cyhsieh extended material animation
  m_fMaterialAniTime = 0.0f;
  m_pTextureTransform = NULL;
  m_pMaterialAnimator = NULL;
  // 070205 cyhsieh extended material animation diffuse color
  m_pDiffuseOpacity = NULL;

  // 070205 cyhsieh extended material property
  m_dwAlphaBlendType = 0;
  m_dwTextureAniRows = 1;
  m_dwTextureAniCols = 1;
  m_dwTextureAniRepeat = 0;
}

//-----------------------------------------------------------------------------
// Name: ~cMaterial()
// Desc: cMaterial 摸秆篶じ
//-----------------------------------------------------------------------------
cMaterial::~cMaterial()
{
  //SAFE_RELEASE(m_pTexture);
  //n_delete_array( m_pszTextName );

  // 070205 cyhsieh extended material animation
  n_delete( m_pTextureTransform );
  m_pMaterialAnimator = NULL;

  // 070205 cyhsieh extended material animation diffuse color
  n_delete( m_pDiffuseOpacity );

  if (m_pTexture.isvalid())
    ResourceManager::Instance()->DiscardManagedResource(this->m_pTexture.upcast<ManagedResource>());
  if (this->fakeReflectTexture.isvalid())
	ResourceManager::Instance()->DiscardManagedResource(this->fakeReflectTexture.upcast<ManagedResource>());
}

// 070205 cyhsieh extended material animation
void cMaterial::FrameMove( float fTime )
{
  if( m_pMaterialAnimator == NULL )
  {
    return;
  }

  m_fMaterialAniTime += fTime * 4800.0f;
  if( m_pTextureTransform )
  {
    m_pMaterialAnimator->SampleTextureTransform( m_fMaterialAniTime, m_pTextureTransform );
  }

  // 070205 cyhsieh extended material animation diffuse color
  if( m_pDiffuseOpacity )
  {
    m_pMaterialAnimator->SampleDiffuseOpacity( m_fMaterialAniTime, m_pDiffuseOpacity );
  }
}

// 070205 cyhsieh extended material animation diffuse color
void cMaterial::SetAniTime( float fTime )
{
  if( m_pMaterialAnimator == NULL )
  {
    return;
  }

  m_fMaterialAniTime = fTime;
  if( m_pTextureTransform )
  {
    m_pMaterialAnimator->SampleTextureTransform( m_fMaterialAniTime, m_pTextureTransform );
  }

  // 070205 cyhsieh extended material animation diffuse color
  if( m_pDiffuseOpacity )
  {
    m_pMaterialAnimator->SampleDiffuseOpacity( m_fMaterialAniTime, m_pDiffuseOpacity );
  }
}

// 070205 cyhsieh extended material animation
void cMaterial::SetMaterialAnimator( cMaterialAnimator* pMaterialAnimator )
{
  n_delete( m_pTextureTransform );
  // 070205 cyhsieh extended material animation diffuse color
  n_delete( m_pDiffuseOpacity );
  m_pMaterialAnimator = NULL;

  if( pMaterialAnimator )
  {
    DWORD dwPosKeyCnt = pMaterialAnimator->GetPosKeyCnt();
    DWORD dwRotateZKeyCnt = pMaterialAnimator->GetRotateZKeyCnt();
    DWORD dwScaleKeyCnt = pMaterialAnimator->GetScaleKeyCnt();
    if( dwPosKeyCnt > 0 || dwRotateZKeyCnt > 0 || dwScaleKeyCnt > 0 )
    {
      m_pTextureTransform = n_new(cTransform33);
      if( m_pTextureTransform )
      {
        if( dwPosKeyCnt > 0 ) m_pTextureTransform->SetUseTranslation();
        if( dwRotateZKeyCnt > 0 ) m_pTextureTransform->SetUseRotateZ();
        if( dwScaleKeyCnt > 0 ) m_pTextureTransform->SetUseScale();
      }
    }

    // 070205 cyhsieh extended material animation diffuse color
    DWORD dwDiffuseOpacityCnt = pMaterialAnimator->GetDiffuseOpacityKeyCnt();
    if( dwDiffuseOpacityCnt > 0 )
    {
      m_pDiffuseOpacity = n_new(D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ));

      // 070302 cyhsieh extended material animation diffuse color
      if( m_pDiffuseOpacity )
      {
        pMaterialAnimator->SampleDiffuseOpacity( 0.0f, m_pDiffuseOpacity );
      }
    }
  }

  // 070205 cyhsieh extended material animation diffuse color
  if( m_pTextureTransform || m_pDiffuseOpacity )
  {
    m_pMaterialAnimator = pMaterialAnimator;
  }
}

// 070205 cyhsieh extended material animation
const D3DXMATRIXA16* cMaterial::GetTextureTransformMatrix( void )
{
  if( m_pTextureTransform )
  {
    return &m_pTextureTransform->GetMatrix44();
  }

  return NULL;
}

// 070212 cyhsieh extended material animation texture address
void cMaterial::GetAnimatorAddress( DWORD& dwAddressU, DWORD& dwAddressV )
{
  if( m_pMaterialAnimator )
  {
    DWORD dwFlag = m_pMaterialAnimator->GetAddressFlag();
    if( dwFlag & 1 )
    { 
      dwAddressU = D3DTADDRESS_WRAP;
    }
    else
    {
      dwAddressU = D3DTADDRESS_CLAMP;
    }

    if( dwFlag & 2 )
    {
      dwAddressV = D3DTADDRESS_WRAP;
    }
    else
    {
      dwAddressV = D3DTADDRESS_CLAMP;
    }
  }
  else
  {
    dwAddressU = D3DTADDRESS_WRAP;
    dwAddressV = D3DTADDRESS_WRAP;
  }
}

void 
cMaterial::LoadTexture(const String& filePath)
{
	/*if (m_pTexture.isvalid())
	{
		ResourceManager::Instance()->DiscardManagedResource(this->m_pTexture.upcast<ManagedResource>());
		m_pTexture = 0;
	}*/
	/*if (filePath.Length() > 0)
	{
		m_pTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, filePath).downcast<ManagedTexture>();
	}*/
}

void 
cMaterial::LoadFakeReflectTexture(const Util::String& filePath)
{
	//if (this->fakeReflectTexture.isvalid())
	//{
	//	ResourceManager::Instance()->DiscardManagedResource(this->fakeReflectTexture.upcast<ManagedResource>());
	//	this->fakeReflectTexture = 0;
	//}
	//if (filePath.Length() > 0)
	//{
	//	// 先判断反光贴图是否存在，只有存在的时候才加载，这地方有严重效率问题！！
	//	const Ptr<IO::LPQArchive>& pack =  IO::LPQFileSystem::Instance()->FindLPQArchiveByName(filePath);
	//	if (pack.isvalid())
	//		this->fakeReflectTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, filePath).downcast<ManagedTexture>();
	//}
}

}