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
// Desc: cSkeletonSerializerFactory �⹹Ԫ
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

	//   Mob��Ч����
	m_pSkeletonEffectScaleMap.Clear();
	LoadEffectScaleTable();

	// �����Ӱ������
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

	// �����Ӱ������
	Array<sMountCameraAdjust*> mounts = m_pSkeletonCameraAdjustMap.GetContents();
	for( int i = 0; i < mounts.Size(); i++ )
	{
		n_delete( mounts[i] );
	}
	mounts.Clear();
	m_pSkeletonCameraAdjustMap.Clear();


	// Mob��Ч����
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
// Desc: Ѱ��id
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
		// ��ȡ�������
		if (iNameSize > 0)
		{
			pSkeletonSerializer = n_new(cSkeletonSerializer);

			szFileName = n_new_array(char, iNameSize+1);
			strcpy(szFileName,szSkeletonSerializerName);

			if (pSkeletonSerializer->importSkeleton(szFileName, szPath) < 0)
			{
				// ��ȡ��ܴ��� 
				char szErrorMessage[MAX_PATH];
				sprintf(szErrorMessage,"��ȡ�Ǽܵ� %s%s ����", szPath,szFileName); 

				n_delete(pSkeletonSerializer);
				n_delete_array(szFileName);
				return pSkeletonSerializer;
			}
			
			//--------------------------------------------
			// ��ȡ�����嵥
			// ����ͬ�� Model ����, ���ֳ� ActionTable �Ĵ���ʽ
			if (pSkeletonSerializer->importActionTable(iModelType, szFileName, szPath) < 0)
			{
				// ��ȡ�����嵥����
				char szErrorMessage[MAX_PATH];
				sprintf(szErrorMessage,"��ȡ�����嵥 %s%s ����", szPath,szFileName); 

				n_delete(pSkeletonSerializer);
				n_delete_array(szFileName);
				return pSkeletonSerializer;
			}
			//  ��������Ƭ, ��ЧƬ, ���	��������Ѷ�б�
			pSkeletonSerializer->constructLinkerSkeletonHierarchyTables();

			//--------------------------------------------
			// ��ȡ�����¼���
			// sprintf(szFilePath,"%s.aef\0",szFileName);

			// ����ʹ�ö����¼�������...
			//      if (pFrameWork->LoadAefFile(szFileName,szPath)< 0)
			//      {
			//        // ��ȡ�����¼�����
			//      }
			//-------------------------------------------- 
			n_delete_array(szFileName);
			m_pSkeletonSerializerMap.Add(szSkeletonSerializerName, pSkeletonSerializer);

			// Mob��Ч����
			if( !m_pSkeletonEffectScaleMap.IsEmpty() )
			{
				DWORD pTemp;
				if (m_pSkeletonEffectScaleMap.Find(szSkeletonSerializerName, pTemp))
				{
					// �趨�˹Ǽܵ���Ч����ֵ
					float fEffectScale = (int)pTemp / 100.0f;
					pSkeletonSerializer->SetEffectScale( fEffectScale );
				}
			}

			// �����Ӱ������
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
// Desc: ��ȡ��Ч���ű�
//-----------------------------------------------------------------------------
//   Mob��Ч����
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
		case cECReader::ttINT:     // ����
			iScale = ecReader.GetTokenInt();
			m_pSkeletonEffectScaleMap.Add( szSkeletonName, iScale );
			break;
		case cECReader::ttCOMMENT: // ע��
			break;
		case cECReader::ttCOMMA:   // ����
			break;
		default:
			bLoop = false;
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: LoadMountCameraAdjustTable
// Desc: ��ȡ�������Ӱ��������
//-----------------------------------------------------------------------------
// 080123 cyhsieh �����Ӱ������  
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

			if( ecReader.GetToken() != cECReader::ttCOMMA ) return; // ����

			iType = ecReader.GetToken();
			if( ( iType != cECReader::ttFLOAT ) && ( iType != cECReader::ttINT ) )  
				return;
			fAdjustY = (float)ecReader.GetTokenDouble();            // �ӵ�߶ȵ���

			if( ecReader.GetToken() != cECReader::ttCOMMA ) return; // ����

			iType = ecReader.GetToken();
			if( ( iType != cECReader::ttFLOAT ) && ( iType != cECReader::ttINT ) )  
				return;
			fZoomDistance = (float)ecReader.GetTokenDouble();       // ϵͳ��׷Zoom����

				pMountCameraAdjust = n_new(sMountCameraAdjust( fAdjustY, fZoomDistance ));
			if( pMountCameraAdjust )
			{
				m_pSkeletonCameraAdjustMap.Add( szSkeletonName, pMountCameraAdjust);
			}
			break;

		case cECReader::ttCOMMENT: // ע��
			break;

		default:
			bLoop = false;
			break;
		}
	}
}

}