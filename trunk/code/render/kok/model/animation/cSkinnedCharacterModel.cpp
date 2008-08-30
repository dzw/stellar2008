#include "stdafx.h"
/*----------------------------------------------------------------------*/
// 檔名: cSkinnedCharacterModel.cpp
// 說明:
/*----------------------------------------------------------------------*/
#include "cSkinnedCharacterModel.h"
#include "..\Share\cTools.h"
#include "..\Share\Camera.h"

// 070205 cyhsieh model effect
#include "../Effect/cModelEffect.h"

#include "../Thread/ThreadingBuffer.h"

extern EZString g_sAppPath;
extern cCamera  g_Camera;
extern cScene*  g_mScene;          // 場景繪製管理

// 070816 cyhsieh Skinned Character LOD
float cSkinnedCharacterModel::m_fLodDistanceSq[MAX_BEINGLOD_LV] = { 0.0f, 60.0f * 60.0f, 90.0f * 90.0f };

//-----------------------------------------------------------------------------
// Name: SkinnedCharacterPart()
// Desc: SkinnedCharacterPart 建構元
//-----------------------------------------------------------------------------
cSkinnedCharacterModel::SkinnedCharacterPart::SkinnedCharacterPart()
{
	// 070816 cyhsieh Skinned Character LOD
	for( int i = 0; i < MAX_BEINGLOD_LV; i++ )
	{
		m_pMesh[i] = NULL;
	}

	m_iTextureId = 0;
}

//-----------------------------------------------------------------------------
// Name: ~SkinnedCharacterPart()
// Desc: SkinnedCharacterPart 解構元
//-----------------------------------------------------------------------------
cSkinnedCharacterModel::SkinnedCharacterPart::~SkinnedCharacterPart()
{
	clsSkinnedCharacterPart();
}

//-----------------------------------------------------------------------------
// Name: ClsSkinnedCharacterPart()
// Desc: 
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::SkinnedCharacterPart::clsSkinnedCharacterPart()
{
	// 070816 cyhsieh Skinned Character LOD
	for( int i = 0; i < MAX_BEINGLOD_LV; i++ )
	{
		m_pMesh[i] = NULL;
	}

	m_iTextureId = 0;
}

///////////////////////////////////////////////////////////////////////////////
//  生物模型的基本資料
///////////////////////////////////////////////////////////////////////////////
//jingjie test 2006.07.10
unsigned int cSkinnedCharacterModel::m_uiSkinnedCharacterModelCount = 0;
//-----------------------------------------------------------------------------
// Name: cSkinnedCharacterModel()
// Desc: cSkinnedCharacterModel 建構元
//-----------------------------------------------------------------------------
cSkinnedCharacterModel::cSkinnedCharacterModel()
{
	m_CurnAction.clear();    // 初始化 目前的動作
	m_FadeoutAction.clear(); // 初始化 正在淡出的動作 

	m_FadeoutAction.m_iIndex = -1;
	m_fFadeoutTimeRemain = 0.0f;
	m_fFadeoutTime       = 0.0f;

	m_pSkeletonSerializer = NULL; // 由外部傳入的 cFrameWork
	m_pPart              = NULL;
	m_pTextureDB         = NULL;
	m_pRenderBuffer      = NULL;

	m_pWorldSpaceBoundingBoxMin = _NEW D3DXVECTOR3;
	m_pWorldSpaceBoundingBoxMax = _NEW D3DXVECTOR3;
	m_pLocalSpaceBoundingBoxMin = _NEW D3DXVECTOR3(-50.0f,-50.0f,-50.0f);
	m_pLocalSpaceBoundingBoxMax = _NEW D3DXVECTOR3(50.0f,12.5f,50.0f);

	m_szNoneCombinedSkinnedCharacterWeaponName = NULL;

	m_pEquipPartList = NULL;

	// 071030 cyhsieh 動作武器顯示
	m_bHideWeaponEquip = false;

	m_bNoneCombinedSkinnedCharacterAdorn01 = false;
	m_bNoneCombinedSkinnedCharacterAdorn02 = false;
	m_bNoneCombinedSkinnedCharacterAdorn03 = false;
	m_bUpadteWorldMatrix = false;

	m_iDirection         = -900;

	m_pRender            = NULL;

	D3DXMatrixIdentity(&m_matTranslation);
	D3DXMatrixIdentity(&m_matRotation);
	D3DXMatrixIdentity(&m_matScaling);
	D3DXMatrixIdentity(&m_matMatrix);

	setSkinnedCharacterRenderEntity( rsFadeIn );

	m_iRenderStyle = rsDefault;      // 繪製方式(不畫)

	// 060420 cyhsieh 實體影子
	m_pShadowData = NULL;

	m_byWaitResourceCount = 0;

	//jingjie test 2006.07.10
	m_uiSkinnedCharacterModelCount++;
}

//-----------------------------------------------------------------------------
// Name: ~cSkinnedCharacterModel()
// Desc: cSkinnedCharacterModel 解構元
//-----------------------------------------------------------------------------
cSkinnedCharacterModel::~cSkinnedCharacterModel()
{
	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			SAFE_DELETE( pEquipPart );
		}
		delete m_pEquipPartList;
	}

	m_pSkeletonSerializer = NULL;
	m_pTextureDB    = NULL;
	m_pRenderBuffer = NULL;

	SAFE_RELEASE(m_pRender);// 讓場景管理去清除

	SAFE_DELETE_ARRAY(m_pPart);

	SAFE_DELETE_ARRAY(m_szNoneCombinedSkinnedCharacterWeaponName);

	SAFE_DELETE( m_pWorldSpaceBoundingBoxMin );
	SAFE_DELETE( m_pWorldSpaceBoundingBoxMax );
	SAFE_DELETE( m_pLocalSpaceBoundingBoxMin );
	SAFE_DELETE( m_pLocalSpaceBoundingBoxMax );

	// 060420 cyhsieh 實體影子
	SAFE_DELETE( m_pShadowData );

	m_uiSkinnedCharacterModelCount--;
}

//-----------------------------------------------------------------------------
// Name: SetPartMeshSerializer
// Desc:
//-----------------------------------------------------------------------------
// 070816 cyhsieh Skinned Character LOD
void cSkinnedCharacterModel::SetPartMeshSerializer( int iPart, int iLodLevel, cMeshSerializer* pMeshSerializer )
{
	if( m_pPart == NULL )
	{
		m_pPart = _NEW SkinnedCharacterPart[IPaperDoll::mptCOUNT];
	}
	if( m_pPart && iPart < IPaperDoll::mptCOUNT && iLodLevel < MAX_BEINGLOD_LV )
	{
		m_pPart[iPart].m_pMesh[iLodLevel] = pMeshSerializer;
	}
}

//-----------------------------------------------------------------------------
// Name: createSkeletonSerializer()
// Desc: jingjie modified 2006.07.14 create a skeleton serializer from cSkeletonSerializerFactory.
//-----------------------------------------------------------------------------
cSkeletonSerializer* cSkinnedCharacterModel::createSkeletonSerializer(int iModelType, 
																	  cSkeletonSerializerFactory* pSkeletonSerializerFactory,
																	  const char *szSkeletonSerializerName,
																	  const char *szPath)
{
	if( pSkeletonSerializerFactory )
	{
		m_pSkeletonSerializer = pSkeletonSerializerFactory->createSkeletonSerializer(iModelType, szSkeletonSerializerName, szPath);		
	}
	return m_pSkeletonSerializer;
}

//-----------------------------------------------------------------------------
// Name: createMeshSerializer()
// Desc: jingjie modified 2006.07.14 create a mesh serializer from cMeshSerializerFactory.
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::createMeshSerializer(cBeing* pOwnerBeing,int iCharacterPartType, cMeshSerializerFactory* pMeshSerializerFactory,const char *MeshSerializeFileName,const char *szPath)
{
	// 070816 cyhsieh Skinned Character LOD
	if( pMeshSerializerFactory )
	{
		cMeshSerializer* pDetailMeshSerializer = pMeshSerializerFactory->createMeshSerializer( MeshSerializeFileName, szPath, 0 );
		if( pDetailMeshSerializer )
		{
			if (pDetailMeshSerializer->getSubMeshSerializersCount()==0)
			{
				// 080424 cyhsieh Thread
				if( pOwnerBeing )
				{
					BYTE byType = OWNER_BEING;
					int iID = 0;
					pOwnerBeing->GetResourceOwnerData( byType, iID );
					pDetailMeshSerializer->InsertOwnerBeing( byType, iID );
					m_byWaitResourceCount++;
				}
			}
			SetPartMeshSerializer( iCharacterPartType, 0, pDetailMeshSerializer );

			// cMeshSerializer* pLodMeshSerializer = NULL;
			for( int iLodLevel = 1; iLodLevel < MAX_BEINGLOD_LV; iLodLevel++ )
			{
				SetPartMeshSerializer( iCharacterPartType, iLodLevel, pDetailMeshSerializer );
				// L: 0423, 2008 先將lod mesh的讀取關起來，現在還沒有lod mesh，如果一直嘗試去讀取，會花很多時間
				/*pLodMeshSerializer = pMeshSerializerFactory->createMeshSerializer( MeshSerializeFileName, szPath, iLodLevel );
				if( pLodMeshSerializer )
				{
				// 有Lod的Mesh
				SetPartMeshSerializer( iCharacterPartType, iLodLevel, pLodMeshSerializer );
				}
				else
				{
				// 沒有Lod的Mesh
				SetPartMeshSerializer( iCharacterPartType, iLodLevel, pDetailMeshSerializer );
				}*/
			}
		}
		else
		{
			removeMeshSerializer( iCharacterPartType );
		}
	}
}
//-----------------------------------------------------------------------------
// Name: removeMeshSerializer()
// Desc: jingjie modified 2006.07.14 remove a detail and lod  mesh serializer by CHARACTER_PART_TYPE
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::removeMeshSerializer(int iCharacterPartType)
{
	if(!m_pPart) return;

	m_pPart[iCharacterPartType].clsSkinnedCharacterPart();
}

//-----------------------------------------------------------------------------
// Name: GetMeshSerializer
// Desc: 
//-----------------------------------------------------------------------------
// 070816 cyhsieh Skinned Character LOD
cMeshSerializer* cSkinnedCharacterModel::GetPartMeshSerializer( int iPart, int iLodLevel )
{
	if( m_pPart && iPart < IPaperDoll::mptCOUNT && iLodLevel < MAX_BEINGLOD_LV )
	{
		return m_pPart[iPart].m_pMesh[iLodLevel];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: GetPartMeshTextureID
// Desc: 
//-----------------------------------------------------------------------------
// 070827 cyhsieh 人物觀察
int cSkinnedCharacterModel::GetPartMeshTextureID( int iPart )
{
	if( m_pPart && iPart < IPaperDoll::mptCOUNT )
	{
		return m_pPart[iPart].m_iTextureId;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: checkSkinnedCharacterPartsCombined()
// Desc: jingjie modified 2006.07.018 check Skinned Character Parts is combined?
//-----------------------------------------------------------------------------
bool cSkinnedCharacterModel::checkSkinnedCharacterPartsCombined()
{
	// NPC或MOB只有IPaperDoll::mptBody，所以檢查IPaperDoll::mptHair就可以
	if( m_pPart && m_pPart[IPaperDoll::mptHair].m_pMesh[0] )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name: setMeshSerializerTextureId()
// Desc: jingjie modified 2006.07.17 set texture id of mesh serializer.
//-----------------------------------------------------------------------------
// 070626 cyhsieh 檢查NPC或MOB是否改變貼圖或裝備配件
bool cSkinnedCharacterModel::setMeshSerializerTextureId( int iCharacterPartType, int iTextureId )
{
	bool bChange = false; // 回傳是否有改變

	if( m_pPart )
	{
		if( m_pPart[iCharacterPartType].m_iTextureId != iTextureId )
		{
			m_pPart[iCharacterPartType].m_iTextureId = iTextureId;
			bChange = true;
		}
	}

	return bChange;
}

//-----------------------------------------------------------------------------
// Name: checkEquipMeshVisible
// Desc: 
//-----------------------------------------------------------------------------
bool cSkinnedCharacterModel::checkEquipMeshVisible( char* pMeshName )
{
	bool bShow = true;

	if( NULL != strstr( pMeshName, "m_wea" ) )
	{
		bShow = false;

		if( m_szNoneCombinedSkinnedCharacterWeaponName )
		{
			if( NULL != strstr( pMeshName, m_szNoneCombinedSkinnedCharacterWeaponName ) )
			{
				bShow = true;
			}
		}
	}
	else if( NULL != strstr( pMeshName, "m_eq" ) )
	{
		bShow = false;

		if( m_bNoneCombinedSkinnedCharacterAdorn01 )
		{
			if( NULL != strstr( pMeshName, "m_eq01" ) )
			{
				bShow = true;
			}
		}
		if( m_bNoneCombinedSkinnedCharacterAdorn02 )
		{
			if( NULL != strstr( pMeshName, "m_eq02" ) )
			{
				bShow = true;
			}
		}
		if( m_bNoneCombinedSkinnedCharacterAdorn03 )
		{
			if( NULL != strstr( pMeshName, "m_eq03" ) )
			{
				bShow = true;
			}
		}
	}

	return bShow;
}


//-----------------------------------------------------------------------------
// Name: getSubMeshUsedSkeletonHierarchy
// Desc: 
//-----------------------------------------------------------------------------
#ifdef _CHECK_NOUSE_ANIMATION
void cSkinnedCharacterModel::getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, cSubMeshSerializer* pSubMeshSerial )
{
	if( m_pSkeletonSerializer == NULL || pSubMeshSerial == NULL )
	{
		return;
	}

	cSkinWeights* pMeshSkinWeights;
	DWORD dwSkinWeightsSize = pSubMeshSerial->getSubMeshSkinWeightsSize();
	if( dwSkinWeightsSize > 0 )
	{
		// 有SkinWeight
		for( DWORD k = 0; k < dwSkinWeightsSize; k++ )
		{
			pMeshSkinWeights = pSubMeshSerial->getSubMeshSkinWeights( k );
			//jingjie modified 2006.06.28
			// cSkeletonHierarchy* pSkeletonHierarchy = m_epFrameWork->FindFrameFromMap( pMeshSkinWeights->getSkinWeightsSkeletonName() );
			cSkeletonHierarchy* pSkeletonHierarchy = m_pSkeletonSerializer->findSkeletonHierarchy( pMeshSkinWeights->getSkinWeightsSkeletonName() );
			if( pSkeletonHierarchy )
			{
				SkeletonHierarchyUsabilityMap::iterator iter = SkeletonHierarchyUsedMap.find( pSkeletonHierarchy );
				if( iter == SkeletonHierarchyUsedMap.end() )
				{
					SkeletonHierarchyUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
				}
			}
		}
	}
	else
	{
		// 沒有SkinWeight
		//jingjie modified 2006.06.28
		//cSkeletonHierarchy* pSkeletonHierarchy = m_epFrameWork->FindFrameFromMap( pSubMeshSerial->getSubMeshName() );
		cSkeletonHierarchy* pSkeletonHierarchy = m_pSkeletonSerializer->findSkeletonHierarchy( pSubMeshSerial->getSubMeshName() );
		if( pSkeletonHierarchy )
		{
			SkeletonHierarchyUsabilityMap::iterator iter = SkeletonHierarchyUsedMap.find( pSkeletonHierarchy );
			if( iter == SkeletonHierarchyUsedMap.end() )
			{
				SkeletonHierarchyUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
			}
		}
	}
}
#endif

//-----------------------------------------------------------------------------
// Name: getUnusedSkeletonHierarchy
// Desc: 
//-----------------------------------------------------------------------------
#ifdef _CHECK_NOUSE_ANIMATION
void cSkinnedCharacterModel::getUnusedSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy, SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, bool& bParentUse )
{
	bool bSelfUse = false;
	bool bNextUse = false;

	if( pSkeletonHierarchy->getChild() )
	{
		getUnusedSkeletonHierarchy( pSkeletonHierarchy->getChild(), SkeletonHierarchyUsedMap, bSelfUse );
	}

	if( pSkeletonHierarchy->getSibling() )
	{
		getUnusedSkeletonHierarchy( pSkeletonHierarchy->getSibling(), SkeletonHierarchyUsedMap, bNextUse );
	}

	if( bSelfUse )
	{
		bParentUse = true;
	}
	else
	{
		if( bNextUse )
		{
			bParentUse = true;
		}

		SkeletonHierarchyUsabilityMap::iterator iter = SkeletonHierarchyUsedMap.find( pSkeletonHierarchy );
		if( iter != SkeletonHierarchyUsedMap.end() )
		{
			bSelfUse = true;
			bParentUse = true;
		}
		else
		{
			if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "k_weapon" ) != NULL )
			{
				bParentUse = true;
				m_SkeletonHierarchyUnUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
			}
			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "k_magic" ) != NULL )
			{
				bParentUse = true;
			}
			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "Link_Ride01" ) != NULL )
			{
				bParentUse = true;
			}
			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "Link_Ride02" ) != NULL )
			{
				bParentUse = true;
			}
			else if ( strstr( pSkeletonHierarchy->getSkeletonHierarchyName(), "Link_Ride03" ) != NULL )
			{
				bParentUse = true;
			}
			else
			{
				m_SkeletonHierarchyUnUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
			}
		}
	}
}
#endif

bool cSkinnedCharacterModel::CreateBeingRender()
{
	if (!m_pRender)
	{
		cBeingRender *pBeingRender = (cBeingRender *)g_mScene->RenderCreate( RENDER_C_BEING );
		if( pBeingRender )
		{
			m_pRender = (cBeingRender *)pBeingRender->Create();
			m_pRender->State             = RENDER_STATE_DATA_HANDLE;
			m_pRender->iRenderType       = RENDER_TYPE_1;
			m_pRender->bEnabled          = false;
			m_pRender->m_dwTextureFactor = 0xffffffff;

			setSkinnedCharacterRenderEntity( m_iRenderStyle, m_pRender );

			m_pRender->m_dwColorArg1     = D3DTA_TEXTURE;
			m_pRender->m_dwColorArg2     = D3DTA_DIFFUSE;
			m_pRender->m_dwColorOp       = D3DTOP_MODULATE;

			m_pRender->m_dwAlphaArg1     = D3DTA_TEXTURE;
			m_pRender->m_dwAlphaArg2     = D3DTA_DIFFUSE;
			m_pRender->m_dwAlphaOp       = D3DTOP_MODULATE;

			m_pRender->m_pRenderBuffer = m_pRenderBuffer;
		}
	}
	return (m_pRender!=NULL);
}

void cSkinnedCharacterModel::CheckLodMeshExisting(bool bHasLodMesh[MAX_BEINGLOD_LV])
{
	for( DWORD dwLodLevel = 0; dwLodLevel < MAX_BEINGLOD_LV; dwLodLevel++ )
	{
		if (dwLodLevel==0)
		{
			bHasLodMesh[dwLodLevel]=true;
			continue;
		}
		bHasLodMesh[dwLodLevel]=false;
		for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )  
		{
			if ((m_pPart)&&( m_pPart[iPartIdx].m_pMesh[dwLodLevel] != m_pPart[iPartIdx].m_pMesh[0] ))
			{
				bHasLodMesh[dwLodLevel] = true;
				break;
			}
		}
	}
}

void cSkinnedCharacterModel::ValidateMeshPartsVisiablility(cMeshSerializer* pMeshSerial,sModelPartData* pModelPartData,SkeletonHierarchyUsabilityMap* SkeletonHierarchyUsedMap)
{
	if (!pMeshSerial) return;
	if (!pModelPartData) return;

	DWORD dwObjMeshSize = pMeshSerial->getSubMeshSerializersCount();
	for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
	{
		cSubMeshSerializer* pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );

#ifdef _CHECK_NOUSE_ANIMATION
		getSubMeshUsedSkeletonHierarchy( *SkeletonHierarchyUsedMap, pSubMeshSerial );
#endif
		// NPC & MOB要檢查飾品、武器是否顯示
		if( checkSkinnedCharacterPartsCombined() != true)
		{
			// 060914 cyhsieh Bug Fix:同樣Model時NPC & MOB沒隱藏法術的link片
			char* pMeshName = pSubMeshSerial->getSubMeshName();
			if( NULL != strstr( pMeshName, "k_" ) )
			{
				pModelPartData->m_pShow[dwObjIndex] = false;
			}
			else
			{
				pModelPartData->m_pShow[dwObjIndex] = checkEquipMeshVisible( pSubMeshSerial->getSubMeshName() );
			}
		}
	}
}

void cSkinnedCharacterModel::ChangeSubMeshTexture(cBeing* pOwnerBeing,cMeshSerializer* pMeshSerial,sModelPartData* pModelPartData,SkinnedCharacterPart* pPart,bool* bPartHasFakeReflect)
{
	if (!pMeshSerial) return;
	if (!pModelPartData) return;
	if (!pPart) return;

	DWORD dwObjMeshSize = pMeshSerial->getSubMeshSerializersCount();

	pModelPartData->ReleaseTexture();
	pModelPartData->m_iTexID = pPart->m_iTextureId;

	// 070418 cyhsieh 人物假反光
	bool bHasFakeReflect = false;
	// 070528 cyhsieh 反光貼圖開關
	if( g_Options.GetShowFakeReflect() && ( pModelPartData->m_pFakeReflectMat == NULL ) )
	{
		pModelPartData->m_pFakeReflectMat = _NEW cMaterial[pModelPartData->m_iMaterialSize];
	}

	int iMaterialIndex = 0;

	for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
	{
		cSubMeshSerializer* pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );

		// 讀取texture
		DWORD dwMeshMaterialSize = pSubMeshSerial->getSubMeshMaterialSize();
		for( DWORD dwIndex = 0; dwIndex < dwMeshMaterialSize; dwIndex++ )
		{
			cMaterial* pMaterial = pSubMeshSerial->getSubMeshMaterial( dwIndex );
			if( pMaterial == NULL )
			{
				continue;
			}

			if( pMaterial->m_iNameSize && pMaterial->m_pszTextName )
			{
				char szFileName[MAX_PATH];
				char szTexturePath[] = "Mesh\\Being\\Tex\\";

				sprintf( szFileName, "%s%02d", pMaterial->m_pszTextName, pPart->m_iTextureId );
				pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture = m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));

				cTexture* pTexture = pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture;

				// 080424 cyhsieh Thread
				if( pTexture )
				{
					if( pTexture->GetTexture( NULL ) == NULL )
					{
						if( pOwnerBeing )
						{
							BYTE byType = OWNER_BEING;
							int iID = 0;
							pOwnerBeing->GetResourceOwnerData( byType, iID );
							pTexture->InsertOwnerBeing( byType, iID );
							m_byWaitResourceCount++;
						}
					}
				}

				// 070528 cyhsieh 反光貼圖開關
				if( pModelPartData->m_pFakeReflectMat && pTexture && pTexture->GetLoadFakeReflectTex() )
				{
					sprintf( szFileName, "%s%02d_s", pMaterial->m_pszTextName, pPart->m_iTextureId );
					pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture =m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
					if( pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture )
					{
						bHasFakeReflect = true;
						*bPartHasFakeReflect = true;
					}
					else
					{
						// 讀不到反光貼圖，以後不要再嘗試load
						pTexture->SetLoadFakeReflectTex( false );
					}
				}
			}

			iMaterialIndex++;
		}
	}

	if( !bHasFakeReflect )
	{
		SAFE_DELETE_ARRAY( pModelPartData->m_pFakeReflectMat );
	}
}

void cSkinnedCharacterModel::ChangeModelMeshPart(cBeing* pOwnerBeing,cMeshSerializer* pMeshSerial,sModelPartData* pModelPartData,SkinnedCharacterPart* pPart,bool* bPartHasFakeReflect)
{
	if (!pMeshSerial) return;
	if (!pModelPartData) return;
	if (!pPart) return;

	DWORD dwObjMeshSize = pMeshSerial->getSubMeshSerializersCount();
	if (dwObjMeshSize==0) return;

	// 計算總共的Material數量
	int iMaterialSize = 0;
	for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
	{
		cSubMeshSerializer* pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );

#ifdef _DEBUG
		// Material和AttributeTable的數量應該要一樣
		if( pSubMeshSerial->getSubMeshMaterialSize() != pSubMeshSerial->getSubMeshAttributeTableSize() )
		{
			__asm int 3
		}
#endif

		iMaterialSize += pSubMeshSerial->getSubMeshMaterialSize();
	}
	if (iMaterialSize<=0) return;

	pModelPartData->ReleaseResource();

	// 070816 cyhsieh Skinned Character LOD
	for( int iLodLevel = 0; iLodLevel < MAX_BEINGLOD_LV; iLodLevel++ )
	{
		pModelPartData->m_pMeshSerial[iLodLevel] = pPart->m_pMesh[iLodLevel];
	}

	pModelPartData->m_iTexID = pPart->m_iTextureId;
	pModelPartData->m_iMaterialSize = iMaterialSize;
	pModelPartData->m_pMaterial = _NEW cMaterial[iMaterialSize];
	pModelPartData->m_pShow = _NEW bool[dwObjMeshSize];

	// 070418 cyhsieh 人物假反光
	bool bHasFakeReflect = false;

	// 070528 cyhsieh 反光貼圖開關
	if( g_Options.GetShowFakeReflect() )
	{
		pModelPartData->m_pFakeReflectMat = _NEW cMaterial[iMaterialSize];
	}

	int iMaterialIndex = 0;

	for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
	{
		cSubMeshSerializer* pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );

		pModelPartData->m_pShow[dwObjIndex] = true;

		// 讀取texture
		DWORD dwMeshMaterialSize = pSubMeshSerial->getSubMeshMaterialSize();
		for( DWORD dwIndex = 0; dwIndex < dwMeshMaterialSize; dwIndex++ )
		{
			cMaterial* pMaterial = pSubMeshSerial->getSubMeshMaterial( dwIndex );
			if( pMaterial == NULL )
			{
				continue;
			}

			if( pMaterial->m_iNameSize && pMaterial->m_pszTextName )
			{
				char szFileName[MAX_PATH];
				char szTexturePath[] = "Mesh\\Being\\Tex\\";

				sprintf( szFileName, "%s%02d", pMaterial->m_pszTextName, pPart->m_iTextureId );
				pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture = m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));

				cTexture* pTexture = pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture;

				// 080424 cyhsieh Thread
				if( pTexture )
				{
					if( pTexture->GetTexture( NULL ) == NULL )
					{
						if( pOwnerBeing )
						{
							BYTE byType = OWNER_BEING;
							int iID = 0;
							pOwnerBeing->GetResourceOwnerData( byType, iID );
							pTexture->InsertOwnerBeing( byType, iID );
							m_byWaitResourceCount++;
						}
					}
				}

				// 070528 cyhsieh 反光貼圖開關
				if( pModelPartData->m_pFakeReflectMat && pTexture && pTexture->GetLoadFakeReflectTex() )
				{
					sprintf( szFileName, "%s%02d_s", pMaterial->m_pszTextName, pPart->m_iTextureId );
					pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture = m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
					if( pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture )
					{
						bHasFakeReflect = true;
						*bPartHasFakeReflect = true;
					}
					else
					{
						// 讀不到反光貼圖，以後不要再嘗試load
						pTexture->SetLoadFakeReflectTex( false );
					}
				}
			}

			mem_cpy( &(pModelPartData->m_pMaterial[iMaterialIndex].m_D3DMaterial), &(pMaterial->m_D3DMaterial), sizeof(D3DMATERIAL9) );

			iMaterialIndex++;
		}
	}

	// 070418 cyhsieh 人物假反光
	if( !bHasFakeReflect )
	{
		SAFE_DELETE_ARRAY( pModelPartData->m_pFakeReflectMat );
	}
}

void cSkinnedCharacterModel::CalculateMeshTotalVtxIdx(DWORD dwTotalVertex[MAX_BEINGLOD_LV],DWORD dwTotalIndex[MAX_BEINGLOD_LV],
													  bool bHasLodMesh[MAX_BEINGLOD_LV])
{
	memset(dwTotalVertex,0,sizeof(DWORD)*MAX_BEINGLOD_LV);
	memset(dwTotalIndex,0,sizeof(DWORD)*MAX_BEINGLOD_LV);
	for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
	{
		DWORD dwObjMeshSize=0;
		for( DWORD dwLodLevel = 0; dwLodLevel < MAX_BEINGLOD_LV; dwLodLevel++ )
		{
			cMeshSerializer* pMeshSerial = m_pPart[iPartIdx].m_pMesh[dwLodLevel];
			if (!pMeshSerial) continue;
			if (dwLodLevel==0) dwObjMeshSize=pMeshSerial->getSubMeshSerializersCount();
			for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
			{
				if (bHasLodMesh[dwLodLevel])
				{
					cSubMeshSerializer* pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );
					if (pSubMeshSerial)
					{
						dwTotalVertex[dwLodLevel] += pSubMeshSerial->getSubMeshVerticesBufferSize();
						dwTotalIndex[dwLodLevel] += pSubMeshSerial->getSubMeshIndexBufferSize();
					}
				}
			}
		}
	}
}

void cSkinnedCharacterModel::CopyVertexToRender(DWORD dwTotalVertex[MAX_BEINGLOD_LV],DWORD dwTotalIndex[MAX_BEINGLOD_LV],bool bHasLodMesh[MAX_BEINGLOD_LV])
{
	if (!m_pRender) return;
	if (!m_pPart) return;

	if ((dwTotalIndex[0]==0)||(dwTotalVertex[0]==0))
	{
		m_pRender->SetTotalIndexSize(0,0);
		m_pRender->SetTotalVertexSize(0,0);
		return;
	}

	MDLVTX* pRenderMesh=NULL;
	DWORD dwVerticesBufferSize;
	cMeshSerializer* pMeshSerial[MAX_BEINGLOD_LV] = { NULL,NULL,NULL };

	// 070518 cyhsieh Bug Fix: 人物Index錯誤
	m_pRender->SetTotalIndexSize( 0, dwTotalIndex[0] );

	if( dwTotalVertex[0] != m_pRender->GetTotalVertexSize( 0 ) )
	{
		pRenderMesh = _NEW MDLVTX[dwTotalVertex[0]];

		if( pRenderMesh == NULL )
		{
			m_pRender->SetTotalVertexSize( 0, 0 );
			m_pRender->SetTotalIndexSize( 0, 0 );
			m_pRender->SetRenderMesh( 0, NULL );
		}
		else
		{
			m_pRender->SetTotalVertexSize( 0, dwTotalVertex[0] );
			m_pRender->SetRenderMesh( 0, pRenderMesh );
		}
	}
	pRenderMesh = m_pRender->GetRenderMesh( 0 );
	if( pRenderMesh )
	{
		int iStartIndex = 0;
		for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
		{
			pMeshSerial[0] = m_pPart[iPartIdx].m_pMesh[0];
			if( pMeshSerial[0] == NULL )
			{
				continue;
			}

			DWORD dwObjMeshSize = pMeshSerial[0]->getSubMeshSerializersCount();
			if (dwObjMeshSize>0)
			{
				for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
				{
					cSubMeshSerializer* pSubMeshSerial = pMeshSerial[0]->getSubMeshSerializer( dwObjIndex );
					if (!pSubMeshSerial) continue;

					dwVerticesBufferSize = pSubMeshSerial->getSubMeshVerticesBufferSize();
					if (dwVerticesBufferSize==0) continue;

					mem_cpy( pRenderMesh + iStartIndex, pSubMeshSerial->getSubMeshVerticesBuffer(), sizeof(MDLVTX) * dwVerticesBufferSize );

					iStartIndex += dwVerticesBufferSize;
				}
			}
		}
		ASSERT(iStartIndex<=(int)dwTotalVertex[0]);
	}

	// 070816 cyhsieh Skinned Character LOD
	for( int iLodLevel = 1; iLodLevel < MAX_BEINGLOD_LV; iLodLevel++ )
	{
		if( bHasLodMesh[iLodLevel] )
		{
			m_pRender->SetTotalIndexSize( iLodLevel, dwTotalIndex[iLodLevel] );

			// 有Lod的話，copy Lod的點資料
			if( dwTotalVertex[iLodLevel] != m_pRender->GetTotalVertexSize( iLodLevel ) )
			{
				pRenderMesh = _NEW MDLVTX[dwTotalVertex[iLodLevel]];

				if( pRenderMesh == NULL )
				{
					m_pRender->SetTotalVertexSize( iLodLevel, 0 );
					m_pRender->SetTotalIndexSize( iLodLevel, 0 );
					m_pRender->SetRenderMesh( iLodLevel, NULL );
				}
				else
				{
					m_pRender->SetTotalVertexSize( iLodLevel, dwTotalVertex[iLodLevel] );
					m_pRender->SetRenderMesh( iLodLevel, pRenderMesh );
				}
			}
			pRenderMesh = m_pRender->GetRenderMesh( iLodLevel );
			if( pRenderMesh )
			{
				int iStartIndex = 0;
				for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
				{
					pMeshSerial[iLodLevel] = m_pPart[iPartIdx].m_pMesh[iLodLevel];
					if( pMeshSerial[iLodLevel] == NULL )
					{
						continue;
					}

					DWORD dwObjMeshSize = pMeshSerial[iLodLevel]->getSubMeshSerializersCount();
					if (dwObjMeshSize>0)
					{
						for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
						{
							cSubMeshSerializer* pSubMeshSerial = pMeshSerial[iLodLevel]->getSubMeshSerializer( dwObjIndex );
							if (!pSubMeshSerial) continue;

							dwVerticesBufferSize = pSubMeshSerial->getSubMeshVerticesBufferSize();
							if (dwVerticesBufferSize==0) continue;

							mem_cpy( pRenderMesh + iStartIndex, pSubMeshSerial->getSubMeshVerticesBuffer(), sizeof(MDLVTX) * dwVerticesBufferSize );

							iStartIndex += dwVerticesBufferSize;
						}
					}
				}
			}
		}
		else
		{
			// 沒有Lod，清除資料
			m_pRender->SetTotalVertexSize( iLodLevel, 0 );
			m_pRender->SetTotalIndexSize( iLodLevel, 0 );
			m_pRender->SetRenderMesh( iLodLevel, NULL );
		}
	}
}
//-----------------------------------------------------------------------------
// Name: combineSkinnedCharacterPartsMesh()
// Desc: jingjie modified 2006.07.17 combine each character part mesh to one character's mesh.
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::combineSkinnedCharacterPartsMesh(cBeing* pOwnerBeing)
{
	PROFILE_SCOPE( BeingModel_combineSkinnedCharacterPartsMesh );

	char szTexturePath[] = "Mesh\\Being\\Tex\\";

	if( m_pRender == NULL )
	{
		bool res=CreateBeingRender();
		if (!res) return;
	}

	m_pRender->State = RENDER_STATE_DATA_HANDLE;
	m_pRender->bEnabled = false;

	bool bModelChange = false;

	sModelPartData* pModelPartData;
	cMeshSerializer* pMeshSerial[MAX_BEINGLOD_LV] = { NULL };
	bool bHasLodMesh[MAX_BEINGLOD_LV] = { false };
	DWORD	dwObjMeshSize;
	DWORD dwTotalVertex[MAX_BEINGLOD_LV] = { 0 };
	DWORD dwTotalIndex[MAX_BEINGLOD_LV] = { 0 };
	int iMaterialIndex = 0;

#ifdef _CHECK_NOUSE_ANIMATION
	SkeletonHierarchyUsabilityMap SkeletonHierarchyUsedMap;
#endif

	if (m_pPart)
	{
		// 070418 cyhsieh 人物假反光
		bool bPartHasFakeReflect = false;

		// 070816 cyhsieh Skinned Character LOD
		// 檢查是否有Lod的Mesh
		CheckLodMeshExisting(bHasLodMesh);

		for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
		{
			pMeshSerial[0] = m_pPart[iPartIdx].m_pMesh[0];
			if( pMeshSerial[0] == NULL )
			{
				// 070518 cyhsieh Bug Fix:部位刪掉Render資料沒有更新
				pModelPartData = m_pRender->GetModelPartData( iPartIdx );
				if( pModelPartData )
				{
					pModelPartData->ReleaseResource();
				}

				continue;
			}

			pModelPartData = m_pRender->GetModelPartData( iPartIdx );
			if( pModelPartData == NULL )
			{
				continue;
			}

			// 070816 cyhsieh Skinned Character LOD
			for( DWORD dwLodLevel = 1; dwLodLevel < MAX_BEINGLOD_LV; dwLodLevel++ )
			{
				pMeshSerial[dwLodLevel] = m_pPart[iPartIdx].m_pMesh[dwLodLevel];
			}

			bool bSameModel = ( pModelPartData->m_pMeshSerial[0] == pMeshSerial[0] );             // Model是否相同
			bool bSameMaterial = ( pModelPartData->m_iTexID == m_pPart[iPartIdx].m_iTextureId );  // Texture是否相同

			dwObjMeshSize = pMeshSerial[0]->getSubMeshSerializersCount();

			// 分三種情況處理
			// 1. Model改變
			// 2. Model沒變Texture改變
			// 3. Model沒變Texture沒變

			if( bSameModel )
			{
				if( bSameMaterial )
				{
					ValidateMeshPartsVisiablility(pMeshSerial[0],pModelPartData,&SkeletonHierarchyUsedMap);

					// 3. Model沒變Texture沒變
					/*
					for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
					{
					pSubMeshSerial = pMeshSerial[0]->getSubMeshSerializer( dwObjIndex );

					#ifdef _CHECK_NOUSE_ANIMATION
					getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsedMap, pSubMeshSerial );
					#endif

					// NPC & MOB要檢查飾品、武器是否顯示
					if( checkSkinnedCharacterPartsCombined() != true)
					{
					// 060914 cyhsieh Bug Fix:同樣Model時NPC & MOB沒隱藏法術的link片
					char* pMeshName = pSubMeshSerial->getSubMeshName();
					if( NULL != strstr( pMeshName, "k_" ) )
					{
					pModelPartData->m_pShow[dwObjIndex] = false;
					}
					else
					{
					pModelPartData->m_pShow[dwObjIndex] = checkEquipMeshVisible( pSubMeshSerial->getSubMeshName() );
					}
					}

					// 070816 cyhsieh Skinned Character LOD
					dwTotalVertex[0] += pSubMeshSerial->getSubMeshVerticesBufferSize();
					dwTotalIndex[0] += pSubMeshSerial->getSubMeshIndexBufferSize();
					for( DWORD dwLodLevel = 1; dwLodLevel < MAX_BEINGLOD_LV; dwLodLevel++ )
					{
					if( bHasLodMesh[dwLodLevel] )
					{
					pLodSubMeshSerial = pMeshSerial[dwLodLevel]->getSubMeshSerializer( dwObjIndex );
					dwTotalVertex[dwLodLevel] += pLodSubMeshSerial->getSubMeshVerticesBufferSize();
					dwTotalIndex[dwLodLevel] += pLodSubMeshSerial->getSubMeshIndexBufferSize();
					}
					}
					} //*/
				}
				else
				{
					ChangeSubMeshTexture(pOwnerBeing,pMeshSerial[0],pModelPartData,&m_pPart[iPartIdx],&bPartHasFakeReflect);
					ValidateMeshPartsVisiablility(pMeshSerial[0],pModelPartData,&SkeletonHierarchyUsedMap);
					// 2. Model沒變Texture改變
					/*
					pModelPartData->ReleaseTexture();
					pModelPartData->m_iTexID = m_pPart[iPartIdx].m_iTextureId;

					// 070418 cyhsieh 人物假反光
					bool bHasFakeReflect = false;
					// 070528 cyhsieh 反光貼圖開關
					if( g_Options.GetShowFakeReflect() && ( pModelPartData->m_pFakeReflectMat == NULL ) )
					{
					pModelPartData->m_pFakeReflectMat = _NEW cMaterial[pModelPartData->m_iMaterialSize];
					}

					iMaterialIndex = 0;

					for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
					{
					pSubMeshSerial = pMeshSerial[0]->getSubMeshSerializer( dwObjIndex );

					#ifdef _CHECK_NOUSE_ANIMATION
					getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsedMap, pSubMeshSerial );
					#endif

					// NPC & MOB要檢查飾品、武器是否顯示
					if( checkSkinnedCharacterPartsCombined() != true)
					{
					// 060914 cyhsieh Bug Fix:同樣Model時NPC & MOB沒隱藏法術的link片
					char* pMeshName = pSubMeshSerial->getSubMeshName();
					if( NULL != strstr( pMeshName, "k_" ) )
					{
					pModelPartData->m_pShow[dwObjIndex] = false;
					}
					else
					{
					pModelPartData->m_pShow[dwObjIndex] = checkEquipMeshVisible( pSubMeshSerial->getSubMeshName() );
					}
					}

					// 讀取texture
					DWORD dwMeshMaterialSize = pSubMeshSerial->getSubMeshMaterialSize();
					for( DWORD dwIndex = 0; dwIndex < dwMeshMaterialSize; dwIndex++ )
					{
					cMaterial* pMaterial = pSubMeshSerial->getSubMeshMaterial( dwIndex );
					if( pMaterial == NULL )
					{
					continue;
					}

					if( pMaterial->m_iNameSize && pMaterial->m_pszTextName )
					{
					sprintf( szFileName, "%s%02d", pMaterial->m_pszTextName, m_pPart[iPartIdx].m_iTextureId );
					pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture = m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0, OWNER_BEING, iOwnerGUID ));

					// 070528 cyhsieh 反光貼圖開關
					cTexture* pTexture = pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture;
					if( pModelPartData->m_pFakeReflectMat && pTexture && pTexture->GetLoadFakeReflectTex() )
					{
					sprintf( szFileName, "%s%02d_s", pMaterial->m_pszTextName, m_pPart[iPartIdx].m_iTextureId );
					pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture =m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
					if( pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture )
					{
					bHasFakeReflect = true;
					bPartHasFakeReflect = true;
					}
					else
					{
					// 讀不到反光貼圖，以後不要再嘗試load
					pTexture->SetLoadFakeReflectTex( false );
					}
					}
					}

					iMaterialIndex++;
					}

					// 070816 cyhsieh Skinned Character LOD
					dwTotalVertex[0] += pSubMeshSerial->getSubMeshVerticesBufferSize();
					dwTotalIndex[0] += pSubMeshSerial->getSubMeshIndexBufferSize();
					for( DWORD dwLodLevel = 1; dwLodLevel < MAX_BEINGLOD_LV; dwLodLevel++ )
					{
					if( bHasLodMesh[dwLodLevel] )
					{
					pLodSubMeshSerial = pMeshSerial[dwLodLevel]->getSubMeshSerializer( dwObjIndex );
					dwTotalVertex[dwLodLevel] += pLodSubMeshSerial->getSubMeshVerticesBufferSize();
					dwTotalIndex[dwLodLevel] += pLodSubMeshSerial->getSubMeshIndexBufferSize();
					}
					}
					}

					// 070418 cyhsieh 人物假反光
					if( !bHasFakeReflect )
					{
					SAFE_DELETE_ARRAY( pModelPartData->m_pFakeReflectMat );
					} //*/
				}
			}
			else
			{
				// 1. Model改變
				bModelChange = true;

				ChangeModelMeshPart(pOwnerBeing,pMeshSerial[0],pModelPartData,&m_pPart[iPartIdx],&bPartHasFakeReflect);
				ValidateMeshPartsVisiablility(pMeshSerial[0],pModelPartData,&SkeletonHierarchyUsedMap);

				// 計算總共的Material數量
				/*
				int iMaterialSize = 0;
				for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
				{
				pSubMeshSerial = pMeshSerial[0]->getSubMeshSerializer( dwObjIndex );

				#ifdef _DEBUG
				// Material和AttributeTable的數量應該要一樣
				if( pSubMeshSerial->getSubMeshMaterialSize() != pSubMeshSerial->getSubMeshAttributeTableSize() )
				{
				__asm int 3
				}
				#endif

				iMaterialSize += pSubMeshSerial->getSubMeshMaterialSize();
				}

				pModelPartData->ReleaseResource();

				// 070816 cyhsieh Skinned Character LOD
				for( int iLodLevel = 0; iLodLevel < MAX_BEINGLOD_LV; iLodLevel++ )
				{
				pModelPartData->m_pMeshSerial[iLodLevel] = m_pPart[iPartIdx].m_pMesh[iLodLevel];
				}

				pModelPartData->m_iTexID = m_pPart[iPartIdx].m_iTextureId;
				pModelPartData->m_iMaterialSize = iMaterialSize;
				pModelPartData->m_pMaterial = _NEW cMaterial[iMaterialSize];
				pModelPartData->m_pShow = _NEW bool[dwObjMeshSize];

				// 070418 cyhsieh 人物假反光
				bool bHasFakeReflect = false;

				// 070528 cyhsieh 反光貼圖開關
				if( g_Options.GetShowFakeReflect() )
				{
				pModelPartData->m_pFakeReflectMat = _NEW cMaterial[iMaterialSize];
				}

				iMaterialIndex = 0;

				for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
				{
				pSubMeshSerial = pMeshSerial[0]->getSubMeshSerializer( dwObjIndex );

				pModelPartData->m_pShow[dwObjIndex] = true;

				#ifdef _CHECK_NOUSE_ANIMATION
				getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsedMap, pSubMeshSerial );
				#endif

				// NPC & MOB要檢查飾品、武器是否顯示
				if( checkSkinnedCharacterPartsCombined() != true)
				{
				char* pMeshName = pSubMeshSerial->getSubMeshName();

				if( NULL != strstr( pMeshName, "k_" ) )
				{
				pModelPartData->m_pShow[dwObjIndex] = false;
				}
				else
				{
				pModelPartData->m_pShow[dwObjIndex] = checkEquipMeshVisible( pSubMeshSerial->getSubMeshName() );
				}
				}

				// 讀取texture
				DWORD dwMeshMaterialSize = pSubMeshSerial->getSubMeshMaterialSize();
				for( DWORD dwIndex = 0; dwIndex < dwMeshMaterialSize; dwIndex++ )
				{
				cMaterial* pMaterial = pSubMeshSerial->getSubMeshMaterial( dwIndex );
				if( pMaterial == NULL )
				{
				continue;
				}

				if( pMaterial->m_iNameSize && pMaterial->m_pszTextName )
				{
				sprintf( szFileName, "%s%02d", pMaterial->m_pszTextName, m_pPart[iPartIdx].m_iTextureId );
				pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture = m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0, OWNER_BEING, iOwnerGUID ));

				// 070528 cyhsieh 反光貼圖開關
				cTexture* pTexture = pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture;
				if( pModelPartData->m_pFakeReflectMat && pTexture && pTexture->GetLoadFakeReflectTex() )
				{
				sprintf( szFileName, "%s%02d_s", pMaterial->m_pszTextName, m_pPart[iPartIdx].m_iTextureId );
				pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture = m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
				if( pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture )
				{
				bHasFakeReflect = true;
				bPartHasFakeReflect = true;
				}
				else
				{
				// 讀不到反光貼圖，以後不要再嘗試load
				pTexture->SetLoadFakeReflectTex( false );
				}
				}
				}

				mem_cpy( &pModelPartData->m_pMaterial[iMaterialIndex].m_D3DMaterial, &pMaterial->m_D3DMaterial, sizeof(D3DMATERIAL9) );

				iMaterialIndex++;
				}

				// 070816 cyhsieh Skinned Character LOD
				dwTotalVertex[0] += pSubMeshSerial->getSubMeshVerticesBufferSize();
				dwTotalIndex[0] += pSubMeshSerial->getSubMeshIndexBufferSize();
				for( DWORD dwLodLevel = 1; dwLodLevel < MAX_BEINGLOD_LV; dwLodLevel++ )
				{
				if( bHasLodMesh[dwLodLevel] )
				{
				pLodSubMeshSerial = pMeshSerial[dwLodLevel]->getSubMeshSerializer( dwObjIndex );
				dwTotalVertex[dwLodLevel] += pLodSubMeshSerial->getSubMeshVerticesBufferSize();
				dwTotalIndex[dwLodLevel] += pLodSubMeshSerial->getSubMeshIndexBufferSize();
				}
				}
				}

				// 070418 cyhsieh 人物假反光
				if( !bHasFakeReflect )
				{
				SAFE_DELETE_ARRAY( pModelPartData->m_pFakeReflectMat );
				} //*/
			}
		} // end of for (int iPartIdx)

		// 070418 cyhsieh 人物假反光
		if( bPartHasFakeReflect )
		{
			cTexture* pTexture = m_pTextureDB->QueryTexture("FakeReflect",szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
			m_pRender->SetFakeReflectTexture( pTexture );
		}

	} // end of (m_pPart)

	if( bModelChange )
	{
		// 080424 cyhsieh Thread
		if( m_byWaitResourceCount > 0 )
		{
			m_pRender->SetTotalIndexSize( 0, 0 ); // 等模型相關資源讀取結束後，要重新計算點資料
			return;
		}
		else
		{
			CalculateMeshTotalVtxIdx(dwTotalVertex,dwTotalIndex,bHasLodMesh);
			CopyVertexToRender(dwTotalVertex,dwTotalIndex,bHasLodMesh);
		}
		// Model改變要重新copy點資料
		/*
		MDLVTX* pRenderMesh;
		DWORD dwVerticesBufferSize;

		// 070518 cyhsieh Bug Fix: 人物Index錯誤
		m_pRender->SetTotalIndexSize( 0, dwTotalIndex[0] );

		if( dwTotalVertex[0] != m_pRender->GetTotalVertexSize( 0 ) )
		{
		pRenderMesh = _NEW MDLVTX[dwTotalVertex[0]];

		if( pRenderMesh == NULL )
		{
		m_pRender->SetTotalVertexSize( 0, 0 );
		m_pRender->SetTotalIndexSize( 0, 0 );
		m_pRender->SetRenderMesh( 0, NULL );
		}
		else
		{
		m_pRender->SetTotalVertexSize( 0, dwTotalVertex[0] );
		m_pRender->SetRenderMesh( 0, pRenderMesh );
		}
		}
		pRenderMesh = m_pRender->GetRenderMesh( 0 );
		if( pRenderMesh )
		{
		int iStartIndex = 0;
		for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
		{
		pMeshSerial[0] = m_pPart[iPartIdx].m_pMesh[0];
		if( pMeshSerial[0] == NULL )
		{
		continue;
		}

		dwObjMeshSize = pMeshSerial[0]->getSubMeshSerializersCount();
		for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
		{
		pSubMeshSerial = pMeshSerial[0]->getSubMeshSerializer( dwObjIndex );

		dwVerticesBufferSize = pSubMeshSerial->getSubMeshVerticesBufferSize();

		mem_cpy( pRenderMesh + iStartIndex, pSubMeshSerial->getSubMeshVerticesBuffer(), sizeof(MDLVTX) * dwVerticesBufferSize );

		iStartIndex += dwVerticesBufferSize;
		}
		}
		}

		// 070816 cyhsieh Skinned Character LOD
		for( int iLodLevel = 1; iLodLevel < MAX_BEINGLOD_LV; iLodLevel++ )
		{
		if( bHasLodMesh[iLodLevel] )
		{
		m_pRender->SetTotalIndexSize( iLodLevel, dwTotalIndex[iLodLevel] );

		// 有Lod的話，copy Lod的點資料
		if( dwTotalVertex[iLodLevel] != m_pRender->GetTotalVertexSize( iLodLevel ) )
		{
		pRenderMesh = _NEW MDLVTX[dwTotalVertex[iLodLevel]];

		if( pRenderMesh == NULL )
		{
		m_pRender->SetTotalVertexSize( iLodLevel, 0 );
		m_pRender->SetTotalIndexSize( iLodLevel, 0 );
		m_pRender->SetRenderMesh( iLodLevel, NULL );
		}
		else
		{
		m_pRender->SetTotalVertexSize( iLodLevel, dwTotalVertex[iLodLevel] );
		m_pRender->SetRenderMesh( iLodLevel, pRenderMesh );
		}
		}
		pRenderMesh = m_pRender->GetRenderMesh( iLodLevel );
		if( pRenderMesh )
		{
		int iStartIndex = 0;
		for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
		{
		pMeshSerial[iLodLevel] = m_pPart[iPartIdx].m_pMesh[iLodLevel];
		if( pMeshSerial[iLodLevel] == NULL )
		{
		continue;
		}

		dwObjMeshSize = pMeshSerial[iLodLevel]->getSubMeshSerializersCount();
		for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
		{
		pSubMeshSerial = pMeshSerial[iLodLevel]->getSubMeshSerializer( dwObjIndex );

		dwVerticesBufferSize = pSubMeshSerial->getSubMeshVerticesBufferSize();

		mem_cpy( pRenderMesh + iStartIndex, pSubMeshSerial->getSubMeshVerticesBuffer(), sizeof(MDLVTX) * dwVerticesBufferSize );

		iStartIndex += dwVerticesBufferSize;
		}
		}
		}
		}
		else
		{
		// 沒有Lod，清除資料
		m_pRender->SetTotalVertexSize( iLodLevel, 0 );
		m_pRender->SetTotalIndexSize( iLodLevel, 0 );
		m_pRender->SetRenderMesh( iLodLevel, NULL );
		}
		}*/
		m_pRender->ComputeLowestLodLevel();
	}

#ifdef _CHECK_NOUSE_ANIMATION
	m_SkeletonHierarchyUnUsedMap.clear();
	if( m_pSkeletonSerializer )
	{
		bool bUse;
		getUnusedSkeletonHierarchy( m_pSkeletonSerializer->getSkeletonHierarchyRoot(), SkeletonHierarchyUsedMap, bUse );
	}
#endif

	m_pRender->State = RENDER_STATE_DATA_OK;
	m_pRender->bEnabled = true;
}

void cSkinnedCharacterModel::ReCombineSkinnedCharacterPartsMesh( cBeing* pOwnerBeing )
{
	// 080424 cyhsieh Thread
	if( m_byWaitResourceCount > 0 )
	{
		return;
	}

	if( m_pRender == NULL ) return;
	if( m_pPart == NULL ) return;

	sModelPartData* pModelPartData;
	cMeshSerializer* pMeshSerial;

	bool bHasLodMesh[MAX_BEINGLOD_LV];
	DWORD dwTotalVertex[MAX_BEINGLOD_LV];
	DWORD dwTotalIndex[MAX_BEINGLOD_LV];

#ifdef _CHECK_NOUSE_ANIMATION
	SkeletonHierarchyUsabilityMap SkeletonHierarchyUsedMap;
#endif

	for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
	{
		pMeshSerial = m_pPart[iPartIdx].m_pMesh[0];
		if (!pMeshSerial) continue;

		pModelPartData = m_pRender->GetModelPartData( iPartIdx );
		if (!pModelPartData) continue;

		char szTexturePath[] = "Mesh\\Being\\Tex\\";
		bool bPartHasFakeReflect = false;
		ChangeModelMeshPart(pOwnerBeing,pMeshSerial,pModelPartData,&m_pPart[iPartIdx],&bPartHasFakeReflect);
		if ((m_pTextureDB)&&( bPartHasFakeReflect ))
		{
			cTexture* pTexture = m_pTextureDB->QueryTexture("FakeReflect",szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
			m_pRender->SetFakeReflectTexture( pTexture );
		}
		ValidateMeshPartsVisiablility(pMeshSerial,pModelPartData,&SkeletonHierarchyUsedMap);
	}

	CheckLodMeshExisting(bHasLodMesh);
	CalculateMeshTotalVtxIdx(dwTotalVertex,dwTotalIndex,bHasLodMesh);
	CopyVertexToRender(dwTotalVertex,dwTotalIndex,bHasLodMesh);
	m_pRender->ComputeLowestLodLevel();

#ifdef _CHECK_NOUSE_ANIMATION
	m_SkeletonHierarchyUnUsedMap.clear();
	if( m_pSkeletonSerializer )
	{
		bool bUse;
		getUnusedSkeletonHierarchy( m_pSkeletonSerializer->getSkeletonHierarchyRoot(), SkeletonHierarchyUsedMap, bUse );
	}
#endif

	m_pRender->State = RENDER_STATE_DATA_OK;
	m_pRender->bEnabled = true;
}

//-----------------------------------------------------------------------------
// Name: createAnimationAction()
// Desc: jingjie modified 2006.07.17 create a new animation action by index with fading out prevoius animation action 
//       and set new aniamtion action operation , play speed, fade out time. 
//-----------------------------------------------------------------------------
int cSkinnedCharacterModel::createAnimationAction( int iActionIndex, float fPlaySpeed, AnimationActionOperation animationActionOperation, float fFadeoutTime )
{
	if (iActionIndex < 0) return -1; // 不合法的動作索引, 不進行動作變更
	if (m_pSkeletonSerializer == NULL)    return -1; // 沒有m_pSkeletonSerializer, 無法進行動作變更
	if (m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount() == 0)  return -1; // 沒有動作表, 無法進行動作變更

	// 060502 cyhsieh Act Loading 流程簡化
	m_pSkeletonSerializer->prepareAnimationKeys( iActionIndex );

	// 淡出前一個動作.
	if (fFadeoutTime > 0.0f)
	{
		// 060513 cyhsieh Act Loading 流程簡化
		m_pSkeletonSerializer->prepareAnimationKeys( m_CurnAction.m_iIndex );

		m_FadeoutAction.m_iIndex          = m_CurnAction.m_iIndex;
		m_FadeoutAction.m_fAnimTime       = m_CurnAction.m_fAnimTime;         // 從頭開始播放
		m_FadeoutAction.m_fAnimTimeLength = m_CurnAction.m_fAnimTimeLength; // 記錄動作時間總長
		m_FadeoutAction.m_fPlaySpeed      = m_CurnAction.m_fPlaySpeed;
		m_fFadeoutTimeRemain              = fFadeoutTime;
		m_fFadeoutTime                    = fFadeoutTime;
	} // end of if (bFadeoutLastAction)
	else 
	{ // 沒有動作需要淡出
		m_fFadeoutTime = 1.0f;
		m_fFadeoutTimeRemain = 0.0f;
		m_FadeoutAction.m_iIndex = -1;
	}

	// 避免分母為 0, 直接令 m_fFadeoutTime 等於 1
	if (m_fFadeoutTime < 0.0f) m_fFadeoutTime = 1.0f;

	cAnimationActionInfo* pAnimationActionInfo = m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo( iActionIndex );

	if (pAnimationActionInfo == NULL) return -1;
	// 新動作的起始時間 
	DWORD dwStartTime = pAnimationActionInfo->getAnimationActionStartTime();
	// 新動作的結束時間 
	DWORD dwEndTime   = pAnimationActionInfo->getAnimationActionEndTime();
	// 動作時間總長(200 unit per frame)
	m_CurnAction.m_iIndex = iActionIndex; // 記錄新的動作索引
	// 每個Frame 200個單位, 24個Frame共4800個單位, 從0開始計算, 最後一張應為 4600, 所以減去 200
	m_CurnAction.m_fAnimTimeLength = float(dwEndTime - dwStartTime) - ((animationActionOperation & aoIgnoreCountBackwards1ndKeyframe) ? 200 : 0); // 記錄動作時間總長
	// 如果有特別指定, 就鎖在最後一張, 不然就從第一張開始播放
	m_CurnAction.m_fAnimTime = (animationActionOperation & aoCountBackwards2ndKeyframe) ? m_CurnAction.m_fAnimTimeLength : 0; // 播放起始位置

	m_CurnAction.m_fPlaySpeed = fPlaySpeed;

	return iActionIndex;
}


//-----------------------------------------------------------------------------
// Name: createAnimationActionIntoBlending
// Desc: jingjie modified 2006.07.17 create a new animation action by index into blending current animation action and set it's playing speed , fading out time. 
//-----------------------------------------------------------------------------
int cSkinnedCharacterModel::createAnimationActionIntoBlending( int iActionIndex, float fPlaySpeed, float fFadeoutTime )
{
	if (iActionIndex < 0) return -1; // 不合法的動作索引, 不進行動作變更
	if (m_pSkeletonSerializer == NULL)    return -1; // 沒有FrameWork, 無法進行動作變更
	if (m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount() == 0)  return -1; // 沒有動作表, 無法進行動作變更

	// 060502 cyhsieh Act Loading 流程簡化
	m_pSkeletonSerializer->prepareAnimationKeys( iActionIndex );

	cAnimationActionInfo* pAnimationActionInfo = m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo( iActionIndex );
	if (pAnimationActionInfo == NULL) return -1;

	// 新動作的起始時間
	//jingjie modified 2006.06.29
	//jingjie test 2006.07.08
	//DWORD dwStartTime = m_pSkeletonSerializer->getActionTable()->GetActTable(iActionIndex)->dwStartTime;
	//jingjie test 2006.07.09
	//DWORD dwStartTime = m_pSkeletonSerializer->getActionTable()->GetActTable(iActionIndex)->getAnimationActionStartTime();
	DWORD dwStartTime = m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo(iActionIndex)->getAnimationActionStartTime();
	// 新動作的結束時間 
	//jingjie modified 2006.06.29
	//jingjie test 2006.07.08
	//DWORD dwEndTime   = m_pSkeletonSerializer->getActionTable()->GetActTable(iActionIndex)->dwEndTime;
	//jingjie test 2006.07.09
	//DWORD dwEndTime   = m_pSkeletonSerializer->getActionTable()->GetActTable(iActionIndex)->getAnimationActionEndTime();
	DWORD dwEndTime   = m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo(iActionIndex)->getAnimationActionEndTime();

	m_FadeoutAction.m_iIndex          = iActionIndex;
	m_FadeoutAction.m_fAnimTime       = 0;         // 從頭開始播放
	m_FadeoutAction.m_fAnimTimeLength = float(dwEndTime - dwStartTime); // 記錄動作時間總長
	m_FadeoutAction.m_fPlaySpeed      = fPlaySpeed;
	m_fFadeoutTimeRemain              = fFadeoutTime;
	m_fFadeoutTime                    = fFadeoutTime;

	return 0;
}
//-----------------------------------------------------------------------------
// Name: tickAnimationAction()
// Desc: jingjie modified 2006.07.17 tick animation action by frame time.
//-----------------------------------------------------------------------------
bool cSkinnedCharacterModel::tickAnimationAction(float fElapsedTime)
{
	// 動作時間會受 m_fPlaySpeed (動作播放速率) 所影響...
	// 在各種不同的動作下有不同的PlaySpeed, 
	// m_fPlaySpeed 會在必要的時候被設定成不一樣的速率
	m_CurnAction.m_fAnimTime  += (fElapsedTime * 4800 * m_CurnAction.m_fPlaySpeed);
	if (m_CurnAction.m_fAnimTime > m_CurnAction.m_fAnimTimeLength)
	{
		// 鎖定在最後一張Frame
		m_CurnAction.m_fAnimTime = m_CurnAction.m_fAnimTimeLength;
	} // end of if (m_FadeoutAction.m_fAnimTime > m_FadeoutAction.m_fAnimTimeLength)
	// 有動作正在淡出...
	if (m_fFadeoutTimeRemain > 0.0f)
	{
		// 遞減淡出時間
		m_fFadeoutTimeRemain -= fElapsedTime;
		// 淡出完畢!!!
		if (m_fFadeoutTimeRemain <= 0.0f)
		{
			m_FadeoutAction.m_iIndex = -1;
			m_fFadeoutTimeRemain = 0.0f;
			m_fFadeoutTime       = 1.0f;
		} // end of if (m_fFadeoutTimeRemain < 0.0f)
		else 
		{
			// 淡出的動作不再加時間... 
			m_FadeoutAction.m_fAnimTime += (fElapsedTime * 4800 * m_FadeoutAction.m_fPlaySpeed);
			if (m_FadeoutAction.m_fAnimTime > m_FadeoutAction.m_fAnimTimeLength)
			{
				// 鎖定在最後一張Frame
				// 070305 cyhsieh Bug Fix:淡出的動作應該是最後的動作
				// 因為m_fAnimTime會 fmod m_fAnimTimeLength，這樣就會回到第0個frame的動作
				// !!!To do... 有沒有比較好的方法?
				//m_FadeoutAction.m_fAnimTime = m_FadeoutAction.m_fAnimTimeLength;
				m_FadeoutAction.m_fAnimTime = m_FadeoutAction.m_fAnimTimeLength;

			} // end of if (m_FadeoutAction.m_fAnimTime > m_FadeoutAction.m_fAnimTimeLength)
		} // end of if (m_fFadeoutTimeRemain > 0.0f)
	} // end of if (m_fFadeoutTimeRemain > 0.0f)

	// 070503 cyhsieh 裝備特效
	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && pEquipPart->getEquipVisible() )
			{
				pEquipPart->TickAnimTime( fElapsedTime );
			}
		}
	}

	return true; 
}

//-----------------------------------------------------------------------------
// Name: UpdateRenderMesh
// Desc:
//-----------------------------------------------------------------------------
bool cSkinnedCharacterModel::UpdateRenderMesh( D3DXMATRIXA16* pRootTransform, bool bUpdateRenderMesh )
{
	// 080424 cyhsieh Thread
	if( m_byWaitResourceCount > 0 )
	{
		return false;
	}

	if( !m_pSkeletonSerializer || !m_pSkeletonSerializer->getAnimationRoot() )
	{
		return false;
	}
	if( m_pRender == NULL )
	{
		return false;
	}

	DWORD dwVertexSize = m_pRender->GetCurrentTotalVertexSize();
	MDLVTX* pRenderMesh = m_pRender->GetCurrentRenderMesh();
	if( dwVertexSize == 0 || pRenderMesh == NULL )
	{
		return false;
	}

	if( bUpdateRenderMesh == false )
	{
		// 不更新計算點資料，但由於骨架是共用的，為了處理法術或裝備連結點，骨架動作還是需要計算一次
#ifdef _CHECK_NOUSE_ANIMATION
		m_pSkeletonSerializer->computeSkeletonHierarchyMatrix( m_sComputeAnimParam,
			pRootTransform,
			&m_SkeletonHierarchyUnUsedMap );
#else
		m_pSkeletonSerializer->computeSkeletonHierarchyMatrix( m_sComputeAnimParam,
			pRootTransform );
#endif
		return false;
	}

	m_sComputeAnimParam.m_iActionIndex1 = m_CurnAction.m_iIndex;
	m_sComputeAnimParam.m_iActionIndex2 = m_FadeoutAction.m_iIndex;
	m_sComputeAnimParam.m_fAnimTime1 = m_CurnAction.m_fAnimTime;
	m_sComputeAnimParam.m_fAnimTime2 = m_FadeoutAction.m_fAnimTime;
	m_sComputeAnimParam.m_fLerpValue = m_fFadeoutTimeRemain / m_fFadeoutTime;

	// 計算動作
#ifdef _CHECK_NOUSE_ANIMATION
	// 070425 cyhsieh 修改Mount Matrix的計算
	m_pSkeletonSerializer->computeSkeletonHierarchyMatrix( m_sComputeAnimParam,
		pRootTransform,
		&m_SkeletonHierarchyUnUsedMap );
#else
	// 070425 cyhsieh 修改Mount Matrix的計算
	m_pSkeletonSerializer->computeSkeletonHierarchyMatrix( m_sComputeAnimParam,
		pRootTransform );
#endif

	for( unsigned int i = 0; i < dwVertexSize; i++ )
	{
		pRenderMesh[i].p.x = 0.0f;
		pRenderMesh[i].p.y = 0.0f;
		pRenderMesh[i].p.z = 0.0f;
		pRenderMesh[i].n.x = 0.0f;
		pRenderMesh[i].n.y = 0.0f;
		pRenderMesh[i].n.z = 0.0f;
	}

	cMeshSerializer* pMeshSerial;
	cSubMeshSerializer* pSubMeshSerial;
	DWORD	dwObjMeshSize;
	cAnimation* pAnimation;
	D3DXMATRIXA16 matCombined;
	cSkinWeights* pSkinWeight;
	int iStartIndex = 0;
	DWORD dwSkinIndex;
	D3DXVECTOR3 vPos, vNormal;
	cSkeletonHierarchy* pSkeletonHierarchy;

	// 070625 cyhsieh 不顯示的Submesh不計算
	sModelPartData* pModelPartData;
	D3DXVECTOR3 vSubMeshMin, vSubMeshMax;
	if (m_pLocalSpaceBoundingBoxMin)
	{
		m_pLocalSpaceBoundingBoxMin->x = FLT_MAX;
		m_pLocalSpaceBoundingBoxMin->y = FLT_MAX;
		m_pLocalSpaceBoundingBoxMin->z = FLT_MAX;
	}
	if (m_pLocalSpaceBoundingBoxMax)
	{
		m_pLocalSpaceBoundingBoxMax->x = -FLT_MAX;
		m_pLocalSpaceBoundingBoxMax->y = -FLT_MAX;
		m_pLocalSpaceBoundingBoxMax->z = -FLT_MAX;
	}

	DWORD dwLodLevel = m_pRender->GetLodLevel();
	for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
	{
		// 070816 cyhsieh Skinned Character LOD
		pMeshSerial=NULL;
		if (m_pPart) pMeshSerial = m_pPart[iPartIdx].m_pMesh[dwLodLevel];
		if( pMeshSerial == NULL )
		{
			continue;
		}

		dwObjMeshSize = pMeshSerial->getSubMeshSerializersCount();
		if (dwObjMeshSize==0) continue;
		for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
		{
			pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );
			if (!pSubMeshSerial) continue;

			MDLVTX* pVertexBuffer = pSubMeshSerial->getSubMeshVerticesBuffer();
			DWORD dwVertexBufferSize = pSubMeshSerial->getSubMeshVerticesBufferSize();
			if (!pVertexBuffer) continue;
			if (dwVertexBufferSize==0) continue;

			// 070625 cyhsieh 不顯示的Submesh不計算
			pModelPartData = m_pRender->GetModelPartData( iPartIdx );
			if ((!pModelPartData)||(!pModelPartData->m_pShow)) continue;
			if( (pModelPartData) && (pModelPartData->m_pShow) && (pModelPartData->m_pShow[dwObjIndex] == false) )
			{
				iStartIndex += dwVertexBufferSize;
				continue;
			}

			DWORD dwSkinWeightsSize = pSubMeshSerial->getSubMeshSkinWeightsSize();
			if( dwSkinWeightsSize > 0 )
			{
				for( DWORD dwIndex = 0; dwIndex < dwSkinWeightsSize; dwIndex++ )
				{
					pSkinWeight = pSubMeshSerial->getSubMeshSkinWeights( dwIndex );
					if (!pSkinWeight) continue;
					pSkeletonHierarchy = m_pSkeletonSerializer->findSkeletonHierarchy( pSkinWeight->getSkinWeightsSkeletonName() );
					if( pSkeletonHierarchy )
					{
						//jingjie modified 2006.06.27
						pAnimation = pSkeletonHierarchy->getAnimation();
						if( pAnimation == NULL )
						{
							iStartIndex += dwVertexBufferSize;
							continue;
						}
					}
					else
					{
						iStartIndex += dwVertexBufferSize;
						continue;
					}
					//jingjie test 2006.07.07
					//matCombined = *(pSkinWeight->getSkinWeightsSkeletonMatrix()) * pAnimation->m_mxMatrix;
					matCombined = *(pSkinWeight->getSkinWeightsSkeletonMatrix()) * (*pSkeletonHierarchy->getSkeletonHierarchyMatrix());

					// 把skin weight, skin vertex index的陣列先取出存放，不要在loop內一直抓，節省一點點時間
					float* skin_weights=pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesWeight();
					unsigned int vtx_count=pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesSize();
					DWORD* skin_index=pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesIndex();

					for( unsigned int i = 0; i < vtx_count; i++ ) //pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesSize(); i++ )
					{
						dwSkinIndex = skin_index[i]; //*pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesIndex(i);

						if (skin_weights) //( pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesWeight() )
						{
							D3DXVec3TransformCoord( &vPos,
								&pVertexBuffer[dwSkinIndex].p,
								&matCombined );
							D3DXVec3TransformNormal( &vNormal,
								&pVertexBuffer[dwSkinIndex].n,
								&matCombined );
							pRenderMesh[dwSkinIndex + iStartIndex].p += (skin_weights[i] * vPos); //( vPos * (*pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesWeight(i) ));
							pRenderMesh[dwSkinIndex + iStartIndex].n += (skin_weights[i] * vNormal); //( vNormal * (*pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesWeight(i)) );
						}
						else
						{
							D3DXVec3TransformCoord( &pRenderMesh[dwSkinIndex + iStartIndex].p,
								&pVertexBuffer[dwSkinIndex].p,
								&matCombined );

							D3DXVec3TransformNormal( &pRenderMesh[dwSkinIndex + iStartIndex].n,
								&pVertexBuffer[dwSkinIndex].n,
								&matCombined );
						}
					}
				}
			}
			else
			{
				pSkeletonHierarchy = m_pSkeletonSerializer->findSkeletonHierarchy( pSubMeshSerial->getSubMeshName() );
				if( pSkeletonHierarchy == NULL )
				{
					iStartIndex += dwVertexBufferSize;
					continue;
				}

				for( DWORD dwIndex = 0; dwIndex < dwVertexBufferSize; dwIndex++ )
				{
					D3DXVec3TransformCoord( &pRenderMesh[dwIndex + iStartIndex].p,
						&pVertexBuffer[dwIndex].p,
						pSkeletonHierarchy->getSkeletonHierarchyMatrix() );

					D3DXVec3TransformNormal( &pRenderMesh[dwIndex + iStartIndex].n,
						&pVertexBuffer[dwIndex].n,
						pSkeletonHierarchy->getSkeletonHierarchyMatrix() );
				}
			}

			// 070625 cyhsieh 不顯示的Submesh不計算
			D3DXComputeBoundingBox( &pRenderMesh[iStartIndex].p, dwVertexBufferSize, D3DXGetFVFVertexSize(D3DFVF_MDLVTX), &vSubMeshMin, &vSubMeshMax );
			if (m_pLocalSpaceBoundingBoxMin)
			{
				if( vSubMeshMin.x < m_pLocalSpaceBoundingBoxMin->x ) m_pLocalSpaceBoundingBoxMin->x = vSubMeshMin.x;
				if( vSubMeshMin.y < m_pLocalSpaceBoundingBoxMin->y ) m_pLocalSpaceBoundingBoxMin->y = vSubMeshMin.y;
				if( vSubMeshMin.z < m_pLocalSpaceBoundingBoxMin->z ) m_pLocalSpaceBoundingBoxMin->z = vSubMeshMin.z;
			}
			if (m_pLocalSpaceBoundingBoxMax)
			{
				if( vSubMeshMax.x > m_pLocalSpaceBoundingBoxMax->x ) m_pLocalSpaceBoundingBoxMax->x = vSubMeshMax.x;
				if( vSubMeshMax.y > m_pLocalSpaceBoundingBoxMax->y ) m_pLocalSpaceBoundingBoxMax->y = vSubMeshMax.y;
				if( vSubMeshMax.z > m_pLocalSpaceBoundingBoxMax->z ) m_pLocalSpaceBoundingBoxMax->z = vSubMeshMax.z;
			}

			iStartIndex += dwVertexBufferSize;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: tickSkinnedCharacterMesh()
// Desc: jingjie modified 2006.07.17 tick  skinned mesh of character and before running tickSkinnedCharacterMesh 
//       ,we must be already excuted tickAnimationAction first;
//-----------------------------------------------------------------------------
// 070307 cyhsieh 馬的影子
bool cSkinnedCharacterModel::tickSkinnedCharacterMesh( bool bCreateSkinnedCharacterMesh, D3DXMATRIXA16* pMountMat )
{
	bool bResult = false;

	if (m_pSkeletonSerializer == NULL) return bResult;

	// 在這裡處理Model的Animation
	{
		//jingjie add 2006.07.17. tickSkinnedCharacterMesh之前必須先tickAnimationAction,才能正確createSkinnedCharacterMesh.
		PROFILE_SCOPE(BeingModel_UpdateRenderMesh);
		// 070307 cyhsieh 馬的影子
		bResult = UpdateRenderMesh( pMountMat, bCreateSkinnedCharacterMesh );
	} // end of if (bComputeSkinMesh)

	// [George] 070730 修正:雙人騎乘時,後座會配件會離位
	// 這邊也要更新,否則沒騎馬的時候反而會配件錯過更新
	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && pEquipPart->getEquipVisible() )
			{
				pEquipPart->ComputeAnimation();
				pEquipPart->SetWorldTransform( m_matMatrix );
			}
		}
	}

	return bResult;
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionInfoCount()
// Desc: jingjie modified 2006.07.17 get group size of animation action keyframes range .
//-----------------------------------------------------------------------------
int cSkinnedCharacterModel::getAnimationActionInfoCount()
{
	//jingjie test 2006.07.17 for thread struct , now not in using.
	//if(DataState != BEING_DATA_STATE_DATA_OK) return -1;
	//jingjie modified 2006.06.29
	//jingjie test 2006.07.12 
	if( m_pSkeletonSerializer == 0 ) return -1;
	return m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount();
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionTimeLength()
// Desc: jingjie modified 2006.07.17 get time length of animation action by action index 
//-----------------------------------------------------------------------------
DWORD cSkinnedCharacterModel::getAnimationActionTimeLength(int iActionIndex)
{
	//jingjie test 2006.07.17 for thread struct , now not in using.
	//if (DataState != BEING_DATA_STATE_DATA_OK) return 0;
	if (m_pSkeletonSerializer == NULL)  
		return 0;
	//jingjie modified 2006.06.29
	//jingjie modified 2006.07.12
	//if (iActionIndex >= m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount()) 
	//return 0;
	//jingjie modified 2006.06.29
	//jingjie modified 2006.07.12
	if (m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfoCount() == 0) 
		return 0;
	//jingjie modified 2006.06.29
	//jingjie modified 2006.07.08
	/*return m_pSkeletonSerializer->getActionTable()->GetActTable(iActionIndex)->dwEndTime -
	m_pSkeletonSerializer->getActionTable()->GetActTable(iActionIndex)->dwStartTime;
	*/
	//return m_pSkeletonSerializer->getActionTable()->GetActTable(iActionIndex)->getAnimationActionTimeLength();
	//jingjie modified 2006.07.09
	return m_pSkeletonSerializer->getAnimationActionInfoGroup()->getAnimationActionInfo(iActionIndex)->getAnimationActionTimeLength();
}
//-----------------------------------------------------------------------------
// Name: getCurrentAnimationActionTimeLength()
// Desc: jingjie modified 2006.07.17 get time length of current animation action
//-----------------------------------------------------------------------------
DWORD cSkinnedCharacterModel::getCurrentAnimationActionTimeLength()
{
	return getAnimationActionTimeLength(m_CurnAction.m_iIndex);
}

//-----------------------------------------------------------------------------
// Name: createSkinnedCharacterShadowData
// Desc:
//----------------------------------------------------------------------------
// 060420 cyhsieh 實體影子
void cSkinnedCharacterModel::createSkinnedCharacterShadowData( LPDIRECT3DDEVICE9 pD3DDDevice )
{
	if( m_pTextureDB == NULL || m_pRender == NULL || pD3DDDevice == NULL )
	{
		return;
	}

	if( !m_pShadowData )
	{
		m_pShadowData = _NEW sShadowData;
		if( m_pShadowData )
		{
			char szTmp[32];
			sprintf( szTmp, "%x", this );
			m_pShadowData->m_pTexture = m_pTextureDB->CreateDynamicTexture( szTmp, 128, 128, true, true );
		}
	}

	if( m_pRender )
	{
		m_pRender->SetShadowData( m_pShadowData );
	}
}

//-----------------------------------------------------------------------------
// Name: ComputeShadowData
// Desc:
//----------------------------------------------------------------------------
void cSkinnedCharacterModel::ComputeShadowData( cSkinnedCharacterModel* pMountModel )
{
	if( m_pShadowData )   // 根據Bounding Box計算影子所需的大小、等等
	{
		D3DXVECTOR3 vec[8];
		float fMinX = FLT_MAX;
		float fMaxX = -FLT_MAX;
		float fMinZ = FLT_MAX;
		float fMaxZ = -FLT_MAX;

		// 計算人物的world bounding box
		D3DXVECTOR3 vMin( FLT_MAX, FLT_MAX, FLT_MAX );
		D3DXVECTOR3 vMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vec[0] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMin->x, m_pLocalSpaceBoundingBoxMin->y, m_pLocalSpaceBoundingBoxMin->z );
		vec[1] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMax->x, m_pLocalSpaceBoundingBoxMin->y, m_pLocalSpaceBoundingBoxMin->z );
		vec[2] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMin->x, m_pLocalSpaceBoundingBoxMax->y, m_pLocalSpaceBoundingBoxMin->z );
		vec[3] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMin->x, m_pLocalSpaceBoundingBoxMin->y, m_pLocalSpaceBoundingBoxMax->z );
		vec[4] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMax->x, m_pLocalSpaceBoundingBoxMax->y, m_pLocalSpaceBoundingBoxMin->z );
		vec[5] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMax->x, m_pLocalSpaceBoundingBoxMin->y, m_pLocalSpaceBoundingBoxMax->z );
		vec[6] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMin->x, m_pLocalSpaceBoundingBoxMax->y, m_pLocalSpaceBoundingBoxMax->z );
		vec[7] = D3DXVECTOR3( m_pLocalSpaceBoundingBoxMax->x, m_pLocalSpaceBoundingBoxMax->y, m_pLocalSpaceBoundingBoxMax->z );
		for( int j = 0; j < 8; j++ )
		{
			D3DXVec3TransformCoord( &vec[j], &vec[j], &m_matMatrix );
			if( vec[j].x > vMax.x )
			{
				vMax.x = vec[j].x;
			}
			if( vec[j].x < vMin.x )
			{
				vMin.x = vec[j].x;
			}
			if( vec[j].y > vMax.y )
			{
				vMax.y = vec[j].y;
			}
			if( vec[j].y < vMin.y )
			{
				vMin.y = vec[j].y;
			}
			if( vec[j].z > vMax.z )
			{
				vMax.z = vec[j].z;
			}
			if( vec[j].z < vMin.z )
			{
				vMin.z = vec[j].z;
			}
		}  

		// 計算武器的world bounding box
		if( m_pEquipPartList )
		{
			cSkinnedCharacterEquipPart* pEquipPart;
			int iCount = m_pEquipPartList->GetCount();
			for( int i = 0; i < iCount; i++ )
			{
				pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
				if( pEquipPart && pEquipPart->getEquipVisible() )
				{
					cEquipModel* pModel = pEquipPart->GetEquipModel();
					if( pModel == NULL ) continue;

					vec[0] = D3DXVECTOR3( pModel->GetBoundingBoxMin().x, pModel->GetBoundingBoxMin().y, pModel->GetBoundingBoxMin().z );
					vec[1] = D3DXVECTOR3( pModel->GetBoundingBoxMax().x, pModel->GetBoundingBoxMin().y, pModel->GetBoundingBoxMin().z );
					vec[2] = D3DXVECTOR3( pModel->GetBoundingBoxMin().x, pModel->GetBoundingBoxMax().y, pModel->GetBoundingBoxMin().z );
					vec[3] = D3DXVECTOR3( pModel->GetBoundingBoxMin().x, pModel->GetBoundingBoxMin().y, pModel->GetBoundingBoxMax().z );
					vec[4] = D3DXVECTOR3( pModel->GetBoundingBoxMax().x, pModel->GetBoundingBoxMax().y, pModel->GetBoundingBoxMin().z );
					vec[5] = D3DXVECTOR3( pModel->GetBoundingBoxMax().x, pModel->GetBoundingBoxMin().y, pModel->GetBoundingBoxMax().z );
					vec[6] = D3DXVECTOR3( pModel->GetBoundingBoxMin().x, pModel->GetBoundingBoxMax().y, pModel->GetBoundingBoxMax().z );
					vec[7] = D3DXVECTOR3( pModel->GetBoundingBoxMax().x, pModel->GetBoundingBoxMax().y, pModel->GetBoundingBoxMax().z );

					for( int j = 0; j < 8; j++ )
					{
						D3DXVec3TransformCoord( &vec[j], &vec[j], &pEquipPart->GetEqMatrix() );

						if( vec[j].x > vMax.x ) vMax.x = vec[j].x;
						if( vec[j].x < vMin.x ) vMin.x = vec[j].x;
						if( vec[j].y > vMax.y ) vMax.y = vec[j].y;
						if( vec[j].y < vMin.y ) vMin.y = vec[j].y;
						if( vec[j].z > vMax.z ) vMax.z = vec[j].z;
						if( vec[j].z < vMin.z ) vMin.z = vec[j].z;
					} 
				}
			}
		}

		// 070307 cyhsieh 馬的影子
		if( pMountModel )
		{
			const D3DXVECTOR3& vMountBoundingMin = pMountModel->getWorldBoundingBoxMin();
			const D3DXVECTOR3& vMountBoungindMax = pMountModel->getWorldBoundingBoxMax();

			vec[0] = D3DXVECTOR3( vMountBoundingMin.x, vMountBoundingMin.y, vMountBoundingMin.z );
			vec[1] = D3DXVECTOR3( vMountBoungindMax.x, vMountBoundingMin.y, vMountBoundingMin.z );
			vec[2] = D3DXVECTOR3( vMountBoundingMin.x, vMountBoungindMax.y, vMountBoundingMin.z );
			vec[3] = D3DXVECTOR3( vMountBoundingMin.x, vMountBoundingMin.y, vMountBoungindMax.z );
			vec[4] = D3DXVECTOR3( vMountBoungindMax.x, vMountBoungindMax.y, vMountBoundingMin.z );
			vec[5] = D3DXVECTOR3( vMountBoungindMax.x, vMountBoundingMin.y, vMountBoungindMax.z );
			vec[6] = D3DXVECTOR3( vMountBoundingMin.x, vMountBoungindMax.y, vMountBoungindMax.z );
			vec[7] = D3DXVECTOR3( vMountBoungindMax.x, vMountBoungindMax.y, vMountBoungindMax.z );

			for( int j = 0; j < 8; j++ )
			{
				if( vec[j].x > vMax.x )
				{
					vMax.x = vec[j].x;
				}
				if( vec[j].x < vMin.x )
				{
					vMin.x = vec[j].x;
				}
				if( vec[j].y > vMax.y )
				{
					vMax.y = vec[j].y;
				}
				if( vec[j].y < vMin.y )
				{
					vMin.y = vec[j].y;
				}
				if( vec[j].z > vMax.z )
				{
					vMax.z = vec[j].z;
				}
				if( vec[j].z < vMin.z )
				{
					vMin.z = vec[j].z;
				}
			}

			cBeingRender* pMountRender = pMountModel->getSkinnedCharacterRenderEntity();
			if( pMountRender )
			{
				pMountRender->SetShadowData( m_pShadowData );
			}
		}

		vec[0] = D3DXVECTOR3( vMin.x, vMin.y, vMin.z );
		vec[1] = D3DXVECTOR3( vMax.x, vMin.y, vMin.z );
		vec[2] = D3DXVECTOR3 (vMin.x, vMax.y, vMin.z );
		vec[3] = D3DXVECTOR3( vMin.x, vMin.y, vMax.z );
		vec[4] = D3DXVECTOR3( vMax.x, vMax.y, vMin.z );
		vec[5] = D3DXVECTOR3( vMax.x, vMin.y, vMax.z );
		vec[6] = D3DXVECTOR3( vMin.x, vMax.y, vMax.z );
		vec[7] = D3DXVECTOR3( vMax.x, vMax.y, vMax.z );

		D3DXPLANE plane;
		D3DXVECTOR3 vPos = D3DXVECTOR3( m_matMatrix._41, m_matMatrix._42, m_matMatrix._43 ); 
		D3DXVECTOR3 vNormal = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); 
		D3DXPlaneFromPointNormal( &plane, &vPos, &vNormal ); 

		// 070307 cyhsieh 馬的影子
		D3DXVECTOR4 vLight = D3DXVECTOR4( -0.45f, 0.85f, -0.28f, 0.0f );

		D3DXMatrixShadow( &m_pShadowData->m_matPlane, &vLight, &plane );

		for( int i = 0; i < 8; i++ )
		{
			D3DXVec3TransformCoord( &vec[i], &vec[i], &m_pShadowData->m_matPlane );
			if( vec[i].x > fMaxX )
			{
				fMaxX = vec[i].x;
			}
			if( vec[i].x < fMinX )
			{
				fMinX = vec[i].x;
			}
			if( vec[i].z > fMaxZ )
			{
				fMaxZ = vec[i].z;
			}
			if( vec[i].z < fMinZ )
			{
				fMinZ = vec[i].z;
			}
		}

		D3DXVECTOR3 vOrig = D3DXVECTOR3( m_matMatrix._41, m_matMatrix._42, m_matMatrix._43 );
		D3DXVec3TransformCoord( &vOrig, &vOrig, &m_pShadowData->m_matPlane );

		float fPercent = 0.9f; // 影子佔貼圖的比例
		float fSizeX = ( fMaxX - fMinX ) / fPercent;
		float fSizeZ = ( fMaxZ - fMinZ ) / fPercent;
		float fOffsetX = ( fMaxX - fMinX ) * 0.5f - ( vOrig.x - fMinX );
		float fOffsetZ = ( fMaxZ - fMinZ ) * 0.5f - ( vOrig.z - fMinZ );
		D3DXVECTOR3 vEyeAt, vLookAt, vUp;

		vEyeAt = D3DXVECTOR3( m_matMatrix._41 + fOffsetX, m_matMatrix._42 + 20.0f, m_matMatrix._43 + fOffsetZ );
		vLookAt = D3DXVECTOR3( m_matMatrix._41 + fOffsetX, m_matMatrix._42, m_matMatrix._43 + fOffsetZ );
		vUp = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXMatrixLookAtLH( &m_pShadowData->m_matView, &vEyeAt, &vLookAt, &vUp );
		D3DXMatrixOrthoLH( &m_pShadowData->m_matProj, fSizeX, fSizeZ, 1.0f, 25.0f );

		m_pShadowData->m_vOffset.x = fSizeX * ( 1.0f - fPercent ) * 0.5f + ( vOrig.x - fMinX );
		m_pShadowData->m_vOffset.y = fSizeZ * ( 1.0f - fPercent ) * 0.5f + ( vOrig.z - fMinZ );
		m_pShadowData->m_vSize.x = fSizeX;
		m_pShadowData->m_vSize.y = fSizeZ;
	}
}

//-----------------------------------------------------------------------------
// Name: setSkinnedCharacterPosition()
// Desc: 
//-----------------------------------------------------------------------------
// 070413 cyhsieh 人物特效
void cSkinnedCharacterModel::setSkinnedCharacterPosition( float fPosX, float fPosY, float fPosZ )
{
	D3DXMatrixTranslation( &m_matTranslation, fPosX, fPosY, fPosZ );

	m_bUpadteWorldMatrix = true;

	if( m_pRender )
	{
		// 070816 cyhsieh Skinned Character LOD
		ComputeLod();

		m_pRender->SetNowPos( fPosX, fPosY, fPosZ );
	}
}

//-----------------------------------------------------------------------------
// Name: setSkinnedCharacterDirection()
// Desc: jingjie modified 2006.07.18 set Skinned Character's direction
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::setSkinnedCharacterDirection(float fDirection)
{
	//  if(iDirect == (int)direction) return;

	m_iDirection = (int)fDirection;
	D3DXMatrixRotationY(&m_matRotation, ((m_iDirection + 360) % 360) * (D3DX_PI / 180.0f));

	m_bUpadteWorldMatrix = true;
	//  m_pRender->SetDirection(direction);

}

//-----------------------------------------------------------------------------
// Name: setSkinnedCharacterScale()
// Desc: jingjie modified 2006.07.18 set Skinned Character's scale
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::setSkinnedCharacterScale(float fScaleX,float fScaleY,float fScaleZ)
{
	D3DXMatrixScaling( &m_matScaling, fScaleX, fScaleY, fScaleZ );

	m_bUpadteWorldMatrix = true;
}

//-----------------------------------------------------------------------------
// Name: createSkinnedCharacterMatrix()
// Desc: jingjie modified 2006.07.18 create SkinnedCharacter's Matrix
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::createSkinnedCharacterMatrix( void )
{
	// 需要更新世界矩陣??
	if (m_bUpadteWorldMatrix)
	{
		D3DXMatrixMultiply( &m_matMatrix, &m_matScaling, &m_matRotation);
		D3DXMatrixMultiply( &m_matMatrix, &m_matMatrix, &m_matTranslation);

		if(m_pLocalSpaceBoundingBoxMin && m_pLocalSpaceBoundingBoxMax)
		{
			//jingjie modified 2005.09.09
			D3DXVECTOR3 SBox[8];   
			SBox[0]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMin->x,m_pLocalSpaceBoundingBoxMin->y,m_pLocalSpaceBoundingBoxMin->z);
			SBox[1]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMax->x,m_pLocalSpaceBoundingBoxMin->y,m_pLocalSpaceBoundingBoxMin->z);
			SBox[2]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMin->x,m_pLocalSpaceBoundingBoxMax->y,m_pLocalSpaceBoundingBoxMin->z);
			SBox[3]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMin->x,m_pLocalSpaceBoundingBoxMin->y,m_pLocalSpaceBoundingBoxMax->z);
			SBox[4]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMax->x,m_pLocalSpaceBoundingBoxMax->y,m_pLocalSpaceBoundingBoxMin->z);
			SBox[5]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMax->x,m_pLocalSpaceBoundingBoxMin->y,m_pLocalSpaceBoundingBoxMax->z);
			SBox[6]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMin->x,m_pLocalSpaceBoundingBoxMax->y,m_pLocalSpaceBoundingBoxMax->z);
			SBox[7]= D3DXVECTOR3(m_pLocalSpaceBoundingBoxMax->x,m_pLocalSpaceBoundingBoxMax->y,m_pLocalSpaceBoundingBoxMax->z);
			D3DXVECTOR3 minbox(999999999.9f,99999999.9f,999999999.9f), maxbox(-999999999.9f,-99999999.9f,-999999999.9f);
			for(int i = 0; i < 8; i++)
			{
				D3DXVec3TransformCoord(&SBox[i],&SBox[i],&m_matMatrix);	
				if( SBox[i].x > maxbox.x )
				{
					maxbox.x = SBox[i].x;
				}
				if( SBox[i].x < minbox.x )
				{
					minbox.x = SBox[i].x;
				}
				if( SBox[i].y > maxbox.y )
				{
					maxbox.y = SBox[i].y;
				}
				if( SBox[i].y < minbox.y )
				{
					minbox.y = SBox[i].y;
				}
				if( SBox[i].z > maxbox.z )
				{
					maxbox.z = SBox[i].z;
				}
				if( SBox[i].z < minbox.z )
				{
					minbox.z = SBox[i].z;
				}
			}    
			*m_pWorldSpaceBoundingBoxMax = D3DXVECTOR3(maxbox.x ,maxbox.y ,maxbox.z );
			*m_pWorldSpaceBoundingBoxMin = D3DXVECTOR3(minbox.x ,minbox.y ,minbox.z );
		}
	} // end of if (bUpdateWorldMatrix

	if (m_pRender)
	{
		SBUG( m_pRender );
		m_pRender->SetWorldMatrix(m_matMatrix);
	} // end of if (m_pRender)

	m_bUpadteWorldMatrix = false;  
}

//-----------------------------------------------------------------------------
// Name: getSkinnedCharacterMatrix()
// Desc: jingjie modified 2006.07.18 get Skinned Character's matrix
//-----------------------------------------------------------------------------
const D3DXMATRIXA16& cSkinnedCharacterModel::getSkinnedCharacterMatrix()
{
	m_matMatrix = m_matScaling * m_matRotation * m_matTranslation;

	return m_matMatrix;
}

//-----------------------------------------------------------------------------
// Name: setSkinnedCharacterRenderEntity
// Desc: jingjie modified 2006.07.17 set render entity style of Skinned Character.
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::setSkinnedCharacterRenderEntity(RenderEntityStyle iStyle, cBeingRender* pRender)
{
	cRender* pTarget;

	if (pRender) pTarget = pRender;
	else         pTarget = m_pRender;

	// 屬性設定
	m_iRenderStyle = iStyle;

	if( pTarget )
	{
		switch( iStyle )
		{
		case rsNone:
			// 開啟 Blending 功能
			pTarget->m_bAlphaBlendenable = TRUE;
			// 忽略來源色
			pTarget->m_dwSrcBlend  = D3DBLEND_ZERO;
			pTarget->m_dwDestBlend = D3DBLEND_ONE; 
			break;
		case rsDefault:
			// 繪製實體
			pTarget->m_bAlphaBlendenable = FALSE;
			// 計算 Alpha, 不過Blend 設為 FALSE, 底下的設定並不會作用.
			pTarget->m_dwSrcBlend  = D3DBLEND_SRCALPHA;
			pTarget->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
			break;
		case rsAddColor:
			// 開啟 Blending 功能
			pTarget->m_bAlphaBlendenable = TRUE;
			// 來源色 * (SRC_BLEND = 1) 與 目的色 * (DEST_BLEND = 1) 相加
			pTarget->m_dwSrcBlend  = D3DBLEND_ONE;
			pTarget->m_dwDestBlend = D3DBLEND_ONE;
			pTarget->SetFadeStyle( cRender::fsAdd ); // 改成加色
			break;
		case rsFadeIn:
			pTarget->SetFadeTypeEasy( 0.0f );
			break;
		case rsFadeOut:
			pTarget->SetFadeTypeEasy( 255.0f );
			break;
		}
	}

	if( m_pEquipPartList )
	{
		float fFadeValue;
		switch( iStyle )
		{
		case rsFadeIn:
			fFadeValue = 0.0f;
			break;
		case rsFadeOut:
			fFadeValue = 255.0f;
			break;
		default:
			return;
		}
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && pEquipPart->IsModelExist() )
			{
				pEquipPart->SetFade( fFadeValue );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: SetEquipPart
// Desc: 設定裝備模型
//-----------------------------------------------------------------------------
void cSkinnedCharacterModel::SetEquipPart( BYTE byPartID, cEquipModel* pEquipModel, int iTextureID,
										  bool bWeaponRibbonLerp, cBeing* pOwnerBeing )
{
	if( byPartID >= IPaperDoll::eptCOUNT )
	{
		return;
	}

	BYTE byLinkID;
	if( byPartID == IPaperDoll::eptFishingRod )
	{
		byLinkID = IPaperDoll::eptWeaponR;  // 釣竿使用右手連結點
	}
	else
	{
		byLinkID = byPartID;
	}

	cSkinnedCharacterEquipPart* pEquipPart = NULL;
	if( pEquipModel )
	{
		if( m_pEquipPartList == NULL )
		{
			m_pEquipPartList = _NEW BTList;
			if( m_pEquipPartList == NULL )
			{
				return;
			}
			pEquipPart = _NEW cSkinnedCharacterEquipPart( byPartID, byLinkID );
			if( pEquipPart )
			{
				m_pEquipPartList->Add( pEquipPart );
			}
		}
		else
		{
			int i;
			int iCount = m_pEquipPartList->GetCount();
			for( i = 0; i < iCount; i++ )
			{
				pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
				if( pEquipPart && ( pEquipPart->GetPartID() == byPartID ) )
				{
					// 此部位已在List中
					break;
				}
			}
			if( i == iCount )
			{
				// 此部位不在List中
				pEquipPart = _NEW cSkinnedCharacterEquipPart( byPartID, byLinkID );
				if( pEquipPart )
				{
					m_pEquipPartList->Add( pEquipPart );
				}
			}
		}

		if( pEquipPart )
		{
			switch( byPartID )
			{
			case IPaperDoll::eptWeaponL:        // 各式武器
			case IPaperDoll::eptWeaponR:        // 各式武器
			case IPaperDoll::eptForearmL:       // 腕盾,拳刃
			case IPaperDoll::eptForearmR:       // 腕盾,拳刃
			case IPaperDoll::eptHandbackL:      // 塔盾,持盾,巨盾
			case IPaperDoll::eptHandbackR:      // 塔盾,持盾,巨盾
				pEquipPart->SetForceHide( m_bHideWeaponEquip ); // 武器是否顯示
				break;
			}

			pEquipPart->SetEquipModel( pEquipModel, iTextureID, bWeaponRibbonLerp, pOwnerBeing );
			if( byPartID != IPaperDoll::eptFishingRod )
			{
				pEquipPart->SetShadowData( m_pShadowData ); // 釣竿不畫影子
			}

			if( m_pSkeletonSerializer )
			{
				cSkeletonHierarchy* pSkeletonHierarchy = m_pSkeletonSerializer->getWeaponSkeletonHierarchy( byLinkID );

				if( pSkeletonHierarchy )
				{
					pEquipPart->setEquipSkeletonHierarchy( pSkeletonHierarchy );

#ifdef _CHECK_NOUSE_ANIMATION
					SkeletonHierarchyUsabilityMap::iterator iter = m_SkeletonHierarchyUnUsedMap.find( pSkeletonHierarchy );
					if( iter != m_SkeletonHierarchyUnUsedMap.end() )
					{
						m_SkeletonHierarchyUnUsedMap.erase( iter );
					}
#endif
				}
			}
		}
	}
	else
	{
		// 刪掉此部位
		if( m_pEquipPartList )
		{
			int iCount = m_pEquipPartList->GetCount();
			for( int i = 0; i < iCount; i++ )
			{
				pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
				if( pEquipPart && ( pEquipPart->GetPartID() == byPartID ) )
				{
					delete pEquipPart;
					m_pEquipPartList->Exchange( i, iCount - 1 );
					m_pEquipPartList->Delete( iCount - 1 );
					break;
				}
			}

#ifdef _CHECK_NOUSE_ANIMATION
			if( m_pSkeletonSerializer )
			{
				int iCount = m_pEquipPartList->GetCount();
				for( int i = 0; i < iCount; i++ )
				{
					pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
					if( pEquipPart && ( pEquipPart->GetLinkID() == byLinkID ) )
					{
						return;
					}
				}
				cSkeletonHierarchy* pSkeletonHierarchy = m_pSkeletonSerializer->getWeaponSkeletonHierarchy( byLinkID );
				SkeletonHierarchyUsabilityMap::iterator iter = m_SkeletonHierarchyUnUsedMap.find( pSkeletonHierarchy );
				if( iter == m_SkeletonHierarchyUnUsedMap.end() )
				{
					m_SkeletonHierarchyUnUsedMap[pSkeletonHierarchy] = pSkeletonHierarchy;
				}
			}
#endif
		}
	}
}

//-----------------------------------------------------------------------------
// Name: GetEquipPartByPartID
// Desc: 根據部位取得裝備
//-----------------------------------------------------------------------------
cSkinnedCharacterEquipPart* cSkinnedCharacterModel::GetEquipPartByPartID( BYTE byPartID )
{
	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && ( pEquipPart->GetPartID() == byPartID ) )
			{
				return pEquipPart;
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: GetEquipPartByIndex
// Desc: 根據索引取得裝備
//-----------------------------------------------------------------------------
cSkinnedCharacterEquipPart* cSkinnedCharacterModel::GetEquipPartByIndex( int iIndex )
{
	if( m_pEquipPartList )
	{
		return (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( iIndex );
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: setSkinnedCharacterEquip()
// Desc: jingjie modified 2006.07.20 set none-combied skinned character's weapon.
//-----------------------------------------------------------------------------
// 070626 cyhsieh 檢查NPC或MOB是否改變貼圖或裝備配件
bool cSkinnedCharacterModel::setSkinnedCharacterEquip( int iAttackType, int iWeaponID )
{
	bool bChange = false; // 回傳是否有改變

	char szNewWeaponName[8];
	sprintf( szNewWeaponName, "m_wea%d%d", ( iAttackType % 3 ) + 1, ( iWeaponID % 3 ) + 1 );

	if( m_szNoneCombinedSkinnedCharacterWeaponName == NULL )
	{
		m_szNoneCombinedSkinnedCharacterWeaponName= _NEW char[8];
		strcpy( m_szNoneCombinedSkinnedCharacterWeaponName, szNewWeaponName );
		bChange = true;
	}
	else
	{
		if( strcmp( szNewWeaponName, m_szNoneCombinedSkinnedCharacterWeaponName ) != 0 )
		{
			strcpy( m_szNoneCombinedSkinnedCharacterWeaponName, szNewWeaponName );
			bChange = true;
		}
	}

	return bChange;
}


//-----------------------------------------------------------------------------
// Name: setSkinnedCharacterEquip()
// Desc: jingjie modified 2006.07.20 set none-combied skinned character's adorns.
//-----------------------------------------------------------------------------
// 070626 cyhsieh 檢查NPC或MOB是否改變貼圖或裝備配件
bool cSkinnedCharacterModel::setSkinnedCharacterEquip( bool bAdorn01, bool bAdorn02, bool bAdorn03 )
{
	bool bChange = false; // 回傳是否有改變

	if( m_bNoneCombinedSkinnedCharacterAdorn01 != bAdorn01 )
	{
		m_bNoneCombinedSkinnedCharacterAdorn01 = bAdorn01;
		bChange = true;
	}
	if( m_bNoneCombinedSkinnedCharacterAdorn02 != bAdorn02 )
	{
		m_bNoneCombinedSkinnedCharacterAdorn02 = bAdorn02;
		bChange = true;
	}
	if( m_bNoneCombinedSkinnedCharacterAdorn03 != bAdorn03 )
	{
		m_bNoneCombinedSkinnedCharacterAdorn03 = bAdorn03;
		bChange = true;
	}

	return bChange;
}

//----------------------------------------------------------------------------
// Name: updateSkinnedCharacterEquips
// Desc: jingjie modified 2006.07.20 update skinned character's equips.
//----------------------------------------------------------------------------
void cSkinnedCharacterModel::updateSkinnedCharacterEquips()
{
	if (m_pSkeletonSerializer == NULL) return;

	if( m_pEquipPartList )
	{
		cSkeletonHierarchy* pSkeletonHierarchy = NULL;
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart )
			{
				pEquipPart->SetShadowData( m_pShadowData );

				pSkeletonHierarchy = m_pSkeletonSerializer->getWeaponSkeletonHierarchy( pEquipPart->GetLinkID() );
				if( pSkeletonHierarchy )
				{
					pEquipPart->setEquipSkeletonHierarchy( pSkeletonHierarchy );

#ifdef _CHECK_NOUSE_ANIMATION
					SkeletonHierarchyUsabilityMap::iterator iter = m_SkeletonHierarchyUnUsedMap.find( pSkeletonHierarchy );
					if( iter != m_SkeletonHierarchyUnUsedMap.end() )
					{
						m_SkeletonHierarchyUnUsedMap.erase( iter );
					}
#endif
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
// Name: AddEquipEffect
// Desc: 增加裝備特效
//----------------------------------------------------------------------------
// 070205 cyhsieh model effect
void cSkinnedCharacterModel::AddEquipEffect( BYTE byPartID, char* pName, BYTE byStatusType )
{
	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && ( pEquipPart->GetPartID() == byPartID ) )
			{
				pEquipPart->AddEquipModelEffect( pName, byStatusType );
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------
// Name: ForceAlpha
// Desc:
//----------------------------------------------------------------------------
void cSkinnedCharacterModel::ForceAlpha(float fAlpha)
{
	if (m_pRender)
	{
		m_pRender->ForceAlpha(fAlpha);
	}

	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && ( pEquipPart->IsModelExist() ) )
			{
				pEquipPart->ForceAlpha( fAlpha );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: GetLinkWorldTransform
// Desc: 
//-----------------------------------------------------------------------------
// 070425 cyhsieh 特效連結點(移除舊特效規格)
void cSkinnedCharacterModel::GetLinkWorldTransform( D3DXMATRIXA16& matTransform, BYTE byMagicLinkID, const D3DXMATRIXA16* pForceRotation )
{
	// !!!To do...Optimize
	if( m_pSkeletonSerializer == NULL )
	{
		return;
	}

	// 特效連結點
	if( byMagicLinkID == 0 )  // 0是腳底
	{
		if( pForceRotation )  // 有強制旋轉矩陣
		{
			matTransform = m_matScaling * (*pForceRotation ) * m_matTranslation;
		}
		else
		{
			matTransform = m_matMatrix;
		}
		return;
	}
	else
	{
		cSkeletonHierarchy *pSkeletonHierarchy = m_pSkeletonSerializer->getMagicSkeletonHierarchy( byMagicLinkID - 1 );
		if( pSkeletonHierarchy )
		{
			if( pForceRotation )  // 有強制旋轉矩陣
			{
				matTransform = *pSkeletonHierarchy->getSkeletonHierarchyMatrix() * m_matScaling * (*pForceRotation) * m_matTranslation;
			}
			else
			{
				matTransform = *pSkeletonHierarchy->getSkeletonHierarchyMatrix() * m_matMatrix;
			}
			return;
		}
	}

	D3DXMatrixIdentity( &matTransform );
}

//-----------------------------------------------------------------------------
// Name: GetCenterWorldTransform
// Desc: 
//-----------------------------------------------------------------------------
// 070430 cyhsieh 非玩家Model特效連結點
void cSkinnedCharacterModel::GetCenterWorldTransform( D3DXMATRIXA16& matTransform, const D3DXMATRIXA16* pForceRotation )
{
	if( m_pSkeletonSerializer == NULL )
	{
		return;
	}

	D3DXVECTOR3 vCenter = ( *m_pLocalSpaceBoundingBoxMin + *m_pLocalSpaceBoundingBoxMax ) * 0.5f;

	// 071024 cyhsieh Mob特效縮放
	float fEffectScale = m_pSkeletonSerializer->GetEffectScale();
	if( fEffectScale == 1.0f )
	{
		// 不需要縮放
		D3DXMATRIXA16 matTranslation;
		D3DXMatrixTranslation( &matTranslation, vCenter.x, vCenter.y, vCenter.z );
		if( pForceRotation )  // 有強制旋轉矩陣
		{
			matTransform = matTranslation * m_matScaling * (*pForceRotation) * m_matTranslation;
		}
		else
		{
			matTransform = matTranslation * m_matMatrix;
		}
	}
	else
	{
		// 需要縮放
		D3DXMATRIXA16 matScaleTranslation;
		D3DXMatrixScaling( &matScaleTranslation, fEffectScale, fEffectScale, fEffectScale );
		matScaleTranslation._41 = vCenter.x;
		matScaleTranslation._42 = vCenter.y;
		matScaleTranslation._43 = vCenter.z;
		if( pForceRotation )  // 有強制旋轉矩陣
		{
			matTransform = matScaleTranslation * m_matScaling * (*pForceRotation) * m_matTranslation;
		}
		else
		{
			matTransform = matScaleTranslation * m_matMatrix;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: ForceComputeSkinnedCharacterMesh
// Desc: 強制播某動畫(給照片用)
//-----------------------------------------------------------------------------
// 070516 cyhsieh 裝備介面人物照
void cSkinnedCharacterModel::ForceComputeSkinnedCharacterMesh( int iActionIndex, float fAnimTime )
{
	if( m_byWaitResourceCount > 0 )
	{
		return;
	}

	if( !m_pSkeletonSerializer || !m_pSkeletonSerializer->getAnimationRoot() )
	{
		return;
	}
	if( m_pRender == NULL )
	{
		return;
	}

	DWORD dwVertexSize = m_pRender->GetCurrentTotalVertexSize();
	MDLVTX* pRenderMesh = m_pRender->GetCurrentRenderMesh();
	if( dwVertexSize == 0 || pRenderMesh == NULL )
	{
		return;
	}

	sComputeAnimParam param( iActionIndex, -1, fAnimTime, 0.0f, 0.0f );

	// 計算動作
#ifdef _CHECK_NOUSE_ANIMATION
	m_pSkeletonSerializer->computeSkeletonHierarchyMatrix( param,
		NULL,
		&m_SkeletonHierarchyUnUsedMap );
#else
	m_pSkeletonSerializer->computeSkeletonHierarchyMatrix( param,
		NULL );
#endif

	for( unsigned int i = 0; i < dwVertexSize; i++ )
	{
		pRenderMesh[i].p.x = 0.0f;
		pRenderMesh[i].p.y = 0.0f;
		pRenderMesh[i].p.z = 0.0f;
		pRenderMesh[i].n.x = 0.0f;
		pRenderMesh[i].n.y = 0.0f;
		pRenderMesh[i].n.z = 0.0f;
	}


	cMeshSerializer* pMeshSerial;
	cSubMeshSerializer* pSubMeshSerial;
	DWORD	dwObjMeshSize;
	cAnimation* pAnimation;
	D3DXMATRIXA16 matCombined;
	cSkinWeights* pSkinWeight;
	int iStartIndex = 0;
	DWORD dwSkinIndex;
	D3DXVECTOR3 vPos, vNormal;
	cSkeletonHierarchy* pSkeletonHierarchy;

	DWORD dwLodLevel = m_pRender->GetLodLevel();

	for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
	{
		// 070816 cyhsieh Skinned Character LOD
		pMeshSerial = m_pPart[iPartIdx].m_pMesh[dwLodLevel];
		if( pMeshSerial == NULL )
		{
			continue;
		}

		dwObjMeshSize = pMeshSerial->getSubMeshSerializersCount();
		for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
		{
			pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );

			MDLVTX* pVertexBuffer = pSubMeshSerial->getSubMeshVerticesBuffer();
			DWORD dwVertexBufferSize = pSubMeshSerial->getSubMeshVerticesBufferSize();
			DWORD dwSkinWeightsSize = pSubMeshSerial->getSubMeshSkinWeightsSize();
			if( dwSkinWeightsSize > 0 )
			{
				for( DWORD dwIndex = 0; dwIndex < dwSkinWeightsSize; dwIndex++ )
				{
					pSkinWeight = pSubMeshSerial->getSubMeshSkinWeights( dwIndex );
					pSkeletonHierarchy = m_pSkeletonSerializer->findSkeletonHierarchy( pSkinWeight->getSkinWeightsSkeletonName() );
					if( pSkeletonHierarchy )
					{
						pAnimation = pSkeletonHierarchy->getAnimation();
						if( pAnimation == NULL )
						{
							continue;
						}
					}
					else
					{
						continue;
					}
					matCombined = *(pSkinWeight->getSkinWeightsSkeletonMatrix()) * (*pSkeletonHierarchy->getSkeletonHierarchyMatrix());

					for( unsigned int i = 0; i < pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesSize(); i++ )
					{
						dwSkinIndex = *pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesIndex(i);

						if( pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesWeight() )
						{
							D3DXVec3TransformCoord( &vPos,
								&pVertexBuffer[dwSkinIndex].p,
								&matCombined );
							pRenderMesh[dwSkinIndex + iStartIndex].p += ( vPos * (*pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesWeight(i) ));

							D3DXVec3TransformNormal( &vNormal,
								&pVertexBuffer[dwSkinIndex].n,
								&matCombined );
							pRenderMesh[dwSkinIndex + iStartIndex].n += ( vNormal * (*pSkinWeight->getSkinWeightsSkeletonInfluencedMeshVerticesWeight(i)) );
						}
						else
						{
							D3DXVec3TransformCoord( &pRenderMesh[dwSkinIndex + iStartIndex].p,
								&pVertexBuffer[dwSkinIndex].p,
								&matCombined );

							D3DXVec3TransformNormal( &pRenderMesh[dwSkinIndex + iStartIndex].n,
								&pVertexBuffer[dwSkinIndex].n,
								&matCombined );
						}
					}
				}
			}
			else
			{
				pSkeletonHierarchy = m_pSkeletonSerializer->findSkeletonHierarchy( pSubMeshSerial->getSubMeshName() );
				if( pSkeletonHierarchy == NULL )
				{
					continue;
				}

				for( DWORD dwIndex = 0; dwIndex < dwVertexBufferSize; dwIndex++ )
				{
					D3DXVec3TransformCoord( &pRenderMesh[dwIndex + iStartIndex].p,
						&pVertexBuffer[dwIndex].p,
						pSkeletonHierarchy->getSkeletonHierarchyMatrix() );

					D3DXVec3TransformNormal( &pRenderMesh[dwIndex + iStartIndex].n,
						&pVertexBuffer[dwIndex].n,
						pSkeletonHierarchy->getSkeletonHierarchyMatrix() );
				}
			}

			iStartIndex += dwVertexBufferSize;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: ForceDraw
// Desc: 強制畫(給照片用)
//-----------------------------------------------------------------------------
// 070516 cyhsieh 裝備介面人物照
void cSkinnedCharacterModel::ForceDraw( LPDIRECT3DDEVICE9 pD3DDDevice, D3DXMATRIXA16* pTransform )
{
	if( m_pRender && pD3DDDevice )
	{
		if( pTransform )
		{
			m_pRender->SetWorldMatrix( *pTransform );
		}

		// 記錄一些會影響Render的資訊
		bool bOriUseFocusLight = m_pRender->m_bUseFocusLight;
		bool bOriLightSystem = m_pRender->bLightSystem;
		bool bOriAlphaBlend = m_pRender->m_bAlphaBlendenable;
		float fOriAlpha = m_pRender->GetAlpha();
		BYTE byOriChangeColorType = m_pRender->GetChangeColorType();
		float fOriHurtLightFactor = m_pRender->GetHurtLightFactor();

		// 設成一般的Render
		m_pRender->m_bUseFocusLight = false;      // 不發光
		m_pRender->SetShadowData( NULL );         // 不畫陰影
		m_pRender->bLightSystem = false;          // 不打光
		m_pRender->m_bAlphaBlendenable = false;   // 不要半透明
		m_pRender->ForceAlpha( 0.0f );            // 不要半透明
		m_pRender->SetChangeColorType( 0 );       // 不要變色
		m_pRender->SetHurtLightFactor( 255.0f );  // 不要受擊紅光
		if( fOriHurtLightFactor < 255.0f )
		{
			m_pRender->m_dwColorArg2 = D3DTA_DIFFUSE;
		}

		// 畫
		m_pRender->Draw( pD3DDDevice );

		// 設回來原本的
		m_pRender->m_bUseFocusLight = bOriUseFocusLight;
		m_pRender->SetShadowData( m_pShadowData );
		m_pRender->bLightSystem = bOriLightSystem;
		m_pRender->m_bAlphaBlendenable = bOriAlphaBlend;
		m_pRender->ForceAlpha( fOriAlpha );
		m_pRender->SetChangeColorType( byOriChangeColorType );
		m_pRender->SetHurtLightFactor( fOriHurtLightFactor );

		if( m_pEquipPartList )
		{
			cSkinnedCharacterEquipPart* pEquipPart;
			int iCount = m_pEquipPartList->GetCount();
			for( int i = 0; i < iCount; i++ )
			{
				pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
				if( pEquipPart && ( pEquipPart->IsModelExist() ) )
				{
					// 071128 cyhsieh 釣竿
					if( pEquipPart->GetPartID() == IPaperDoll::eptFishingRod )
					{
						continue; // 釣竿不畫
					}

					pEquipPart->ForceDraw( pD3DDDevice, pTransform );
					if( m_pShadowData )
					{
						pEquipPart->SetShadowData( m_pShadowData );
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: SetFakeReflect
// Desc: 
//-----------------------------------------------------------------------------
// 070528 cyhsieh 反光貼圖開關
void cSkinnedCharacterModel::SetFakeReflect( bool bShow )
{
	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && ( pEquipPart->IsModelExist() ) )
			{
				pEquipPart->SetFakeReflect( bShow );
			}
		}
	}

	if( m_pPart && m_pTextureDB && m_pRender )
	{
		if( bShow )
		{
			char szFileName[MAX_PATH];
			char szTexturePath[]="Mesh\\Being\\Tex\\";
			cMeshSerializer* pMeshSerial;
			cSubMeshSerializer* pSubMeshSerial;
			DWORD	dwObjMeshSize;
			int iMaterialIndex = 0;
			sModelPartData* pModelPartData;
			bool bPartHasFakeReflect = false;

			for( int iPartIdx = 0; iPartIdx < IPaperDoll::mptCOUNT; iPartIdx++ )
			{
				pMeshSerial = m_pPart[iPartIdx].m_pMesh[0];
				if( pMeshSerial == NULL )
				{
					continue;
				}
				pModelPartData = m_pRender->GetModelPartData( iPartIdx );
				if( pModelPartData == NULL || ( pModelPartData->m_pFakeReflectMat != NULL ) )
				{
					continue;
				}

				dwObjMeshSize = pMeshSerial->getSubMeshSerializersCount();

				bool bHasFakeReflect = true;
				pModelPartData->m_pFakeReflectMat = _NEW cMaterial[pModelPartData->m_iMaterialSize];

				iMaterialIndex = 0;

				for( DWORD dwObjIndex = 0; dwObjIndex < dwObjMeshSize; dwObjIndex++ )
				{
					pSubMeshSerial = pMeshSerial->getSubMeshSerializer( dwObjIndex );
					DWORD dwMeshMaterialSize = pSubMeshSerial->getSubMeshMaterialSize();
					for( DWORD dwIndex = 0; dwIndex < dwMeshMaterialSize; dwIndex++ )
					{
						cMaterial* pMaterial = pSubMeshSerial->getSubMeshMaterial( dwIndex );
						if( pMaterial == NULL )
						{
							continue;
						}
						if( pMaterial->m_iNameSize && pMaterial->m_pszTextName )
						{
							cTexture* pTexture = pModelPartData->m_pMaterial[iMaterialIndex].m_pTexture;
							if( pModelPartData->m_pFakeReflectMat && pTexture && pTexture->GetLoadFakeReflectTex() )
							{
								sprintf( szFileName, "%s%02d_s", pMaterial->m_pszTextName, m_pPart[iPartIdx].m_iTextureId );
								pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture = m_pTextureDB->QueryTexture(szFileName,szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
								if( pModelPartData->m_pFakeReflectMat[iMaterialIndex].m_pTexture )
								{
									bHasFakeReflect = true;
									bPartHasFakeReflect = true;
								}
								else
								{
									// 讀不到反光貼圖，以後不要再嘗試load
									pTexture->SetLoadFakeReflectTex( false );
								}
							}
						}
						iMaterialIndex++;
					}
				}
			}

			if( bPartHasFakeReflect )
			{
				cTexture* pFakeReflectTexture = m_pTextureDB->QueryTexture("FakeReflect",szTexturePath,MESH_STUFF_PATH,sTextureQueryParam( false, 0 ));
				m_pRender->SetFakeReflectTexture( pFakeReflectTexture );
			}
		}
		else
		{
			m_pRender->ReleaseFakeReflectResource();
		}
	}
}

//-----------------------------------------------------------------------------
// Name: AddToScene
// Desc: 
//-----------------------------------------------------------------------------
// 070531 cyhsieh 反光整理
void cSkinnedCharacterModel::AddToScene( bool bUseFocusLight, bool bCharQuit )
{
	// 080424 cyhsieh Thread
	if( m_byWaitResourceCount > 0 )
	{
		return;
	}

	if( g_mScene )
	{
		if( m_pRender )
		{
			// 中心點
			m_pRender->Center = ( *m_pWorldSpaceBoundingBoxMin + *m_pWorldSpaceBoundingBoxMax ) * 0.5f;
			m_pRender->m_bUseFocusLight = bUseFocusLight;

			// [George] 070809 要在沒有charquit的狀況下才行
			if( !bCharQuit )
			{
				// [George] 070808 being和camera碰撞時半透明顯示
				const D3DXVECTOR3& vCameraPos = g_Camera.GetEyePt();
				// 如果camera在being的BB裡面的話,就讓being半透明
				if( vCameraPos.x >= m_pWorldSpaceBoundingBoxMin->x && vCameraPos.x <= m_pWorldSpaceBoundingBoxMax->x &&
					vCameraPos.y >= m_pWorldSpaceBoundingBoxMin->y && vCameraPos.y <= m_pWorldSpaceBoundingBoxMax->y &&
					vCameraPos.z >= m_pWorldSpaceBoundingBoxMin->z && vCameraPos.z <= m_pWorldSpaceBoundingBoxMax->z )
				{
					m_pRender->SetFadeTypeEasy(200.0f);
				}
				else
				{
					m_pRender->SetFadeTypeEasy(0.0f);
				}
			}
			g_mScene->AddRender( m_pRender );
		}

		if( m_pEquipPartList )
		{
			cSkinnedCharacterEquipPart* pEquipPart;
			int iCount = m_pEquipPartList->GetCount();
			for( int i = 0; i < iCount; i++ )
			{
				pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
				if( pEquipPart && ( pEquipPart->getEquipVisible() ) )
				{
					pEquipPart->AddToScene( bUseFocusLight );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: SetFocusLight
// Desc: 
//-----------------------------------------------------------------------------
// 070531 cyhsieh 反光整理
void cSkinnedCharacterModel::SetFocusLight( void )
{
	if( m_pRender )
	{
		m_pRender->m_bUseFocusLight = true;
	}

	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && ( pEquipPart->IsModelExist() ) )
			{
				pEquipPart->SetFocusLight();
			}
		}
	}
}

//---------------------------------------------------------------------------
// Name: HideWeaponEquip
// Desc: 隱藏武器
//---------------------------------------------------------------------------
// 070731 cyhsieh 坐下不顯示武器
void cSkinnedCharacterModel::HideWeaponEquip( bool bHide )
{
	if( m_bHideWeaponEquip == bHide )
	{
		return;
	}
	m_bHideWeaponEquip = bHide;

	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart )
			{
				switch( pEquipPart->GetPartID() )
				{
				case IPaperDoll::eptWeaponL:        // 各式武器
				case IPaperDoll::eptWeaponR:        // 各式武器
				case IPaperDoll::eptForearmL:       // 腕盾,拳刃
				case IPaperDoll::eptForearmR:       // 腕盾,拳刃
				case IPaperDoll::eptHandbackL:      // 塔盾,持盾,巨盾
				case IPaperDoll::eptHandbackR:      // 塔盾,持盾,巨盾
					pEquipPart->SetForceHide( bHide );
					break;
				}
			}
		}
	}
}


//---------------------------------------------------------------------------
// [George] 070807 雙騎乘時前座不顯示披風
//---------------------------------------------------------------------------
void cSkinnedCharacterModel::SetCapeVisibility( bool bStatus )
{
	if( m_pRender == NULL ) return;
	sModelPartData* pModelPartData;
	pModelPartData = m_pRender->GetModelPartData( IPaperDoll::mptCloak );
	if( pModelPartData && pModelPartData->m_pShow )
	{
		pModelPartData->m_pShow[ 0 ] = bStatus;
	}
}

//---------------------------------------------------------------------------
// Name: ComputeLod
// Desc: 計算目前的Lod Level
//---------------------------------------------------------------------------
// 070816 cyhsieh Skinned Character LOD
void cSkinnedCharacterModel::ComputeLod( void )
{
	if( m_pRender )
	{
		D3DXVECTOR3 vPos( m_matTranslation._41, m_matTranslation._42, m_matTranslation._43 );
		const D3DXVECTOR3& vCameraPos = g_Camera.GetEyePt();
		D3DXVECTOR3 vDiff = vPos - vCameraPos;
		float fDistToCameraSq = D3DXVec3LengthSq( &vDiff );

		DWORD dwLowestLodLevel = m_pRender->GetLowestLodLevel();
		DWORD dwLodLevel;
		for( dwLodLevel = dwLowestLodLevel; dwLodLevel >= 0; dwLodLevel-- )
		{
			if( fDistToCameraSq >= m_fLodDistanceSq[dwLodLevel] )
			{
				break;
			}
		}
		m_pRender->SetLodLevel( dwLodLevel );
	}
}

//---------------------------------------------------------------------------
// Name: SetEquipActionIndex
// Desc: 設定動畫
//---------------------------------------------------------------------------
// 071128 cyhsieh 釣竿
void cSkinnedCharacterModel::SetEquipActionIndex( BYTE byPartID, int iActionIndex )
{
	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && ( pEquipPart->GetPartID() == byPartID ) )
			{
				pEquipPart->SetActionIndex( iActionIndex );
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
// Name: GetAnimationActionInfo
// Desc: 取得動作資訊
//---------------------------------------------------------------------------
cAnimationActionInfo* cSkinnedCharacterModel::GetAnimationActionInfo( int iActionIndex )
{
	if( m_pSkeletonSerializer )
	{
		cAnimationActionInfoGroup* pAnimationActionInfoGroup = m_pSkeletonSerializer->getAnimationActionInfoGroup();
		if( pAnimationActionInfoGroup )
		{
			return pAnimationActionInfoGroup->getAnimationActionInfo( iActionIndex );
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
// Name: GetSkeletonMountCameraAdjust
// Desc: 取得騎乘攝影機調整
//---------------------------------------------------------------------------
sMountCameraAdjust* cSkinnedCharacterModel::GetSkeletonMountCameraAdjust( void )
{
	if( m_pSkeletonSerializer )
	{
		return m_pSkeletonSerializer->GetMountCameraAdjust();
	}

	return NULL;
}

//---------------------------------------------------------------------------
// Name: ResetLocalBoundingBox
// Desc: 重置Local Bounding Box
//---------------------------------------------------------------------------
void cSkinnedCharacterModel::ResetLocalBoundingBox( void )
{
	if( m_pLocalSpaceBoundingBoxMin && m_pLocalSpaceBoundingBoxMax )
	{
		*m_pLocalSpaceBoundingBoxMin = D3DXVECTOR3( -50.0f, -50.0f, -50.0f );
		*m_pLocalSpaceBoundingBoxMax = D3DXVECTOR3(  50.0f,  50.0f,  50.0f );
	}
}


//---------------------------------------------------------------------------
// Name: GetResourceReady
// Desc: 模型相關資源是否讀取完畢
//---------------------------------------------------------------------------
bool cSkinnedCharacterModel::GetResourceReady( void )
{
	if( m_byWaitResourceCount > 0 )
	{
		return false;
	}

	if( m_pEquipPartList )
	{
		cSkinnedCharacterEquipPart* pEquipPart;
		int iCount = m_pEquipPartList->GetCount();
		for( int i = 0; i < iCount; i++ )
		{
			pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
			if( pEquipPart && ( pEquipPart->GetResourceReady() == false ) )
			{
				return false;
			}
		}
	}

	return true;
}

//---------------------------------------------------------------------------
// Name: ResourceFinishLoad
// Desc: 模型相關資源讀取結束
//---------------------------------------------------------------------------
bool cSkinnedCharacterModel::ResourceFinishLoad( BYTE byEquipPartID )
{
	bool bNeedRecompute = false;

	if( byEquipPartID == 0 )
	{
		if( m_byWaitResourceCount > 0 )
		{
			m_byWaitResourceCount--;

			if( m_byWaitResourceCount == 0 )
			{
				if( m_pRender && ( m_pRender->GetTotalIndexSize( 0 ) == 0 ) )
				{
					bNeedRecompute = true;  // 點資料尚未建立好，需要ReCombineSkinnedCharacterPartsMesh
				}
			}
		}
	}
	else
	{
		if( m_pEquipPartList )
		{
			cSkinnedCharacterEquipPart* pEquipPart;
			int iCount = m_pEquipPartList->GetCount();
			for( int i = 0; i < iCount; i++ )
			{
				pEquipPart = (cSkinnedCharacterEquipPart*)m_pEquipPartList->GetItem( i );
				if( pEquipPart && ( pEquipPart->GetPartID() == ( byEquipPartID - 1 ) ) )
				{
					pEquipPart->ResourceFinishLoad();
					break;
				}
			}
		}
	}

	return bNeedRecompute;
}