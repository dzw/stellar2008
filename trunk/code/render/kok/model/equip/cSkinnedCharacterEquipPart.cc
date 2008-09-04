//------------------------------------------------------------------------------
//  thing.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "cSkinnedCharacterEquipPart.h"
#include "../Thread/ThreadingBuffer.h"

namespace KOK
{

// 装备特效
extern ID3DXMatrixStack* g_pStack;

//-----------------------------------------------------------------------------
// Name: cSkinnedCharacterEquipPart()
// Desc: cSkinnedCharacterEquipPart 建构元
//-----------------------------------------------------------------------------
cSkinnedCharacterEquipPart::cSkinnedCharacterEquipPart( BYTE byPartID, BYTE byLinkID )
{
	m_pSkeletonHierarchy = NULL;// 武器装备对应的框架

	D3DXMatrixIdentity( &m_matEqMatrix );

	//  钓竿
	m_iActionIndex = 0;
	m_byPartID = byPartID;
	m_byLinkID = byLinkID;

	// 坐下不显示武器
	m_bModelExist = false;
	m_bForseHide = false;

	// model effect
	m_pModelEffectList = _NEW BTList;

	m_pStatusEffect = NULL;

	// 武器刀光
	m_pWeaponRibbon = NULL;
	m_bShowWeaponRibbon = false;

	//   修改装备Model Render架构
	m_iTextureID = 0;
	m_pEquipModel = NULL;
	m_iRenderSize = 0;
	m_pRenderArray = NULL;

	// 装备特效
	m_dwParticleEffectSize = 0;
	m_ppParticleEffects = NULL;
	m_fCurrentAnimTime = 0.0f;
	m_fAnimElapsedTime = 0.0f;

	m_byWaitResourceCount = 0;
}

//-----------------------------------------------------------------------------
// Name: ~cSkinnedCharacterEquipPart()
// Desc: cSkinnedCharacterEquipPart 解构元
//-----------------------------------------------------------------------------
cSkinnedCharacterEquipPart::~cSkinnedCharacterEquipPart()
{
	// 装备特效
	if( g_p3dsMaxParticleManager )
	{
		// 释放刀光
		if( m_pWeaponRibbon )
		{
			g_p3dsMaxParticleManager->ReleaseParticleEffect( m_pWeaponRibbon );
			m_pWeaponRibbon = NULL;
		}

		// 释放其他粒子特效
		for( unsigned int i = 0; i < m_dwParticleEffectSize; i++ )
		{
			g_p3dsMaxParticleManager->ReleaseParticleEffect( m_ppParticleEffects[i] );
		}
		SAFE_DELETE_ARRAY( m_ppParticleEffects ); 
	}

	if( m_pRenderArray )
	{
		cEquipRender* pEquipRender;
		for( int i = 0; i < m_iRenderSize; i++ )
		{
			pEquipRender = (cEquipRender*)m_pRenderArray[i].m_pRender;
			if( pEquipRender == NULL )
			{
				continue;
			}
			pEquipRender->SetSubMeshSerializer( NULL );
		}
	}

	//   修改装备Model Render架构
	SAFE_DELETE_ARRAY( m_pRenderArray );
	SAFE_RELEASE( m_pEquipModel );

	SAFE_DELETE( m_pStatusEffect );

	// model effect
	if( m_pModelEffectList )
	{
		cModelEffect* pModelEffect;
		int iCount = m_pModelEffectList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pModelEffect = (cModelEffect*)m_pModelEffectList->GetItem( i );
			SAFE_DELETE( pModelEffect );
		}
		delete m_pModelEffectList;
	}

	m_pSkeletonHierarchy = NULL;
}

//-----------------------------------------------------------------------------
// Name: setEquipSkeletonHierarchy()
// Desc:  set cSkeletonHierarchy of equip or weapon for skinned character model.
//-----------------------------------------------------------------------------
void cSkinnedCharacterEquipPart::setEquipSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy )
{
	m_pSkeletonHierarchy = pSkeletonHierarchy;
}

//-----------------------------------------------------------------------------
// Name: getEquipSkeletonHierarchy()
// Desc:  get cSkeletonHierarchy of equip or weapon for skinned character model.
//-----------------------------------------------------------------------------
cSkeletonHierarchy* cSkinnedCharacterEquipPart::getEquipSkeletonHierarchy()
{
	return m_pSkeletonHierarchy;
}

//-----------------------------------------------------------------------------
// Name: getEquipVisible()
// Desc:  get visible of equip or weapon for skinned character model.
//-----------------------------------------------------------------------------
bool cSkinnedCharacterEquipPart::getEquipVisible()
{
	// 坐下不显示武器
	if( m_bForseHide )
	{
		return false;
	}

	return m_bModelExist;
}

//-----------------------------------------------------------------------------
// Name: setEquipMatrix()
// Desc:   set matrix of weapon for skinned character model by frame move.
//-----------------------------------------------------------------------------
// 武器装备
// 修正model配件会抖
void cSkinnedCharacterEquipPart::SetWorldTransform( const D3DXMATRIXA16&  matTransform )
{
	if( m_pEquipModel == NULL )
	{
		return;
	}

	// 修正model配件会抖
	// 如果skin有更新的话,就把matrix资料保存,没更新的时候拿来用
	if( m_pSkeletonHierarchy )
	{
		D3DXMatrixMultiply( &m_matEqMatrix, m_pSkeletonHierarchy->getSkeletonHierarchyMatrix(), &matTransform );
	}
	else
	{
		m_matEqMatrix = matTransform;
	}

	if( m_pWeaponRibbon && g_Options.GetShowFightEffect() ) // 有开启战斗特效
	{
		if( m_bShowWeaponRibbon )
		{
			m_pWeaponRibbon->ApplyTransformMatrix( &m_matEqMatrix );
		}
		else
		{
			m_pWeaponRibbon->ApplyTransformMatrix( NULL );
		}
		m_pWeaponRibbon->FrameMove( m_fAnimElapsedTime );
	}

	// 装备特效
	if( m_dwParticleEffectSize > 0 )
	{
		cAnimation* pAnimation = m_pEquipModel->GetAnimRoot();
		cSkeletonHierarchy* pModelHierarchy = m_pEquipModel->GetSkeletonHierarchyRoot();
		cParticleEffectBase* pParticleEffect;

		if( pAnimation && pModelHierarchy )
		{
			for( unsigned i = 0; i < m_dwParticleEffectSize; i++ )
			{
				pParticleEffect = m_ppParticleEffects[i];
				pParticleEffect->SetCurAnimTime( m_fCurrentAnimTime );

				cSkeletonHierarchy* pSkeletonHierarchy = pModelHierarchy->findSkeletonHierarchy( pParticleEffect->GetLinkName() );
				if( pSkeletonHierarchy )
				{
					D3DXMATRIXA16* pMatrix = pSkeletonHierarchy->getSkeletonHierarchyMatrix();
					D3DXMATRIXA16 matAnimTransform;
					D3DXMatrixMultiply( &matAnimTransform, pMatrix, &m_matEqMatrix );
					pParticleEffect->ApplyTransformMatrix( &matAnimTransform );
				}
				else
				{
					pParticleEffect->ApplyTransformMatrix( &m_matEqMatrix );
				}
				pParticleEffect->FrameMove( m_fAnimElapsedTime );
			}
		}
		else
		{
			for( unsigned i = 0; i < m_dwParticleEffectSize; i++ )
			{
				pParticleEffect = m_ppParticleEffects[i];
				pParticleEffect->SetCurAnimTime( m_fCurrentAnimTime );
				pParticleEffect->ApplyTransformMatrix( &m_matEqMatrix );
				pParticleEffect->FrameMove( m_fAnimElapsedTime );
			}
		}
	}

	if( m_pModelEffectList )
	{
		cModelEffect* pModelEffect;
		int iCount = m_pModelEffectList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pModelEffect = (cModelEffect*)m_pModelEffectList->GetItem( i );
			if( pModelEffect )
			{
				pModelEffect->ApplyTransformMatrix( m_fAnimElapsedTime, &m_matEqMatrix );
			}
		}
	}

	//   修改装备Model Render架构
	if( m_pRenderArray )
	{
		cRender* pRender;
		cSkeletonHierarchy* pModelHierarchy = m_pEquipModel->GetSkeletonHierarchyRoot();
		cAnimation* pAnimation = m_pEquipModel->GetAnimRoot();
		if( pModelHierarchy && pAnimation )
		{
			cSkeletonHierarchy* pSkeletonHierarchy;
			cSubMeshSerializer* pSubMeshSerializer;
			for( int i = 0; i < m_iRenderSize; i++ )
			{
				pRender = m_pRenderArray[i].m_pRender;
				if( pRender == NULL )
				{
					continue;
				}

				pSubMeshSerializer = m_pEquipModel->getSubMeshSerializer( i );
				if( pSubMeshSerializer == NULL )
				{
					continue;
				}
				pSkeletonHierarchy = pModelHierarchy->findSkeletonHierarchy( 
					pSubMeshSerializer->getSubMeshName() );
				if( pSkeletonHierarchy )
				{
					pRender->SetWorldMatrix( ( *pSkeletonHierarchy->getSkeletonHierarchyMatrix() ) * m_matEqMatrix );
				}
			}
		}
		else
		{
			for( int i = 0; i < m_iRenderSize; i++ )
			{
				pRender = m_pRenderArray[i].m_pRender;
				if( pRender == NULL )
				{
					continue;
				}

				pRender->SetWorldMatrix( m_matEqMatrix );
			}
		}
	}
}

//---------------------------------------------------------------------------
// Name: AddEquipModelEffect
// Desc: 新增一个装备专用模型特效
//---------------------------------------------------------------------------
//   修改装备Model Render架构
void cSkinnedCharacterEquipPart::AddEquipModelEffect( char* pName, BYTE 
													 byStatusType )
{
	if( m_pEquipModel == NULL )
	{
		return;
	}

	if( byStatusType > 0 )
	{
		if( m_pStatusEffect )
		{
			if( m_pStatusEffect->m_byType == byStatusType )
			{
				return;
			}
			else
			{
				RemoveStatusModelEffect();
			}
		}
	}

	if( m_pModelEffectList )
	{
		cModelEffect* pModelEffect = _NEW cModelEffect;
		if( pModelEffect )
		{
			pModelEffect->LoadMesh( pName );
			pModelEffect->ApplyBoundingBox( m_pEquipModel->GetEffectBoundingBoxMin(), 
				m_pEquipModel->GetEffectBoundingBoxMax() );
			m_pModelEffectList->Add( pModelEffect );

			if( byStatusType > 0 )
			{
				if( m_pStatusEffect == NULL )
				{
					m_pStatusEffect = _NEW sStatusEffect();
				}
				if( m_pStatusEffect )
				{
					m_pStatusEffect->m_byType = byStatusType;
					m_pStatusEffect->m_pModelEffect = pModelEffect;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
// Name: RemoveStatusModelEffect
// Desc: 移除状态装备特效
//---------------------------------------------------------------------------
// 状态装备特效
void cSkinnedCharacterEquipPart::RemoveStatusModelEffect( void )
{
	if( m_pStatusEffect )
	{
		if( m_pModelEffectList )
		{
			cModelEffect* pModelEffect;
			int iCount = m_pModelEffectList->GetCount();
			for( int i = 0; i < iCount; i++ )
			{
				pModelEffect = (cModelEffect*)m_pModelEffectList->GetItem( i );
				if( pModelEffect == m_pStatusEffect->m_pModelEffect )
				{
					delete pModelEffect;
					m_pModelEffectList->Exchange( i, iCount - 1 );
					m_pModelEffectList->Delete( iCount - 1 );
					break;
				}
			}
		}

		delete m_pStatusEffect;
		m_pStatusEffect = NULL;
	}
}

//---------------------------------------------------------------------------
// Name: SetEquipModel
// Desc: 设定模型Model
//---------------------------------------------------------------------------
//   修改装备Model Render架构
void cSkinnedCharacterEquipPart::SetEquipModel( cEquipModel* pEquipModel, int 
											   iTextureID, bool bWeaponRibbonLerp, cBeing* pOwnerBeing )
{
	if( g_pEquipModelDB == NULL )
	{
		return;
	}

	cRenderBuffer* pRenderBuffer = g_pEquipModelDB->GetRenderBuffer();
	cTextureDB* pTextureDB = g_pEquipModelDB->GetTextureDB();
	if( pRenderBuffer == NULL || pTextureDB == NULL )
	{
		return;
	}

	if( m_pRenderArray )
	{
		cEquipRender* pEquipRender;
		for( int i = 0; i < m_iRenderSize; i++ )
		{
			pEquipRender = (cEquipRender*)m_pRenderArray[i].m_pRender;
			if( pEquipRender == NULL )
			{
				continue;
			}
			pEquipRender->SetSubMeshSerializer( NULL );
		}
	}

	m_iRenderSize = 0;
	SAFE_DELETE_ARRAY( m_pRenderArray );
	SAFE_RELEASE( m_pEquipModel );

	// 装备特效
	if( g_p3dsMaxParticleManager )
	{
		// 释放刀光
		if( m_pWeaponRibbon )
		{
			g_p3dsMaxParticleManager->ReleaseParticleEffect( m_pWeaponRibbon );
			m_pWeaponRibbon = NULL;
		}
		m_bShowWeaponRibbon = false;

		// 释放其他粒子特效
		for( unsigned int i = 0; i < m_dwParticleEffectSize; i++ )
		{
			g_p3dsMaxParticleManager->ReleaseParticleEffect( m_ppParticleEffects[i] 
			);
		}
		SAFE_DELETE_ARRAY( m_ppParticleEffects );
		m_dwParticleEffectSize = 0;
	}

	SAFE_DELETE( m_pStatusEffect );

	if( m_pModelEffectList )
	{
		cModelEffect* pModelEffect;
		int iCount = m_pModelEffectList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pModelEffect = (cModelEffect*)m_pModelEffectList->GetItem( i );
			SAFE_DELETE( pModelEffect );
		}
		m_pModelEffectList->Clear();
	}

	if( pEquipModel == NULL )
	{
		m_bModelExist = false;
		m_byWaitResourceCount = 0;
		return;
	}

	// 070522 cyhsieh
	char szFileName[MAX_PATH];
	EZString szPath = g_sAppPath + "Mesh\\Equipment\\";
	char szTexturePath[]="Mesh\\Equipment\\";
	char szSubPathFakeReflectTex[]="Mesh\\Being\\Tex\\";

	m_iTextureID = iTextureID;

	cSubMeshSerializer* pSubMeshSerializer;
	m_pEquipModel = pEquipModel;
	m_iRenderSize = m_pEquipModel->getSubMeshSerializersCount();

	if( m_iRenderSize > 0 )
	{
		m_pRenderArray = _NEW stRENDER_ARRAY[m_iRenderSize];
		if( m_pRenderArray == NULL )
		{
			m_bModelExist = false;
			m_iRenderSize = 0;
			SAFE_RELEASE( m_pEquipModel );
			m_byWaitResourceCount = 0;
			return;
		}

		// 装备特效
		int iMeshParticleEffectCount = 0;
		for( int i = 0; i < m_iRenderSize; i++ )
		{
			pSubMeshSerializer = m_pEquipModel->getSubMeshSerializer( i );
			if( pSubMeshSerializer && pSubMeshSerializer->GetMeshType() > 0 )
			{
				iMeshParticleEffectCount++;
			}
		}
		if( g_p3dsMaxParticleManager && m_ppParticleEffects == NULL )
		{
			DWORD dwParticleEffectSize = m_pEquipModel->Get3dsMaxEmitDataSize();
			if( dwParticleEffectSize + iMeshParticleEffectCount > 0 )
			{
				//EZString szPath = g_sAppPath + "Mesh\\Equipment\\";
				m_dwParticleEffectSize = dwParticleEffectSize + 
					iMeshParticleEffectCount;
				m_ppParticleEffects = _NEW cParticleEffectBase*
					[m_dwParticleEffectSize];
				if( m_ppParticleEffects )
				{
					unsigned int iIndex;
					for( iIndex = 0; iIndex < dwParticleEffectSize; iIndex++ )
					{
						c3dsMaxEmitDataBase* pEmitData = m_pEquipModel->Get3dsMaxEmitData( 
							iIndex );
						m_ppParticleEffects[iIndex] = g_p3dsMaxParticleManager->CreateParticleEffectByEmitData( pEmitData, szPath.c_str() );
					}

					for( int i = 0; i < m_iRenderSize; i++ )
					{
						pSubMeshSerializer = m_pEquipModel->getSubMeshSerializer( i );
						if( pSubMeshSerializer && pSubMeshSerializer->GetMeshType() > 0 )
						{
							m_ppParticleEffects[iIndex] = g_p3dsMaxParticleManager->CreateParticleEffectBySubMeshSerializer( pSubMeshSerializer, szPath.c_str() );
							iIndex++;
						}
					}
				}
			}
		}

		m_bModelExist = true;

		for( int i = 0; i < m_iRenderSize; i++ )
		{
			pSubMeshSerializer = m_pEquipModel->getSubMeshSerializer( i );
			if( pSubMeshSerializer == NULL )
			{
				continue;
			}

			char* pSubMeshName = pSubMeshSerializer->getSubMeshName();

			if( strstr( pSubMeshName, "m_shadow" ) != NULL ) continue;
			if( strstr( pSubMeshName, "e_flame" ) != NULL ) continue;
			if( strstr( pSubMeshName, "b_00" ) != NULL ) continue;
			if( pSubMeshSerializer->GetMeshType() > 0 ) continue;

			cEquipRender* pEquipRender = (cEquipRender*)g_mScene->RenderCreate( RENDER_C_EQUIP );
			if( pEquipRender == NULL )
			{
				continue;
			}

			pEquipRender->SetSubMeshSerializer( pSubMeshSerializer );
			pEquipRender->State = RENDER_STATE_DATA_HANDLE;

			m_pRenderArray[i].m_pRender = (cEquipRender*)pEquipRender->Create();

			// 装备特效
			DWORD dwRenderAlphaBlendType = 0;
			const D3DXCOLOR* pColor = NULL;

			int iMaterialSize = pSubMeshSerializer->getSubMeshMaterialSize();
			pEquipRender->iMaterialSize = iMaterialSize;

			if( iMaterialSize > 0 )
			{
				pEquipRender->m_pMaterial = _NEW cMaterial
					[iMaterialSize];

				bool bHasFakeReflect = false;
				cMaterial* pFakeReflectMat = NULL;

				// 反光贴图开关
				if( g_Options.GetShowFakeReflect() )
				{
					pFakeReflectMat = _NEW cMaterial[iMaterialSize];
				}

				if( pEquipRender->m_pMaterial == NULL )
				{
					continue;
				}
				for( int j = 0; j < iMaterialSize; j++ )
				{
					cMaterial* pMaterial = pSubMeshSerializer->getSubMeshMaterial( j );
					if( pMaterial == NULL )
					{
						continue;
					}

					pEquipRender->m_pMaterial[j].m_D3DMaterial = pMaterial->m_D3DMaterial;

					// 装备特效
					pEquipRender->m_pMaterial[j].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
					pEquipRender->m_pMaterial[j].SetTextureAniRows( pMaterial->GetTextureAniRows() );
					pEquipRender->m_pMaterial[j].SetTextureAniCols( pMaterial->GetTextureAniCols() );
					pEquipRender->m_pMaterial[j].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );
					if( j == 0 )
					{
						dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
					}

					if( pMaterial->m_iNameSize && pMaterial->m_pszTextName )
					{
						sprintf( szFileName, "%s%02d", pMaterial->m_pszTextName, iTextureID );
						pEquipRender->m_pMaterial[j].m_pTexture = pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));

						cTexture* pTexture = pEquipRender->m_pMaterial[j].m_pTexture;

						// Thread
						if( pTexture )
						{
							if( pTexture->GetTexture( NULL ) == NULL )
							{
								if( pOwnerBeing )
								{
									BYTE byType = OWNER_BEING;
									int iID = 0;
									pOwnerBeing->GetResourceOwnerData( byType, iID );
									pTexture->InsertOwnerBeing( byType, iID, m_byPartID + 1 );
									m_byWaitResourceCount++;
								}
							}
						}

						// 反光贴图
						// 反光贴图开关
						if( pFakeReflectMat && pTexture && pTexture->GetLoadFakeReflectTex() )
						{
							sprintf( szFileName, "%s%02d_s", pMaterial->m_pszTextName, iTextureID );
							pFakeReflectMat[j].m_pTexture = pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
							if( pFakeReflectMat[j].m_pTexture )
							{
								bHasFakeReflect = true;
							}
							else
							{
								// 读不到反光贴图，以后不要再尝试load
								pTexture->SetLoadFakeReflectTex( false );
							}
						}
					}

					// 装备特效
					cMaterialAnimator* pMaterialAnimator = pSubMeshSerializer->GetMaterialAnimator( j );
					if( pMaterialAnimator )
					{
						pEquipRender->m_pMaterial[j].SetMaterialAnimator( pMaterialAnimator );

						if( pEquipRender->m_pMaterial[j].GetTextureTransformMatrix() )
						{
							pEquipRender->m_pMaterial[j].GetAnimatorAddress( pEquipRender->m_dwAddressU, pEquipRender->m_dwAddressV );
						}

						if( j == 0 )
						{
							pColor = pEquipRender->m_pMaterial[j].GetDiffuseOpacity();
						}
					}
				}

				if( bHasFakeReflect )
				{
					pEquipRender->SetFakeReflectMaterial( pFakeReflectMat );

					if( g_pTextureDB )
					{
						cTexture* pFakeReflectTexture = pTextureDB->QueryTexture("FakeReflect",
							szSubPathFakeReflectTex,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
						pEquipRender->SetFakeReflectTexture( pFakeReflectTexture );
					}
				}
				else
				{
					SAFE_DELETE_ARRAY( pFakeReflectMat );
				}

				// 装备特效
				if( pColor )
				{
					pEquipRender->m_dwAlphaArg1 = D3DTA_TEXTURE;
					pEquipRender->m_dwAlphaArg2 = D3DTA_TFACTOR;
					pEquipRender->m_dwAlphaOp   = D3DTOP_MODULATE;
					pEquipRender->m_dwColorArg1 = D3DTA_TEXTURE;
					pEquipRender->m_dwColorArg2 = D3DTA_TFACTOR;
					pEquipRender->m_dwColorOp   = D3DTOP_MODULATE;
				}
				switch( dwRenderAlphaBlendType )
				{
				case 0:   // Opaque
					break;
				case 1:   // Additive
					pEquipRender->m_bAlphaBlendenable = true;
					pEquipRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
					pEquipRender->m_dwDestBlend = D3DBLEND_ONE;
					pEquipRender->m_bZWriteenable = false;
					pEquipRender->m_dwAlphaRef = 0x01;
					pEquipRender->ForceAlpha( 0.0f );
					pEquipRender->bLightSystem = false;
					break;
				case 2:   // Alpha
					pEquipRender->m_bAlphaBlendenable = true;
					pEquipRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
					pEquipRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
					pEquipRender->m_bZWriteenable = false;
					pEquipRender->m_dwAlphaRef = 0x01;
					pEquipRender->ForceAlpha( 0.0f );
					pEquipRender->bLightSystem = false;
					break;
				case 3:   // Modulate
					pEquipRender->m_bAlphaBlendenable = true;
					pEquipRender->m_dwSrcBlend = D3DBLEND_ZERO;
					pEquipRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
					pEquipRender->m_bZWriteenable = false;
					pEquipRender->m_dwAlphaRef = 0x01;
					pEquipRender->ForceAlpha( 0.0f );
					pEquipRender->bLightSystem = false;
					break;
				case 4:   // Modulate Inv
					pEquipRender->m_bAlphaBlendenable = true;
					pEquipRender->m_dwSrcBlend = D3DBLEND_ZERO;
					pEquipRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
					pEquipRender->m_bZWriteenable = false;
					pEquipRender->m_dwAlphaRef = 0x01;
					pEquipRender->ForceAlpha( 0.0f );
					pEquipRender->bLightSystem = false;
					break;
				}

			}

			pEquipRender->m_pRenderBuffer = pRenderBuffer;
			pEquipRender->m_pAttribTable = pSubMeshSerializer->getSubMeshAttributeTable();
			pEquipRender->iAttribTableSize = pSubMeshSerializer->getSubMeshAttributeTableSize();

			pEquipRender->State = RENDER_STATE_DATA_OK;
		}

		// 刀光
		if( m_pEquipModel->GetShadowMinPos() != m_pEquipModel->GetShadowMaxPos() )
		{
			m_pWeaponRibbon = g_p3dsMaxParticleManager->CreateParticleEffectWeaponRibbon( bWeaponRibbonLerp,
								  m_pEquipModel->GetShadowMinPos(),
								  m_pEquipModel->GetShadowMaxPos(),
								  szTexturePath );
		}

	}
}

//---------------------------------------------------------------------------
// Name: AddToScene
// Desc: 加到Scene
//---------------------------------------------------------------------------
// 反光整理
void cSkinnedCharacterEquipPart::AddToScene( bool bUseFocusLight )
{
	if( m_pRenderArray == NULL )
	{
		return;
	}

	for( int i = 0; i < m_iRenderSize; i++ )
	{
		cRender* pRender = m_pRenderArray[i].m_pRender;
		if( pRender == NULL )
		{
			continue;
		}

		// 反光整理
		pRender->m_bUseFocusLight = bUseFocusLight;

		// !!!To do... For test, should be set in render's framemove
		const D3DXCOLOR* pColor;
		if( pRender->iMaterialSize > 0 && ( pColor = pRender->m_pMaterial
			[0].GetDiffuseOpacity() ) )
		{
			pRender->m_dwTextureFactor = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, 
				pColor->b, pColor->a );
		}

		switch( pRender->GetMaterialAlphaBlendType() )
		{
		case 0:
			g_mScene->AddRender( pRender ); // Opaque
			break;
		default:
			g_mScene->AddRenderWithType( pRender, cScene::E_RENDERTYPE_MODELEFFECT );
			break;
		}
	}
}

//---------------------------------------------------------------------------
// Name: SetFade
// Desc: 设定淡出淡入
//---------------------------------------------------------------------------
//   修改装备Model Render架构
void cSkinnedCharacterEquipPart::SetFade( float fValue )
{
	if( m_pRenderArray == NULL )
	{
		return;
	}

	for( int i = 0; i < m_iRenderSize; i++ )
	{
		if( m_pRenderArray[i].m_pRender )
		{
			m_pRenderArray[i].m_pRender->SetFadeTypeEasy( fValue );
		}
	}
}

//---------------------------------------------------------------------------
// Name: SetShadowData
// Desc: 设定阴影资料
//---------------------------------------------------------------------------
//   修改装备Model Render架构
void cSkinnedCharacterEquipPart::SetShadowData( sShadowData* pShadowData )
{
	if( m_pRenderArray == NULL )
	{
		return;
	}

	for( int i = 0; i < m_iRenderSize; i++ )
	{
		cEquipRender* pRender = (cEquipRender*)m_pRenderArray[i].m_pRender;
		if( pRender && ( pRender->iMaterialSize > 0 ) )
		{
			// 实体物件才投阴影
			if( pRender->m_pMaterial[0].GetAlphaBlendType() == 0 )
			{
				pRender->SetShadowData( pShadowData );
			}
		}
	}
}

//---------------------------------------------------------------------------
// Name: TickAnimTime
// Desc: 更新Animation时间
//---------------------------------------------------------------------------
// 装备特效
void cSkinnedCharacterEquipPart::TickAnimTime( float fElapsedTime )
{
	m_fAnimElapsedTime = fElapsedTime * 4800.0f;
	m_fCurrentAnimTime += m_fAnimElapsedTime;
}

//---------------------------------------------------------------------------
// Name: ComputeAnimation
// Desc: 计算Animation
//---------------------------------------------------------------------------
// 装备特效
void cSkinnedCharacterEquipPart::ComputeAnimation( void )
{
	if( m_pEquipModel && m_pEquipModel->GetSkeletonHierarchyRoot() )
	{
		cAnimation* pAnimation = m_pEquipModel->GetAnimRoot();
		cAnimationActionInfoGroup* pActionInfoGroup = m_pEquipModel-	>GetActionInfoGroup();

		if( pAnimation && pActionInfoGroup && ( pActionInfoGroup->getAnimationActionInfoCount() > 0 ) )
		{
			float fAnimTime = 0.0f;
			cAnimationActionInfo* pActionInfo = pActionInfoGroup->getAnimationActionInfo( m_iActionIndex );
			if( pActionInfo )
			{
				float fStartTime = (float)pActionInfo->getAnimationActionStartTime();
				float fEndTime = (float)pActionInfo->getAnimationActionEndTime();
				if( m_fCurrentAnimTime > fEndTime )
				{
					fAnimTime = fmod( ( m_fCurrentAnimTime - fEndTime ), ( fEndTime - fStartTime ) );
					m_fCurrentAnimTime = fStartTime + fAnimTime;
				}
				else
				{
					fAnimTime = m_fCurrentAnimTime - fStartTime;
				}
			}

			g_pStack->Push();
			g_pStack->LoadIdentity();

			sComputeAnimParam param( m_iActionIndex, -1, fAnimTime, 0.0f, 0.0f );
			m_pEquipModel->GetSkeletonHierarchyRoot()->computeSkeletonHierarchyMatrix( pActionInfoGroup, param );

			g_pStack->Pop();
		}
	}
}

//---------------------------------------------------------------------------
// Name: ForceDraw
// Desc: 强制画(给照片用)
//---------------------------------------------------------------------------
// 装备介面人物照
void cSkinnedCharacterEquipPart::ForceDraw( LPDIRECT3DDEVICE9 pD3DDDevice, 
										   D3DXMATRIXA16* pTransform )
{
	if( ( m_pRenderArray == NULL ) || ( m_pEquipModel == NULL ) )
	{
		return;
	}

	if( pTransform )  // 设定世界矩阵
	{
		if( m_pSkeletonHierarchy )
		{
			m_matEqMatrix = ( *m_pSkeletonHierarchy->getSkeletonHierarchyMatrix() ) * 
				( *pTransform );
		}
		else
		{
			m_matEqMatrix = ( *pTransform );
		}

		cRender* pRender;
		cSkeletonHierarchy* pModelHierarchy = m_pEquipModel->GetSkeletonHierarchyRoot();
		cAnimation* pAnimation = m_pEquipModel->GetAnimRoot();
		if( pModelHierarchy && pAnimation )
		{
			cSkeletonHierarchy* pSkeletonHierarchy;
			cSubMeshSerializer* pSubMeshSerializer;
			for( int i = 0; i < m_iRenderSize; i++ )
			{
				pRender = m_pRenderArray[i].m_pRender;
				if( pRender == NULL )
				{
					continue;
				}

				pSubMeshSerializer = m_pEquipModel->getSubMeshSerializer( i );
				if( pSubMeshSerializer == NULL )
				{
					continue;
				}
				pSkeletonHierarchy = pModelHierarchy->findSkeletonHierarchy( pSubMeshSerializer->getSubMeshName() );
				if( pSkeletonHierarchy )
				{
					pRender->SetWorldMatrix( ( *pSkeletonHierarchy->getSkeletonHierarchyMatrix() ) * m_matEqMatrix );
				}
			}
		}
		else
		{
			for( int i = 0; i < m_iRenderSize; i++ )
			{
				pRender = m_pRenderArray[i].m_pRender;
				if( pRender == NULL )
				{
					continue;
				}

				pRender->SetWorldMatrix( m_matEqMatrix );
			}
		}
	}

	for( int i = 0; i < m_iRenderSize; i++ )
	{
		cEquipRender* pRender = (cEquipRender*)m_pRenderArray[i].m_pRender;
		if( pRender == NULL )
		{
			continue;
		}

		// 记录一些会影响Render的资讯
		bool bOriUseFocusLight = pRender->m_bUseFocusLight;
		bool bOriLightSystem = pRender->bLightSystem;
		float fOriAlpha = pRender->GetAlpha();

		// 设成一般的Render
		pRender->m_bUseFocusLight = false;    // 不发光
		pRender->SetShadowData( NULL );       // 不画阴影
		pRender->bLightSystem = false;        // 不打光
		pRender->ForceAlpha( 0.0f );          // 不要半透明

		// 画
		pRender->Draw( pD3DDDevice );

		// 设回来原本的
		pRender->m_bUseFocusLight = bOriUseFocusLight;
		pRender->bLightSystem = bOriLightSystem;
		pRender->ForceAlpha( fOriAlpha );
	}

	// 特效
	if( ( pTransform == NULL ) && g_p3dsMaxParticleManager )  // 画粒子特效
	{
		if( m_dwParticleEffectSize > 0 )
		{
			for( unsigned i = 0; i < m_dwParticleEffectSize; i++ )
			{
				g_p3dsMaxParticleManager->AddForceDrawParticleEffect( m_ppParticleEffects[i] );
			}
		}

		if( m_pModelEffectList )
		{
			cModelEffect* pModelEffect;
			int iCount = m_pModelEffectList->GetCount();
			for( int i = 0; i < iCount; i++ )
			{
				pModelEffect = (cModelEffect*)m_pModelEffectList->GetItem( i );
				if( pModelEffect )
				{
					pModelEffect->ForceDrawParticleEffects();
				}
			}
		}

		g_p3dsMaxParticleManager->ForceDraw( pD3DDDevice );
	}
}

//----------------------------------------------------------------------------
// Name: ForceAlpha
// Desc: 强制设定Render的Alpha值
//----------------------------------------------------------------------------
void cSkinnedCharacterEquipPart::ForceAlpha( float fAlpha )
{
	if( m_pRenderArray == NULL )
	{
		return;
	}

	for( int i = 0; i < m_iRenderSize; i++ )
	{
		if( m_pRenderArray[i].m_pRender )
		{
			m_pRenderArray[i].m_pRender->ForceAlpha( fAlpha );
		}
	}
}

//---------------------------------------------------------------------------
// Name: SetFakeReflect
// Desc: 设定是否开启假反光
//---------------------------------------------------------------------------
// 反光贴图开关
void cSkinnedCharacterEquipPart::SetFakeReflect( bool bShow )
{
	if( m_pEquipModel == NULL || m_iRenderSize == 0 || m_pRenderArray == NULL )
	{
		return;
	}

	cTextureDB* pTextureDB = g_pEquipModelDB->GetTextureDB();
	if( pTextureDB == NULL || g_pTextureDB == NULL )
	{
		return;
	}

	char szFileName[MAX_PATH];
	EZString szPath = g_sAppPath + "Mesh\\Equipment\\";
	char szTexturePath[]="Mesh\\Equipment\\";
	char szSubPathFakeReflectTex[]="Mesh\\Being\\Tex\\";
	cSubMeshSerializer* pSubMeshSerializer;
	cEquipRender* pEquipRender;

	for( int i = 0; i < m_iRenderSize; i++ )
	{
		pSubMeshSerializer = m_pEquipModel->getSubMeshSerializer( i );
		if( pSubMeshSerializer == NULL )
		{
			continue;
		}

		pEquipRender = (cEquipRender*)m_pRenderArray[i].m_pRender;
		if( pEquipRender == NULL )
		{
			continue;
		}

		if( bShow )
		{
			if( pEquipRender->GetFakeReflectMaterial() != NULL )
			{
				continue;
			}

			bool bHasFakeReflect = false;
			cMaterial* pFakeReflectMat = _NEW cMaterial[pEquipRender->iMaterialSize];
			for( int j = 0; j < pEquipRender->iMaterialSize; j++ )
			{
				cMaterial* pMaterial = pSubMeshSerializer->getSubMeshMaterial( j );
				if( ( pMaterial == NULL ) || ( pMaterial->m_iNameSize == 0 ) )
				{
					continue;
				}
				cTexture* pTexture = pEquipRender->m_pMaterial[j].m_pTexture;
				if( pFakeReflectMat && pTexture && pTexture->GetLoadFakeReflectTex() )
				{
					sprintf( szFileName, "%s%02d_s", pMaterial->m_pszTextName, 
						m_iTextureID );
					pFakeReflectMat[j].m_pTexture = pTextureDB->QueryTexture
						(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
					if( pFakeReflectMat[j].m_pTexture )
					{
						bHasFakeReflect = true;
					}
					else
					{
						// 读不到反光贴图，以后不要再尝试load
						pTexture->SetLoadFakeReflectTex( false );
					}
				}
			}
			if( bHasFakeReflect )
			{
				pEquipRender->SetFakeReflectMaterial( pFakeReflectMat );

				cTexture* pFakeReflectTexture = g_pTextureDB->QueryTexture
					("FakeReflect",szSubPathFakeReflectTex,MESH_STUFF_PATH,sTextureQueryParam( 
					false, 0 )); 
				pEquipRender->SetFakeReflectTexture( pFakeReflectTexture );
			}
			else
			{
				SAFE_DELETE_ARRAY( pFakeReflectMat );
			}
		}
		else
		{
			// 释放假反光相关资源
			pEquipRender->ReleaseFakeReflectResource();
		}
	}
}

//---------------------------------------------------------------------------
// Name: SetFocusLight
// Desc: 将Render设成focus
//---------------------------------------------------------------------------
// 反光整理
void cSkinnedCharacterEquipPart::SetFocusLight( void )
{
	if( m_pRenderArray == NULL )
	{
		return;
	}

	for( int i = 0; i < m_iRenderSize; i++ )
	{
		cRender* pRender = m_pRenderArray[i].m_pRender;
		if( pRender )
		{
			pRender->m_bUseFocusLight = true;
		}
	}
}

//---------------------------------------------------------------------------
// Name: SetActionIndex
// Desc: 设定动作
//---------------------------------------------------------------------------
//  钓竿
void cSkinnedCharacterEquipPart::SetActionIndex( int iActionIndex )
{
	if( m_pEquipModel && m_pEquipModel->GetSkeletonHierarchyRoot() )
	{
		cAnimation* pAnimation = m_pEquipModel->GetAnimRoot();
		cAnimationActionInfoGroup* pActionInfoGroup = m_pEquipModel->GetActionInfoGroup();

		if( pAnimation && pActionInfoGroup )
		{
			int iCount = pActionInfoGroup->getAnimationActionInfoCount();
			if( iActionIndex >= iCount )
			{
				return;
			}

			m_iActionIndex = iActionIndex;
			m_fCurrentAnimTime = 0;

			cAnimationActionInfo* pActionInfo = pActionInfoGroup->getAnimationActionInfo( m_iActionIndex );
			if( pActionInfo )
			{
				m_fCurrentAnimTime = (float)pActionInfo->getAnimationActionStartTime();
			}
		}
	}
}

//---------------------------------------------------------------------------
// Name: GetEquipModelEffectCount
// Desc: 取得装备特效数量
//---------------------------------------------------------------------------
// 特效
int cSkinnedCharacterEquipPart::GetEquipModelEffectCount()
{
	if( m_pModelEffectList )
	{
		return m_pModelEffectList->GetCount();
	}
	return 0;
}

//---------------------------------------------------------------------------
// Name: GetEquipModelEffectName
// Desc: 取得装备特效名称
//---------------------------------------------------------------------------
// 特效
char* cSkinnedCharacterEquipPart::GetEquipModelEffectName( int iIndex )
{
	if( m_pModelEffectList )
	{
		cModelEffect* pModelEffect = (cModelEffect*)m_pModelEffectList->GetItem( iIndex );
		if( pModelEffect )
		{
			return pModelEffect->GetMeshModelName();
		}
	}
	return NULL;
}

}