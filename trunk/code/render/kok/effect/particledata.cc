//------------------------------------------------------------------------------
//  particledata.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/particledata.h"
#include "io/stream.h"
#include "memory/memory.h"

namespace KOK
{
using namespace IO;
using namespace Memory;

inline  void 
ReadString(const Ptr<Stream>& stream, Util::String& str, int len)
{
	Memory::MemoryMark mm(Memory::MemStack);

	char* buf = new(Memory::MemStack, len+1)char;
	stream->Read(buf, len);
	buf[len] = 0;
	str = buf;
}

EmitterData::EmitterData()
	:m_dwDataType(0), 
	m_dwNumMaterial(0), 
	m_pMaterial(NULL), 
	m_pMaterialAnimator(NULL)
{
}

EmitterData::~EmitterData()
{
	n_delete_array( m_pMaterial );
	n_delete_array( m_pMaterialAnimator );
}

void EmitterData::ImportMaterialFromMemory( const Ptr<Stream>& stream, int iIndex )
{
	// 贴图长度
	int iTextureNameLength = 0;
	stream->Read(&iTextureNameLength, sizeof(int));
	m_pMaterial[iIndex].m_iNameSize = iTextureNameLength;
	// 贴图名称
	ReadString(stream, m_pMaterial[iIndex].m_pszTextName, iTextureNameLength);

	// 材质
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.r = m_pMaterial[iIndex].m_D3DMaterial.Ambient.r = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.g = m_pMaterial[iIndex].m_D3DMaterial.Ambient.g = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.b = m_pMaterial[iIndex].m_D3DMaterial.Ambient.b = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.a = m_pMaterial[iIndex].m_D3DMaterial.Ambient.a = 1.0f;

	// Set the RGBA for diffuse reflection.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.r, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.g, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.b, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse.a, sizeof(float));

	// Set the color and sharpness of specular highlights.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Power,      sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.r, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.g, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.b, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular.a, sizeof(float));

	// Set the RGBA for emissive color.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.r, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.g, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.b, sizeof(float));
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive.a, sizeof(float));

	DWORD dwAlphaBlendType;
	stream->Read(&dwAlphaBlendType, sizeof(DWORD));
	m_pMaterial[iIndex].SetAlphaBlendType( dwAlphaBlendType );

	DWORD dwTextureAniRows;
	stream->Read(&dwTextureAniRows, sizeof(DWORD));
	m_pMaterial[iIndex].SetTextureAniRows( dwTextureAniRows );

	DWORD dwTextureAniCols;
	stream->Read(&dwTextureAniCols, sizeof(DWORD));
	m_pMaterial[iIndex].SetTextureAniCols( dwTextureAniCols );

	DWORD dwTextureAniRepeat;
	stream->Read(&dwTextureAniRepeat, sizeof(DWORD));
	m_pMaterial[iIndex].SetTextureAniRepeat( dwTextureAniRepeat );

	ImportMaterialAnimationFromMemory( stream, iIndex );
}

void EmitterData::ImportMaterialAnimationFromMemory( const Ptr<Stream>& stream, int iIndex )
{
	n_assert(stream.isvalid());
	MemoryMark mm(MemStack);

	DWORD dwKey;
	DWORD dwType;
	DWORD dwTotalKeys;
	//sMaterialAnimationKey matAniKey;
	D3DXVECTOR2 vPos;
	D3DXVECTOR2 vScale;
	float* pTimeTmp = NULL;
	D3DXVECTOR2* pVecTmp = NULL;
	float* pFloatTemp = NULL;
	// extended material animation diffuse color
	D3DXCOLOR* pColorTemp = NULL;

	do
	{
		stream->Read(&dwType, sizeof(DWORD));

		switch( dwType )
		{
		case 1: // offset
			stream->Read(&dwTotalKeys, sizeof(DWORD));

			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pVecTmp = new(MemStack) D3DXVECTOR2[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pVecTmp, sizeof(D3DXVECTOR2)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddPosKey( pTimeTmp[dwKey], pVecTmp[dwKey] );
			}
			break;
		case 2: // tiling
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pVecTmp = new(MemStack) D3DXVECTOR2[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pVecTmp, sizeof(D3DXVECTOR2)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddScaleKey( pTimeTmp[dwKey], pVecTmp[dwKey] );
			}
			break;
		case 3: // wangle
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pFloatTemp = new(MemStack) float[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pFloatTemp, sizeof(float)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddRotateZKey( pTimeTmp[dwKey], pFloatTemp[dwKey] );
			}
			break;
			// extended material animation diffuse color
		case 4: // diffuse & opacity
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pColorTemp = new(MemStack)D3DXCOLOR[dwTotalKeys];
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddDiffuseOpacityKey( pTimeTmp[dwKey], pColorTemp[dwKey] );
			}
			break;
			// extended material animation texture address
		case 5:
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			m_pMaterialAnimator[iIndex].SetAddressFlag( dwTotalKeys );
			break;
		}

	} while( dwType != 0 );
}

cMaterial* EmitterData::GetMaterial( DWORD dwIndex )
{
	if( m_pMaterial && dwIndex < m_dwNumMaterial )
	{
		return &m_pMaterial[dwIndex];
	}

	return NULL;
}

cMaterialAnimator* EmitterData::GetMaterialAnimator( DWORD dwIndex )
{
	if( m_pMaterialAnimator && dwIndex < m_dwNumMaterial )
	{
		return &m_pMaterialAnimator[dwIndex];
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
sPCloudInfo::sPCloudInfo()
	:m_dwFormation(0), m_fMoveSpeed(0.0f), m_fMoveSpeedVariation(0.0f), m_dwMoveType(0),
	m_dwBirthMethod(0), m_dwBirthRate(0), m_dwBirthTotal(0), m_dwEmitStart(0),
	m_dwEmitStop(0), m_dwLife(0), m_dwLifeVariation(0), m_fSize(0.0f), m_fSizeVariation(0.0f),
	m_dwGrowTime(0), m_dwFadeTime(0), m_fEmitWidth(0.0f), m_fEmitHeight(0.0f),
	m_fEmitDepth(0.0f), m_dwSpinTime(0), m_fSpinVariation(0.0f), m_fSpinPhaseDeg(0.0f),
	m_fSpinPhaseDegVariation(0.0f)
{
}



////////////////////////////////////////////////////////////////////////////////////////////////
CloudEmitterData::CloudEmitterData()
{
	m_dwDataType = 1;
}

void CloudEmitterData::ImportFromMemory( const Ptr<Stream>& stream )
{
	// data
	stream->Read(&m_sPCloudInfo, sizeof(sPCloudInfo));

	// link
	int iLength;
	stream->Read(&iLength, sizeof(int));
	ReadString(stream, m_pLinkName, iLength);

	// material
	stream->Read(&m_dwNumMaterial, sizeof(DWORD));

	n_delete_array( m_pMaterial );
	n_delete_array( m_pMaterialAnimator );
	if( m_dwNumMaterial > 0 )
	{
		m_pMaterial = n_new_array(cMaterial, m_dwNumMaterial);
		m_pMaterialAnimator = n_new_array(cMaterialAnimator, m_dwNumMaterial);

		for( unsigned int i = 0; i < m_dwNumMaterial; i++ )
		{
			ImportMaterialFromMemory( stream, i );
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh particle superspray
sSuperSprayInfo::sSuperSprayInfo()
	:m_fOffAxisDeg(0.0f), m_fOffAxisSpreadDeg(0.0f),
	m_fOffPlaneDeg(0.0f), m_fOffPlaneSpreadDeg(0.0f),
	m_fMoveSpeed(0.0f), m_fMoveSpeedVariation(0.0f),
	m_dwBirthMethod(0), m_dwBirthRate(0), m_dwBirthTotal(0), m_dwEmitStart(0),
	m_dwEmitStop(0), m_dwLife(0), m_dwLifeVariation(0), m_fSize(0.0f), m_fSizeVariation(0.0f),
	m_dwGrowTime(0), m_dwFadeTime(0), m_fEmitWidth(0.0f),
	m_dwSpinTime(0), m_fSpinVariation(0.0f), m_fSpinPhaseDeg(0.0f),
	m_fSpinPhaseDegVariation(0.0f)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 070205 cyhsieh particle superspray
SuperSprayEmitterData::SuperSprayEmitterData()
{
	m_dwDataType = 2;
}

void SuperSprayEmitterData::ImportFromMemory( const Ptr<Stream>& stream )
{
	// data
	stream->Read(&m_sSuperSprayInfo, sizeof(sSuperSprayInfo));

	// link
	int iLength;
	stream->Read(&iLength, sizeof(int));

	if( iLength > 0 )
	{
		ReadString(stream, m_pLinkName, iLength);
	}

	// material
	stream->Read(&m_dwNumMaterial, sizeof(DWORD));

	n_delete_array( m_pMaterial );
	n_delete_array( m_pMaterialAnimator );

	if( m_dwNumMaterial > 0 )
	{
		m_pMaterial = n_new_array(cMaterial, m_dwNumMaterial);
		m_pMaterialAnimator = n_new_array(cMaterialAnimator, m_dwNumMaterial);

		for( unsigned int i = 0; i < m_dwNumMaterial; i++ )
		{
			ImportMaterialFromMemory( stream, i );
		}
	}
}

//-----------------------------------------------------------------------------
// Name: CreateSpecialMaterialAnimator
// Desc: 
//-----------------------------------------------------------------------------
void SuperSprayEmitterData::CreateSpecialMaterialAnimator( void )
{
	if( m_dwNumMaterial == 0 )
	{
		m_dwNumMaterial = 1;
		m_pMaterialAnimator = n_new_array(cMaterialAnimator, m_dwNumMaterial);
	}
}
}