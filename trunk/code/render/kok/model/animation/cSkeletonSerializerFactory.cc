//----------------------------------------------------------------------------
//  cSkeletonSerializerFactory.cc
//  (C) 2008 cTuo
//----------------------------------------------------------------------------
#include "stdneb.h"
#include "cSkeletonSerializerfactory.h"

namespace KOK
{
ImplementClass(KOK::cSkeletonSerializerFactory, 'SKFY', Core::RefCounted);
ImplementSingleton(KOK::cSkeletonSerializerFactory);

using namespace Util;

ID3DXMatrixStack *g_pStack = NULL;

cSkeletonSerializerFactory::cSkeletonSerializerFactory()
{
	ConstructSingleton;
}

//-----------------------------------------------------------------------------
// Name: ~cSkeletonSerializerFactory()
// Desc: cSkeletonSerializerFactory 解构元
//-----------------------------------------------------------------------------
cSkeletonSerializerFactory::~cSkeletonSerializerFactory()
{
	if (this->IsOpen())
	{
		this->Close();
	}
	DestructSingleton;
}

/// open the input server
void 
cSkeletonSerializerFactory::Open()
{
	D3DXCreateMatrixStack(0, &g_pStack);

	m_pSkeletonSerializerMap.Clear();

	//   Mob特效缩放
	m_pSkeletonEffectScaleMap.Clear();
	LoadEffectScaleTable();

	// 骑乘摄影机调整
	m_pSkeletonCameraAdjustMap.Clear();
	LoadMountCameraAdjustTable();

	this->isOpen = true;
}

/// close the input server
void 
cSkeletonSerializerFactory::Close()
{
	this->isOpen = false;

	if (g_pStack != NULL)
	{
		g_pStack->Release();
		g_pStack = 0;
	}

	// 骑乘摄影机调整
	Array<sMountCameraAdjust*> mounts = m_pSkeletonCameraAdjustMap.GetContents();
	for( int i = 0; i < mounts.Size(); i++ )
	{
		n_delete( mounts[i] );
	}
	mounts.Clear();
	m_pSkeletonCameraAdjustMap.Clear();


	// Mob特效缩放
	m_pSkeletonEffectScaleMap.Clear();

	cSkeletonSerializer *pSkeletonSerializer = 0;
	Array<cSkeletonSerializer*> skeletons = m_pSkeletonSerializerMap.GetContents();
	for ( int i = 0 ; i < skeletons.Size(); i ++ )
	{
		n_delete(skeletons[i]);
	}
	skeletons.Clear();
	m_pSkeletonSerializerMap.Clear();
}

//-----------------------------------------------------------------------------
// Name: createSkeletonSerializer()
// Desc: 寻找id
//-----------------------------------------------------------------------------
cSkeletonSerializer* cSkeletonSerializerFactory::createSkeletonSerializer(int iModelType, 
																		  const char *szSkeletonSerializerName,
																		  const char *szPath)
{
	cSkeletonSerializer *pSkeletonSerializer = NULL;

	int iNameSize =lstrlen(szSkeletonSerializerName);
	if(!iNameSize) return 0;

	bool bfind = m_pSkeletonSerializerMap.Find(szSkeletonSerializerName, pSkeletonSerializer);

	if (pSkeletonSerializer == 0)
	{
		char *szFileName = 0;
		//--------------------------------------------
		// 读取基本框架
		if (iNameSize > 0)
		{
			pSkeletonSerializer = n_new(cSkeletonSerializer);

			szFileName = n_new_array(char, iNameSize+1);
			strcpy(szFileName,szSkeletonSerializerName);

			if (pSkeletonSerializer->importSkeleton(szFileName, szPath) < 0)
			{
				// 读取框架错误 
				char szErrorMessage[MAX_PATH];
				sprintf(szErrorMessage,"读取骨架档 %s%s 错误", szPath,szFileName); 

				n_delete(pSkeletonSerializer);
				n_delete_array(szFileName);
				return pSkeletonSerializer;
			}
			
			//--------------------------------------------
			// 读取动作清单
			// 依不同的 Model 类型, 区分出 ActionTable 的处理方式
			if (pSkeletonSerializer->importActionTable(iModelType, szFileName, szPath) < 0)
			{
				// 读取动作清单错误
				char szErrorMessage[MAX_PATH];
				sprintf(szErrorMessage,"读取动作清单 %s%s 错误", szPath,szFileName); 

				n_delete(pSkeletonSerializer);
				n_delete_array(szFileName);
				return pSkeletonSerializer;
			}
			//  建立武器片, 特效片, 骑乘	连结点等资讯列表
			pSkeletonSerializer->constructLinkerSkeletonHierarchyTables();

			//--------------------------------------------
			// 读取动作事件表
			// sprintf(szFilePath,"%s.aef\0",szFileName);

			// 不再使用动作事件触发表...
			//      if (pFrameWork->LoadAefFile(szFileName,szPath)< 0)
			//      {
			//        // 读取动作事件错误
			//      }
			//-------------------------------------------- 
			n_delete_array(szFileName);
			m_pSkeletonSerializerMap.Add(szSkeletonSerializerName, pSkeletonSerializer);

			// Mob特效缩放
			if( !m_pSkeletonEffectScaleMap.IsEmpty() )
			{
				DWORD pTemp;
				if (m_pSkeletonEffectScaleMap.Find(szSkeletonSerializerName, pTemp))
				{
					// 设定此骨架的特效缩放值
					float fEffectScale = (int)pTemp / 100.0f;
					pSkeletonSerializer->SetEffectScale( fEffectScale );
				}
			}

			// 骑乘摄影机调整
			if( !m_pSkeletonCameraAdjustMap.IsEmpty() )
			{
				sMountCameraAdjust* pMountCameraAdjust;
				if (m_pSkeletonCameraAdjustMap.Find(szSkeletonSerializerName, pMountCameraAdjust))
				{
					pSkeletonSerializer->SetMountCameraAdjust( pMountCameraAdjust );
				}
			}
		}
	}

	return pSkeletonSerializer;
}

//-----------------------------------------------------------------------------
// Name: removeSkeletonSerializer()
// Desc:
//-----------------------------------------------------------------------------
void cSkeletonSerializerFactory::removeSkeletonSerializer( const char *szSkeletonSerializerName)
{
	if( !m_pSkeletonSerializerMap.IsEmpty() )
	{
		cSkeletonSerializer* pSkeletonSerializer;
		if (m_pSkeletonSerializerMap.Find(szSkeletonSerializerName, pSkeletonSerializer))
		{
			n_delete(pSkeletonSerializer);
			m_pSkeletonSerializerMap.Erase(szSkeletonSerializerName);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: removeAnimationKeysByReserveTime()
// Desc:
//-----------------------------------------------------------------------------
void cSkeletonSerializerFactory::removeAnimationKeysByReserveTime(float m_fTimes)
{
	if( !m_pSkeletonSerializerMap.IsEmpty() )
	{
		int i;
		Array<cSkeletonSerializer*> skeletons = m_pSkeletonSerializerMap.GetContents();
		for (i = 0; i < skeletons.Size(); i++)
		{
			if (skeletons[i])
				skeletons[i]->removeAnimationKeysByReserveTime(m_fTimes);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: removeAllSkeletonSerializer()
// Desc:
//-----------------------------------------------------------------------------
void cSkeletonSerializerFactory::removeAllSkeletonSerializer()
{
	if( !m_pSkeletonSerializerMap.IsEmpty() )
	{
		cSkeletonSerializer *pSkeletonSerializer = 0;

		Array<cSkeletonSerializer*> skeletons = m_pSkeletonSerializerMap.GetContents();
		for ( int i = 0 ; i < skeletons.Size() ; i ++ )
		{
			n_delete(skeletons[i]);
		}
		skeletons.Clear();

		m_pSkeletonSerializerMap.Clear();
	}
}

//-----------------------------------------------------------------------------
// Name: getSkeletonSerializerSize()
// Desc:   get SkeletonSerializers Size from SkeletonSerializer Factory
//-----------------------------------------------------------------------------
int cSkeletonSerializerFactory::getSkeletonSerializersSize()
{
	return (int)m_pSkeletonSerializerMap.Size();
}

//-----------------------------------------------------------------------------
// Name: LoadEffectScaleTable
// Desc: 读取特效缩放表
//-----------------------------------------------------------------------------
//   Mob特效缩放
void cSkeletonSerializerFactory::LoadEffectScaleTable( void )
{
	String szTableName = "config\\SkeletonEffectScale.txt";
	cECReader ecReader;
	if (!ecReader.LoadFromFile((char*)szTableName.AsCharPtr()))
		return;

	ecReader.Reset();

	char szSkeletonName[MAX_PATH];
	int iScale = 1;

	bool bLoop = true;
	for( ; bLoop ; )
	{
		switch( ecReader.GetToken() )
		{
		case cECReader::ttIDENTIFIER: // 
			strcpy( szSkeletonName, ecReader.GetTokenStr() );
			break;
		case cECReader::ttINT:     // 整数
			iScale = ecReader.GetTokenInt();
			m_pSkeletonEffectScaleMap.Add( szSkeletonName, iScale );
			break;
		case cECReader::ttCOMMENT: // 注解
			break;
		case cECReader::ttCOMMA:   // 逗号
			break;
		default:
			bLoop = false;
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: LoadMountCameraAdjustTable
// Desc: 读取骑乘物摄影机调整表
//-----------------------------------------------------------------------------
// 080123 cyhsieh 骑乘摄影机调整  
void cSkeletonSerializerFactory::LoadMountCameraAdjustTable( void )
{
	String szTableName = "config\\SkeletonMountCamera.txt";
	cECReader ecReader;
	if (!ecReader.LoadFromFile((char*)szTableName.AsCharPtr()))
		return;
	ecReader.Reset();

	char szSkeletonName[MAX_PATH];
	sMountCameraAdjust* pMountCameraAdjust = NULL;

	int iType;
	float fAdjustY;
	float fZoomDistance;
	bool bLoop = true;
	for( ; bLoop ; )
	{
		switch( ecReader.GetToken() )
		{
		case cECReader::ttIDENTIFIER: // 
			strcpy( szSkeletonName, ecReader.GetTokenStr() );

			if( ecReader.GetToken() != cECReader::ttCOMMA ) return; // 逗点

			iType = ecReader.GetToken();
			if( ( iType != cECReader::ttFLOAT ) && ( iType != cECReader::ttINT ) )  
				return;
			fAdjustY = (float)ecReader.GetTokenDouble();            // 视点高度调整

			if( ecReader.GetToken() != cECReader::ttCOMMA ) return; // 逗点

			iType = ecReader.GetToken();
			if( ( iType != cECReader::ttFLOAT ) && ( iType != cECReader::ttINT ) )  
				return;
			fZoomDistance = (float)ecReader.GetTokenDouble();       // 系统背追Zoom调整

				pMountCameraAdjust = n_new(sMountCameraAdjust( fAdjustY, fZoomDistance ));
			if( pMountCameraAdjust )
			{
				m_pSkeletonCameraAdjustMap.Add( szSkeletonName, pMountCameraAdjust);
			}
			break;

		case cECReader::ttCOMMENT: // 注解
			break;

		default:
			bLoop = false;
			break;
		}
	}
}

}