//----------------------------------------------------------------------------
//  cskeletonserializer.cc
//  (C) 2008 cTuo
//----------------------------------------------------------------------------

#include "stdneb.h"
#include "cSkeletonSerializer.h"
#include "kok/model/modeldef.h"
#include "io/stream.h"
#include "io/ioserver.h"

namespace KOK
{

using namespace Util;
using namespace IO;

extern ID3DXMatrixStack *g_pStack;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Name: cSndTable
// Desc: contrustor
//---------------------------------------------------------------------------
cSndTable::cSndTable()
{
	m_pAnimationActionInfoGroup = NULL;
}

//---------------------------------------------------------------------------
// Name: LoadTable
// Desc:
//---------------------------------------------------------------------------
bool cSndTable::LoadTable(char* szFileName, cAnimationActionInfoGroup* 
						  pAnimationActionInfoGroup)
{
	if (pAnimationActionInfoGroup == NULL) return false;
	// �趨Ŀǰʹ�õ� pCurAnimationActionKeyframesRangeGroup
	m_pAnimationActionInfoGroup = pAnimationActionInfoGroup;

	if (!LoadFromFile(szFileName)) return false;
	// ����
	Reset(); 
	// ��ʼ����
	for (;;)
	{
		switch (GetToken())
		{
		case ttIDENTIFIER: // �ַ�?
			if (ParseCommand()) 
			{
				continue; 
			}
		case ttINT: // ����
			if (ParseSound())
			{
				continue;
			}
			continue;
		case ttCOMMENT: // ע��
			continue;
		} // end of if (type == ttIDNETIFIER)
		// �뿪��Ȧ
		break;
	} // end of for (;;

	return true;
}

//---------------------------------------------------------------------------
// Name: ParseCommand
// Desc:
//---------------------------------------------------------------------------
bool cSndTable::ParseCommand()
{
	String strCMD = GetTokenStr();
	// ���Сд
	strCMD.ToLower();

	if (strCMD == "clear") 
	{
		// ��� cAction �����е���Ч����... 
		m_pAnimationActionInfoGroup->clearSoundSetting();

		return true;
	} // end of if ("clear")

	// δ֪��ָ��
	return false;
}

//---------------------------------------------------------------------------
// Name: ParseSound
// Desc:
//---------------------------------------------------------------------------
bool cSndTable::ParseSound()
{
	// ȡ�ö�������
	int iIndex = GetTokenInt();
	int iDelayTimems = 0; 

	String StrName;

	int TokenType = GetToken();
	// 1 ("xxx",1)
	if (TokenType == ttPARENOPEN)
	{
		if (GetToken() != ttSTRING) return false;
		// ȡ����Ч����
		StrName = GetTokenStr();    

		TokenType  = GetToken();

		if (TokenType == ttCOMMA)
		{
			TokenType  = GetToken();
			// ȡ����Ч�ӳ�ʱ��
			if (TokenType!= ttINT) return false;
			iDelayTimems = GetTokenInt();

			TokenType  = GetToken();
		}

		if (TokenType != ttPARENCLOSE) return false; 
	}
	else
	{
		if (TokenType != ttSTRING) return false;
		// ȡ����Ч����
		StrName = GetTokenStr();
	}

	m_pAnimationActionInfoGroup->setSoundInfo(iIndex, (char*)StrName.AsCharPtr(),iDelayTimems);

	return true;
}

//-----------------------------------------------------------------------------
// Name: cEventTableReader
// Desc: 
//-----------------------------------------------------------------------------
cEventTableReader::cEventTableReader()
:m_pAnimationActionInfoGroup(NULL)
{
}

//-----------------------------------------------------------------------------
// Name: LoadTable
// Desc: 
//-----------------------------------------------------------------------------
bool cEventTableReader::LoadTable( char* pFileName, cAnimationActionInfoGroup* 
								  pAnimationActionInfoGroup )
{
	if( pAnimationActionInfoGroup == NULL ) return false;
	// �趨Ŀǰʹ�õ� pCurAnimationActionKeyframesRangeGroup
	m_pAnimationActionInfoGroup = pAnimationActionInfoGroup;

	if (!LoadFromFile(pFileName))
		return false;

	// ����
	Reset(); 

	// ��ʼ����
	for( ; ; )
	{
		switch( GetToken() )
		{
		case ttINT:     // ����
			if( ParseEvent() )
			{
				continue;
			}
			break;
		case ttCOMMENT: // ע��
			continue;
		}
		// �뿪��Ȧ
		break;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: ParseEvent
// Desc: 
//-----------------------------------------------------------------------------
bool cEventTableReader::ParseEvent()
{
	// ������� { {�¼�} {�¼�} }
	// ȡ�ö�������
	int iActionIndex = GetTokenInt();

	if( GetToken() != ttBRACEOPEN )
	{
		return false;
	}

	for( ; ; )
	{
		switch( GetToken() )
		{
		case ttBRACEOPEN:
			if( GetToken() != ttINT ) // �¼�ID
			{
				return false;
			}
			switch( GetTokenInt() )
			{
			case 1: // ����� {1,��Ѫʱ��}
				if( ParseHurtEvent( iActionIndex ) == false )
				{
					return false;
				}
				break;
			default:
				return false;
			}
			continue;
		case ttBRACECLOSE:
			BackupPtr(); // ����Ŀǰ�Ľ���λ��
			if( GetToken() == ttBRACECLOSE )
			{
				ReloadPtr();  // �ظ���ǰ�Ľ���λ��
				break;
			}
			ReloadPtr();  // �ظ���ǰ�Ľ���λ��
			continue;
		default:
			return false;
		}
		break;
	}

	if( GetToken() != ttBRACECLOSE )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: ParseHurtEvent
// Desc: 
//-----------------------------------------------------------------------------
bool cEventTableReader::ParseHurtEvent( int iActionIndex )
{
	if( GetToken() != ttCOMMA ) // ����
	{
		return false;
	}

	if( GetToken() != ttINT )   // ��Ѫʱ��
	{
		return false;
	}

	float fHurtTime = GetTokenInt() / 24.0f;  // 1��24��frame

	if( m_pAnimationActionInfoGroup )
	{
		m_pAnimationActionInfoGroup->AddHurtEvent( iActionIndex, fHurtTime );
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: cSkeletonSerializer()
// Desc: cSkeletonSerializer ����Ԫ
//-----------------------------------------------------------------------------
cSkeletonSerializer::cSkeletonSerializer()
{
	// 
	m_iVersion = 0;
	m_dwWeaponSize = 0;
	m_dwMagicSize = 0;

	//   Mob��Ч����
	m_fEffectScale = 1.0f;

	// 080123 cyhsieh �����Ӱ������
	m_pMountCameraAdjust = NULL;

	m_pAnimationRoot = 0;
	m_pSkeletonHierarchyRoot = 0;

	// 060606 cyhsieh Bug Fix:��������ʹ��ȴ��ɾ������
	m_pACTFileInfo                  = NULL;
	m_pWeaponSkeletonHierarchyTable = NULL;
	m_pMagicSkeletonHierarchyTable  = NULL;
	m_pMountLinkSkeletonHierarchy01 = NULL; 
	m_pMountLinkSkeletonHierarchy02 = NULL;
	m_pMountLinkSkeletonHierarchy03 = NULL;
	m_strFilePath        = "";
	m_szSkeletonFileName = "";
}

//-----------------------------------------------------------------------------
// Name: ~cSkeletonSerializer()
// Desc: cSkeletonSerializer �⹹Ԫ
//-----------------------------------------------------------------------------
cSkeletonSerializer::~cSkeletonSerializer()
{
	// 060606 cyhsieh Bug Fix:��������ʹ��ȴ��ɾ������
	n_delete_array( m_pACTFileInfo );

	n_delete_array(m_pWeaponSkeletonHierarchyTable);
	n_delete_array(m_pMagicSkeletonHierarchyTable);

	n_delete( m_pSkeletonHierarchyRoot );
	//jingjie test 2007.07.07 animation root is on parent.
	//m_pAnimationRoot = m_pAnimationRoot->getParent();
	n_delete( m_pAnimationRoot );

	m_pMountLinkSkeletonHierarchy01 = 0;
	m_pMountLinkSkeletonHierarchy02 = 0;
	m_pMountLinkSkeletonHierarchy03 = 0;
}

//-----------------------------------------------------------------------------
// Name: getSkeletonHierarchyRoot()
// Desc:   get skeleton hierarchy root ptr
//-----------------------------------------------------------------------------
cSkeletonHierarchy* cSkeletonSerializer::getSkeletonHierarchyRoot()
{
	return m_pSkeletonHierarchyRoot;
}

//-----------------------------------------------------------------------------
// Name: getWeaponSkeletonHierarchy()
// Desc:   get  ptr table of weapon skeleton hierarchy( �������ӵ� )
//-----------------------------------------------------------------------------
cSkeletonHierarchy* cSkeletonSerializer::getWeaponSkeletonHierarchy( unsigned int uiWeaponSkeletonHierarchyIndex )
{
	if( uiWeaponSkeletonHierarchyIndex >= 0 && 
		uiWeaponSkeletonHierarchyIndex < m_dwWeaponSize )
	{
		return m_pWeaponSkeletonHierarchyTable
			[uiWeaponSkeletonHierarchyIndex];
	}else{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: getWeaponSkeletonHierarchySize()
// Desc:   get size  of weapon skeleton hierarchy( �������ӵ����� )
//-----------------------------------------------------------------------------
DWORD cSkeletonSerializer::getWeaponSkeletonHierarchySize()
{
	return m_dwWeaponSize;
}

//-----------------------------------------------------------------------------
// Name: getMagicSkeletonHierarchy()
// Desc:   get  ptr table of magic skeleton hierarchy( ħ�����ӵ� )
//-----------------------------------------------------------------------------
cSkeletonHierarchy* cSkeletonSerializer::getMagicSkeletonHierarchy( unsigned int uiMagicSkeletonHierarchyIndex )
{
	if( uiMagicSkeletonHierarchyIndex >= 0 && uiMagicSkeletonHierarchyIndex 
		< m_dwMagicSize )
	{
		return m_pMagicSkeletonHierarchyTable
			[uiMagicSkeletonHierarchyIndex];
	}else{
		return 0;
	}

}

//-----------------------------------------------------------------------------
// Name: getMagicSkeletonHierarchySize()
// Desc:   get size  of magic skeleton hierarchy( ħ�����ӵ����� )
//-----------------------------------------------------------------------------
DWORD cSkeletonSerializer::getMagicSkeletonHierarchySize()
{
	return m_dwMagicSize;
}

//-----------------------------------------------------------------------------
// Name: getMountSkeletonHierarchy()
// Desc:   get  ptr table of mount skeleton hierarchy( ��������ӵ� )
//-----------------------------------------------------------------------------
cSkeletonHierarchy* cSkeletonSerializer::getMountSkeletonHierarchy(int in_index)
{
	switch(in_index)
	{
	case 1:  return m_pMountLinkSkeletonHierarchy01; // �������
	case 2:  return m_pMountLinkSkeletonHierarchy02; // ˫����� ǰ��
	case 3:  return m_pMountLinkSkeletonHierarchy03; // ˫����� ����
	default: return NULL;
	}
	//return m_pMountLinkSkeletonHierarchy;
}

//-----------------------------------------------------------------------------
// Name: getSkeletonHierarchyRoot()
// Desc:   get animation root ptr 
//-----------------------------------------------------------------------------
cAnimation* cSkeletonSerializer::getAnimationRoot()
{
	return m_pAnimationRoot;
}

//-----------------------------------------------------------------------------
// Name: getActionTable()
// Desc:   get group of animation action keyframes range
//-----------------------------------------------------------------------------
cAnimationActionInfoGroup* cSkeletonSerializer::getAnimationActionInfoGroup()
{
	return &m_AnimationActionInfoGroup;
}

//-----------------------------------------------------------------------------
// Name: constructLinkerSkeletonHierarchyTables()
// Desc:   ��������Ƭ, ��ЧƬ, �����������Ѷ�б�
//-----------------------------------------------------------------------------
void cSkeletonSerializer::constructLinkerSkeletonHierarchyTables()
{
	n_delete_array(m_pWeaponSkeletonHierarchyTable); 
	n_delete_array(m_pMagicSkeletonHierarchyTable);

	/*	DWORD i;
	char ch[255];
	*/
	m_dwWeaponSize = 0 ;
	m_dwMagicSize  = 0 ; 
	//---------------------------------------------------
	// ����������
	// ���㲢��¼��������������
	// 
	//m_pSkeletonHierarchyRoot->countSkeletonHierarchy("k_weapon",m_dwWeaponSize);
	cSkeletonHierarchy::SkeletonHierarchyMap* pSkeletonHierarchyWeaponLinkerMap =  
		m_pSkeletonHierarchyRoot->getSkeletonHierarchyLinkerMaps(cSkeletonHierarchy::SkeletonHierarchyWeaponLinker);
	m_dwWeaponSize = pSkeletonHierarchyWeaponLinkerMap->size();

	// ���㲢��¼��Ч����������
	// 
	//m_pSkeletonHierarchyRoot->countSkeletonHierarchy("k_magic",m_dwMagicSize);
	cSkeletonHierarchy::SkeletonHierarchyMap* 
		pSkeletonHierarchyMagicLinkerMap =  
		m_pSkeletonHierarchyRoot->getSkeletonHierarchyLinkerMaps(cSkeletonHierarchy::SkeletonHierarchyMagicLinker);
	m_dwMagicSize = pSkeletonHierarchyMagicLinkerMap->size();

	//---------------------------------------------------
	// ��������������б�
	if (m_dwWeaponSize > 0) m_pWeaponSkeletonHierarchyTable = n_new_array(cSkeletonHierarchy*, m_dwWeaponSize);
	// ������Ч������б�
	if (m_dwMagicSize  > 0) m_pMagicSkeletonHierarchyTable  = n_new_array(cSkeletonHierarchy*, m_dwMagicSize);

	//---------------------------------------------------
	// �����ҳ����е����������
	/*for (i = 0; i < m_dwWeaponSize; i++)
	{
	sprintf(ch,"k_weapon%02d\0",i);
	m_pWeaponSkeletonHierarchyTable[i] = FindFrame(ch);
	} // end of for (i = 0; i < m_dWeaponSize; i++)
	*/
	// 
	cSkeletonHierarchy::SkeletonHierarchyMap::iterator map_iterator, map_iterator_iend;
	map_iterator = pSkeletonHierarchyWeaponLinkerMap->begin();
	map_iterator_iend = pSkeletonHierarchyWeaponLinkerMap->end();
	unsigned int j = 0;
	for (; map_iterator != map_iterator_iend; ++map_iterator)
	{
		m_pWeaponSkeletonHierarchyTable[j] = map_iterator->second;
		j++;
	}

	// �����ҳ����е���Ч�����
	/*for (i = 0; i < m_dwMagicSize; i++)
	{
	sprintf(ch,"k_magic%02d\0",i+1);
	m_pMagicSkeletonHierarchyTable[i] = findSkeletonHierarchy(ch);
	} // end of for (i = 0; i < m_dMagicSize; i++)
	*/
	// 
	map_iterator = pSkeletonHierarchyMagicLinkerMap->begin();
	map_iterator_iend = pSkeletonHierarchyMagicLinkerMap->end();
	j = 0;
	for (; map_iterator != map_iterator_iend; ++map_iterator)
	{
		m_pMagicSkeletonHierarchyTable[j] = map_iterator->second;
		j++;
	}

	// �ҳ���������
	m_pMountLinkSkeletonHierarchy01 = findSkeletonHierarchy("Link_Ride01");
	m_pMountLinkSkeletonHierarchy02 = findSkeletonHierarchy("Link_Ride02");
	m_pMountLinkSkeletonHierarchy03 = findSkeletonHierarchy("Link_Ride03");
}

//-----------------------------------------------------------------------------
// Name: findSkeletonHierarchy()
// Desc:   find a SkeletonHierarchy by SkeletonHierarchyName
//-----------------------------------------------------------------------------
// 
cSkeletonHierarchy* cSkeletonSerializer::findSkeletonHierarchy(const char *szSkeletonHierarchyName)
{
	// 
	cSkeletonHierarchy *pSkeletonHierarchy = NULL;

	if (!pSkeletonHierarchy && m_pSkeletonHierarchyRoot)
	{
		//DWORD hashID = m_pSkeletonHierarchyRoot->hashIt((char*)sFindName);
		//pSkeletonHierarchy = m_pSkeletonHierarchyRoot->findSkeletonHierarchyWithHash((char *)sFindName, hashID);
		pSkeletonHierarchy = m_pSkeletonHierarchyRoot->findSkeletonHierarchy((char *)szSkeletonHierarchyName);
	}

	return pSkeletonHierarchy;
}

//-----------------------------------------------------------------------------
// Name: ComputeAnimation()
// Desc:
//-----------------------------------------------------------------------------
// 060606 cyhsieh Bug Fix:��������ʹ��ȴ��ɾ������
// ���㶯���ͼ�ȥ��������ʱ��ֿ�
// ��ΪFrameWork�������ǹ��õģ����ܻ�ɾ�����������õĶ���
#ifdef _CHECK_NOUSE_ANIMATION
//   �޸�Mount Matrix�ļ���
void cSkeletonSerializer::computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, D3DXMATRIXA16* pMat, SkeletonHierarchyUsabilityMap* pSkeletonHierarchyUnUsedMap )
#else
//   �޸�Mount Matrix�ļ���
void cSkeletonSerializer::computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, D3DXMATRIXA16* pMat )
#endif
{
	// ��ȡĿǰʹ�õĶ���
	if( param.m_iActionIndex1 >= 0 )
	{
		prepareAnimationKeys( param.m_iActionIndex1 );
	}

	// ��ȡĿǰʹ�õĶ���
	if( param.m_iActionIndex2 >= 0 )
	{
		prepareAnimationKeys( param.m_iActionIndex2 );
	}

	// 1.�������й����ľ���
	if( m_pSkeletonHierarchyRoot )
	{
		g_pStack->Push();

		//   �޸�Mount Matrix�ļ���
		if( pMat )
		{
			g_pStack->LoadMatrix( pMat );
		}
		else
		{
			g_pStack->LoadIdentity();
		}

#ifdef _CHECK_NOUSE_ANIMATION
		m_pSkeletonHierarchyRoot->computeSkeletonHierarchyMatrix( &m_AnimationActionInfoGroup,
			param,
			pSkeletonHierarchyUnUsedMap );
#else
		m_pSkeletonHierarchyRoot->computeSkeletonHierarchyMatrix(&m_AnimationActionInfoGroup,param );
#endif

		g_pStack->Pop();
	}
}

//-----------------------------------------------------------------------------
// Name: importActionTable()
// Desc: ��ȡÿ��������Ӧ�Ķ��������嵥
//       Ŀǰȫ���Ķ���(��������ʽ����)�� 168 ������, 
//       ��ʽ���ܶ������б�������һ��������, ��ʽ�����
//       �ٸ����� 168 ������|֮���. 
//-----------------------------------------------------------------------------
// 060502 cyhsieh Act Loading ���̼�
// ԭ��������Ϊ�������еĶ�����һ�Σ���ȡ������Ķ�������(������ʼʱ�䡢��������ʱ��ȵ�)��
// �򻯵����̸�Ϊֻ��ȡ���һ��������(��ʽ����)������Ķ������ϻ���������ȡ��Act��ʱ����
// ���⶯���Ĺ�ͷ���ṹ�Ľ���Ҳֻ��һ��
// ���Լ򻯹�������ǰ����������
// 1.���������嵥�е����һ������������г���һ�������������Ķ���һ������һ����
// 2.����������Ĺ�ͷ������һ��
int cSkeletonSerializer::importActionTable(int iModelType, char *szActionTableFileName,const char *szPath)
{
	int iActionBaseIndex = 0;
	int iActionOffsetIndex = 0;
	int iReturnValue = 0;
	char szMessage[256];

	cAnimationAction* pAnimationAction = NULL;
	cAnimationActionInfo* pAnimationActionInfo = NULL;
	// 
	//cAnimation* pTempAnimaRoot = _NEW cAnimation;
	n_delete( m_pAnimationRoot );
	m_pAnimationRoot = n_new(cAnimation);

	String strFileName;
	m_strFilePath = szPath;
	// MOB, NPC, ��һ��������������������, ����Ҫ����Щ��Ѷ�⿪����... 
	pAnimationAction = n_new(cAnimationAction);


	//�Ƚ�����ÿ��animation action key frames range	��
	// ��ȡ AFT ��
	switch (iModelType)
	{
		case mtMOB: // 
		case mtNPC: // NPC, MOB Model
			{
				if ( m_AnimationActionInfoGroup.importAnimationActionInfoGroup( szActionTableFileName, szPath ) < 0 )
				{
					iReturnValue = -1;
					goto FW_LAF_QUIT1;
				} // end of if ( importAnimationActionInfoGroup )
			}
			break;
		case mtPLAYER: // ��� Model
		case mtMOUNT:  // ����� (�����XTB��������ҵ�XTB��ͬ)
			{
				if ( m_AnimationActionInfoGroup.importAnimationActionInfoGroupPLAYER( szActionTableFileName, szPath ) < 0)
				{
					iReturnValue = -1;
					goto FW_LAF_QUIT1;
				}
			}
			break;
	} // end of switch (iModelType)

	if ( pAnimationAction )
	{
		// ȡ�ö������� (MOB, NPC, ֻ��һ��������, ȡ����һ������.
		strFileName.Format("%s%s.act", szPath, m_AnimationActionInfoGroup.getAnimationACTName( 0 ) );
		//����ȡkey frame data �������ȡ Skeleton Hierarchy Name �Լ���kyy frame range�ͺ�
			//key frame data�ȵ������õ��˶����Ŷ�ȡ(prepareActionData)
			// ��GKK2��, ���ܶ�����ȫ������һ�����������, Ϊ��֪�����ж�����
			// KeyFrame��KeyTime�з�, ֻ���Ȱ�����������ȡһ��... 
			if ( pAnimationAction->importAnimationAction( (char*)strFileName.AsCharPtr(), true ) < 0 )
			{
				sprintf( szMessage, "��ȡ������ %s ����", (char*)strFileName.AsCharPtr() );
				iReturnValue = -2;
				goto FW_LAF_QUIT1;
			}
			// 
			//int iCount = pAction->ActTableList->GetCount();
			// ��϶�����������֮��Ķ�������...
			int i, iCount = pAnimationAction->getAnimationActionInfoCount();
			for ( i = 0; i < iCount; i++ )
			{
				pAnimationActionInfo = (cAnimationActionInfo*)pAnimationAction->getAnimationActionInfo( i );
				//iActionBaseIndexΪact��index(��0��ʼ)��ʾ�ڼ���act��,��������key frames data�����ڵڼ���act��
					//����һ��act���ڰ����������,����������ж�����key frames dataȫ��������һ��key frames data,
					//���Ҫȡ�Ĵ�act�����κζ���,�����뷴��key frames data�ڵڼ���act��,Ȼ������key frames rangeȥ�鵽�˶���
					//����key frames data���ĸ�key frame��ʼ����,���ĸ�key time��ʼ����,��ȡ�����������ȷ��key frames dataȥ���㶯��.
					pAnimationActionInfo->setAnimationACTIndex( 0 );
				//m_ActionTable.AddActTable( pAnimationActionInfo );
				//iActionBaseIndexΪact��index(��0��ʼ)��ʾ�ڼ���act��,����һ��act���ڰ��������ʽ���ܶ���.
					//���Ҫ�Ѵ˵�������ʽ���ܶ�����key frame range������,���������ҵ��õ��ڸö�����key frame data ,
					//iActionOffsetIndex���Ǹ�act����ÿ��key frame range��index,����iActionBaseIndex + iActionOffsetIndex������
					//���ǵ�iActionBaseIndex��act���ڵĵ�iActionOffsetIndex��key frame range.
					//����֮,������ʽ���ܶ������ǵ�168 index,��������2����ʽ������ index of key frame range����190.
					m_AnimationActionInfoGroup.addAnimationActionInfo( i, pAnimationActionInfo );
			} // end of for (i = 0; i < iCount; i++)
	} // end of if (pAnimationAction)


	// ����������ͷ��
	cAnimation* pTmpAnimation = 0;
	// 
	cSkeletonHierarchy* pSkeletonHierarchy = 0;
	// 
	for ( unsigned int i = 0; i < pAnimationAction->getAnimationKeysCount(); i++ )
	{
		cAnimationKeys* pAnimationKeys = pAnimationAction->getAnimationKeys( i );
		if ( pAnimationKeys )
		{
			pTmpAnimation = m_pAnimationRoot->addAnimationChild(  pAnimationKeys->getSkeletonHierarchyName() );
			pSkeletonHierarchy = findSkeletonHierarchy(  pAnimationKeys->getSkeletonHierarchyName() );

			if ( pSkeletonHierarchy )
			{
				// 
				pSkeletonHierarchy->setAnimation( pTmpAnimation );
			}
			else
			{
				// ���û�д�skeletonȴ�д�skeleton's animation�Ĵ���
				sprintf( szMessage, "������ %s animation error: \n  û�д�skeleton( %s: %s ),ȴ�д�skeleton's animation.", 
				strFileName.AsCharPtr() , m_szSkeletonFileName.AsCharPtr() , pAnimationKeys->getSkeletonHierarchyName() );
			}
		} // end of if ( pAnimationKeys )
	} // end of for (

	n_delete_array( m_pACTFileInfo );
	// ������������, ���� ActFileLifeTime ����
	if ( m_AnimationActionInfoGroup.getAnimationACTCount() > 0 )
	{
		m_pACTFileInfo = n_new_array(stACTFILEINFO, m_AnimationActionInfoGroup.getAnimationACTCount());
		if ( m_pACTFileInfo )
		{
			memset(m_pACTFileInfo, 0, sizeof(stACTFILEINFO) * 
				m_AnimationActionInfoGroup.getAnimationACTCount());
		} // end of if (
	}

	// ��Ч�¹�� SndTable
	{
		strFileName = m_strFilePath + szActionTableFileName;
		strFileName += ".stb";
		cSndTable sndTable;
		sndTable.LoadTable( (char*)strFileName.AsCharPtr(), &m_AnimationActionInfoGroup );
	} // end of ��Ч�¹��

	// ��ȡ�¼���
	{
		strFileName = m_strFilePath + szActionTableFileName;
		strFileName += ".etb";
		cEventTableReader eventTableReader;
		eventTableReader.LoadTable( (char*)strFileName.AsCharPtr(), &m_AnimationActionInfoGroup );
	}

FW_LAF_QUIT1:
	n_delete( pAnimationAction );
	//n_delete( pTempAnimaRoot );

	return iReturnValue;
}

//-----------------------------------------------------------------------------
// Name: importSkeleton()
// Desc:   import skeleton from file or lpq
//-----------------------------------------------------------------------------
int cSkeletonSerializer::importSkeleton(char *szSkeletonFileName,const char *szPath)
{
	char   ATFChar[4];
	m_szSkeletonFileName = szSkeletonFileName;
	cSkeletonHierarchy* pSkeletonHierarchy = NULL;

	// ���skeletomn hierarchy root
	n_delete_array( m_pSkeletonHierarchyRoot );

	int iNameSize = lstrlen(szSkeletonFileName);
	int iPathSize = lstrlen(szPath);

	char* pOpenFileName = NULL;

	if(iNameSize>0)
	{
		pOpenFileName = n_new_array(char, iNameSize+iPathSize+5);
		sprintf(pOpenFileName, "%s%s.atf", szPath, szSkeletonFileName);
	}
	else
	{
		return -1;
	}


	Ptr<Stream> stream = IoServer::Instance()->CreateStream(pOpenFileName);
	if (!stream->Open())
	{
		stream = 0;
		return -1;
	}
	// ��ȡATFʶ����
	// ��ȡ�汾
	stream->Read(ATFChar, sizeof(char)*3);
	ATFChar[3] = '\0';
	stream->Read(&m_iVersion, sizeof(int));

	if (lstrcmp(ATFChar,"ATF"))  // ���ATFʶ����
	{
		stream->Close();
		// ATFʶ���벻��
		n_delete_array(pOpenFileName);
		return -2;
	}
	// 
	pSkeletonHierarchy = n_new(cSkeletonHierarchy);
	m_pSkeletonHierarchyRoot = pSkeletonHierarchy;
	m_pSkeletonHierarchyRoot->importFromMemory(stream);

	stream->Close();
	stream = 0;

	return 0;
}

//-----------------------------------------------------------------------------
// Name: importSkeletonFromFile()
// Desc:   import skeleton from file
//-----------------------------------------------------------------------------
int cSkeletonSerializer::importSkeletonFromFile(char *szSkeletonFileName,const 
												char *szPath)
{
	//char   ATFChar[4];
	//m_szSkeletonFileName = szSkeletonFileName;
	//// 
	//cSkeletonHierarchy* pSkeletonHierarchy = NULL;

	//// ���skeletomn hierarchy root
	//n_delete( m_pSkeletonHierarchyRoot );

	//int iNameSize =lstrlen(szSkeletonFileName);
	//int iPathSize =lstrlen(szPath);

	//char* pOpenFileName = NULL;

	//if(iNameSize > 0)
	//{
	//	pOpenFileName = n_new_array(char, iNameSize+iPathSize+5);
	//	sprintf(pOpenFileName, "%s%s.atf", szPath, szSkeletonFileName);
	//}
	//else
	//{
	//	return -1;
	//}

	//FILE *fp;
	//if ((fp = fopen(pOpenFileName,"rb")) == NULL ) {
	//	n_delete_array(pOpenFileName);
	//	return -1;
	//}


	//fread(ATFChar,3, 1, fp);        		// ��ȡATFʶ����
	//fread(&m_iVersion,sizeof(int), 1, fp);         // ��ȡ�汾

	//ATFChar[3] = '\0';

	//if (lstrcmp(ATFChar,"ATF"))  // ���ATFʶ����
	//{
	//	// ATFʶ���벻��
	//	LOGS(LoadAtfFile,"ATFʶ���벻��");

	//	fclose( fp );
	//	n_delete_array(pOpenFileName);
	//	return -2;
	//}
	//// 
	//pSkeletonHierarchy = _NEW cSkeletonHierarchy();
	//m_pSkeletonHierarchyRoot = pSkeletonHierarchy;
	//m_pSkeletonHierarchyRoot->importFromFile(fp);

	//fclose( fp );
	//n_delete_array(pOpenFileName);

	return 0;
}

//-----------------------------------------------------------------------------
// Name: removeAnimationKeysByReserveTime()
// Desc:
//-----------------------------------------------------------------------------
void cSkeletonSerializer::removeAnimationKeysByReserveTime( float fTime )
{
	if ( m_pAnimationRoot == NULL || m_pACTFileInfo == NULL )
		return;

	for ( int i = 0; i < m_AnimationActionInfoGroup.getAnimationACTCount(); i++ )
	{
		if ( m_pACTFileInfo[i].m_fLifeTime > 0.0f )
		{
			m_pACTFileInfo[i].m_fLifeTime -= fTime;

			if ( m_pACTFileInfo[i].m_fLifeTime <= 0.0f )
			{
				m_pACTFileInfo[i].m_fLifeTime = 0.0f;
				//m_pAnimationRoot->DelAnimData( i );
				m_pAnimationRoot->removeAnimationKeysFromAnimationMap( i );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: prepareAnimationKeys()
// Desc: Act Loading ���̼�
//-----------------------------------------------------------------------------
void cSkeletonSerializer::prepareAnimationKeys( int iActionIndex )
{
	//cAnimationActionKeyframesRange* pAnimationActionKeyframesRange = m_ActionTable.GetActTable( iActionIndex );
	cAnimationActionInfo* pAnimationActionInfo = m_AnimationActionInfoGroup.getAnimationActionInfo( iActionIndex );

	// Bug Fix:��������ʹ��ȴ��ɾ������
	if ( (pAnimationActionInfo == NULL) || 
		(m_pAnimationRoot == NULL)     || 
		(m_pACTFileInfo == NULL) ) 
	{
		return;
	}
	//int iActID = pActTable->m_iActionIndex;
	unsigned int iActID = pAnimationActionInfo->getAnimationACTIndex();

	// �õ���������, ��ȡ�õ�
	// Bug Fix:��������ʹ��ȴ��ɾ������
	if ( m_pACTFileInfo[iActID].m_fLifeTime <= 0.0f )
	{
		cAnimationAction* pAnimationAction = n_new(cAnimationAction);

		// �޷������D������, �뿪��ʽ...
		if ( pAnimationAction == NULL ) return;

		String strFileName;

		// ��ȡ������
		strFileName.Format("%s%s.act", (char*)m_strFilePath.AsCharPtr(), m_AnimationActionInfoGroup.getAnimationACTName( iActID ) );

		if ( pAnimationAction->importAnimationAction( strFileName.AsCharPtr() ) < 0 )
		{
			//char szMessage[256];
			//sprintf( szMessage, "importAnimationAction ( %s ) Error.", strFileName.AsCharPtr() );
			n_delete( pAnimationAction );
			return;
		}

		//WORD wBones = pAction->m_wNumAnimData;
		// 
		WORD wBones = pAnimationAction->getAnimationKeysCount();
		//cAnimationKeys* pAnimData = pAction->m_aAnimData;
		//DWORD dwHashID;

		// ����Act Table����
		int iCount = pAnimationAction->getAnimationActionInfoCount();

		// ���ACT������ֻ�� 1 ������, ������������п��ܱ�����, 
		// ��ҵĶ������, �� 1 ��ACT�� 1 ������.
		// MOB, NPC �Ķ������, �� 1 ��ACT������ 20 ������.
		// �б�Ҫ��¼�� StartTime, EndTime, StartFrame, EndFrame ����Ѷ
		if (iCount == 1) 
		{
			cAnimationActionInfo* pAnimationActionInfo2 = (cAnimationActionInfo*)
				pAnimationAction->getAnimationActionInfo( 0 );
			stACTFILEINFO* pACTFileInfo = &m_pACTFileInfo[iActID];

			pACTFileInfo->m_bSingle      = true;
			pACTFileInfo->m_dwStartTime  = pAnimationActionInfo2->getAnimationActionStartTime();
			pACTFileInfo->m_dwEndTime    = pAnimationActionInfo2->getAnimationActionEndTime();
			pACTFileInfo->m_dwStartFrame = pAnimationActionInfo2->getAnimationActionStartKeyframe();
			pACTFileInfo->m_dwEndFrame   = pAnimationActionInfo2->getAnimationActionEndKeyframe();
		} // end of if (iCount == 1)
		else
		{
			for ( int iIndex = 0; iIndex < iCount; iIndex++ )
			{
				// 
				//cAnimationActionKeyframesRange* pAnimationActionKeyframesRange1 = m_ActionTable.GetActTable( iActID + iIndex );
				cAnimationActionInfo* pAnimationActionInfo1 = m_AnimationActionInfoGroup.getAnimationActionInfo( iActID + iIndex ); // iActionIndex + iIndex ); //iActID + iIndex );
				//cAnimationActionKeyframesRange* pAnimationActionKeyframesRange2 = (cAnimationActionKeyframesRange*)pAction->ActTableList->GetItem( iIndex );
				// 
				cAnimationActionInfo* pAnimationActionInfo2 = (cAnimationActionInfo*)pAnimationAction->getAnimationActionInfo( iIndex );
				if ( pAnimationActionInfo1 && pAnimationActionInfo2 )
				{
					pAnimationActionInfo1->setAnimationActionStartTime( 
						pAnimationActionInfo2->getAnimationActionStartTime() );
					pAnimationActionInfo1->setAnimationActionEndTime( 
						pAnimationActionInfo2->getAnimationActionEndTime() );
					pAnimationActionInfo1->setAnimationActionStartKeyframe( 
						pAnimationActionInfo2->getAnimationActionStartKeyframe() );
					pAnimationActionInfo1->setAnimationActionEndKeyframe( 
						pAnimationActionInfo2->getAnimationActionEndKeyframe() );
				} // end of if (
			} // end of for 
		} // end of if (iCount != 1)

		//
		for ( int j = 0; j < wBones; j++ )
		{
			// 
			cAnimation* pAnimation = NULL;
			cAnimationKeys* pAnimationKeys = pAnimationAction->getAnimationKeys( j );
			if ( pAnimationKeys)
			{
				pAnimation= m_pAnimationRoot->findAniamtion( pAnimationKeys->getSkeletonHierarchyName());
			} // end of if ( pAnimationKeys )

			if ( pAnimation )
			{
				// 
				pAnimation->addAnimationKeys( iActID, pAnimationKeys );
			} // end of if ( pAnimation )
		} // end of for (

		n_delete( pAnimationAction );
	} // end of if ( m_pACTFileInfo[iActID].m_fLifeTime <= 0.0f )

	// ����ǵ�һ����, ���п��ܹ���, ��Ҫע��ʱ����Ѷ���趨
	if (m_pACTFileInfo[iActID].m_bSingle)
	{
		// ����ʱ����Ѷ
		pAnimationActionInfo->setAnimationActionStartTime(m_pACTFileInfo[iActID].m_dwStartTime);
		pAnimationActionInfo->setAnimationActionEndTime(m_pACTFileInfo[iActID].m_dwEndTime);
		pAnimationActionInfo->setAnimationActionStartKeyframe(m_pACTFileInfo[iActID].m_dwStartFrame);
		pAnimationActionInfo->setAnimationActionEndKeyframe(m_pACTFileInfo[iActID].m_dwEndFrame);
	} // end of if (m_pACTFileInfo[iActID].m_bSingle)

	// Bug Fix:��������ʹ��ȴ��ɾ������
	m_pACTFileInfo[iActID].m_fLifeTime = ACTFILE_RESERVE_TIME;
}

}