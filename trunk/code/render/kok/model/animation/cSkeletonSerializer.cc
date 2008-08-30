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
	// 设定目前使用的 pCurAnimationActionKeyframesRangeGroup
	m_pAnimationActionInfoGroup = pAnimationActionInfoGroup;

	if (!LoadFromFile(szFileName)) return false;
	// 重置
	Reset(); 
	// 开始解析
	for (;;)
	{
		switch (GetToken())
		{
		case ttIDENTIFIER: // 字符?
			if (ParseCommand()) 
			{
				continue; 
			}
		case ttINT: // 整数
			if (ParseSound())
			{
				continue;
			}
			continue;
		case ttCOMMENT: // 注解
			continue;
		} // end of if (type == ttIDNETIFIER)
		// 离开回圈
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
	// 变成小写
	strCMD.ToLower();

	if (strCMD == "clear") 
	{
		// 清除 cAction 里所有的音效档名... 
		m_pAnimationActionInfoGroup->clearSoundSetting();

		return true;
	} // end of if ("clear")

	// 未知的指令
	return false;
}

//---------------------------------------------------------------------------
// Name: ParseSound
// Desc:
//---------------------------------------------------------------------------
bool cSndTable::ParseSound()
{
	// 取得动作索引
	int iIndex = GetTokenInt();
	int iDelayTimems = 0; 

	String StrName;

	int TokenType = GetToken();
	// 1 ("xxx",1)
	if (TokenType == ttPARENOPEN)
	{
		if (GetToken() != ttSTRING) return false;
		// 取得音效档名
		StrName = GetTokenStr();    

		TokenType  = GetToken();

		if (TokenType == ttCOMMA)
		{
			TokenType  = GetToken();
			// 取得音效延迟时间
			if (TokenType!= ttINT) return false;
			iDelayTimems = GetTokenInt();

			TokenType  = GetToken();
		}

		if (TokenType != ttPARENCLOSE) return false; 
	}
	else
	{
		if (TokenType != ttSTRING) return false;
		// 取得音效档名
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
	// 设定目前使用的 pCurAnimationActionKeyframesRangeGroup
	m_pAnimationActionInfoGroup = pAnimationActionInfoGroup;

	if (!LoadFromFile(pFileName))
		return false;

	// 重置
	Reset(); 

	// 开始解析
	for( ; ; )
	{
		switch( GetToken() )
		{
		case ttINT:     // 整数
			if( ParseEvent() )
			{
				continue;
			}
			break;
		case ttCOMMENT: // 注解
			continue;
		}
		// 离开回圈
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
	// 动作编号 { {事件} {事件} }
	// 取得动作索引
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
			if( GetToken() != ttINT ) // 事件ID
			{
				return false;
			}
			switch( GetTokenInt() )
			{
			case 1: // 打击点 {1,跳血时间}
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
			BackupPtr(); // 备份目前的解析位置
			if( GetToken() == ttBRACECLOSE )
			{
				ReloadPtr();  // 回复先前的解析位置
				break;
			}
			ReloadPtr();  // 回复先前的解析位置
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
	if( GetToken() != ttCOMMA ) // 逗号
	{
		return false;
	}

	if( GetToken() != ttINT )   // 跳血时间
	{
		return false;
	}

	float fHurtTime = GetTokenInt() / 24.0f;  // 1秒24个frame

	if( m_pAnimationActionInfoGroup )
	{
		m_pAnimationActionInfoGroup->AddHurtEvent( iActionIndex, fHurtTime );
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: cSkeletonSerializer()
// Desc: cSkeletonSerializer 建构元
//-----------------------------------------------------------------------------
cSkeletonSerializer::cSkeletonSerializer()
{
	// 
	m_iVersion = 0;
	m_dwWeaponSize = 0;
	m_dwMagicSize = 0;

	//   Mob特效缩放
	m_fEffectScale = 1.0f;

	// 080123 cyhsieh 骑乘摄影机调整
	m_pMountCameraAdjust = NULL;

	m_pAnimationRoot = 0;
	m_pSkeletonHierarchyRoot = 0;

	// 060606 cyhsieh Bug Fix:动作还在使用却被删除问题
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
// Desc: cSkeletonSerializer 解构元
//-----------------------------------------------------------------------------
cSkeletonSerializer::~cSkeletonSerializer()
{
	// 060606 cyhsieh Bug Fix:动作还在使用却被删除问题
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
// Desc:   get  ptr table of weapon skeleton hierarchy( 武器连接点 )
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
// Desc:   get size  of weapon skeleton hierarchy( 武器连接点总数 )
//-----------------------------------------------------------------------------
DWORD cSkeletonSerializer::getWeaponSkeletonHierarchySize()
{
	return m_dwWeaponSize;
}

//-----------------------------------------------------------------------------
// Name: getMagicSkeletonHierarchy()
// Desc:   get  ptr table of magic skeleton hierarchy( 魔法连接点 )
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
// Desc:   get size  of magic skeleton hierarchy( 魔法连接点总数 )
//-----------------------------------------------------------------------------
DWORD cSkeletonSerializer::getMagicSkeletonHierarchySize()
{
	return m_dwMagicSize;
}

//-----------------------------------------------------------------------------
// Name: getMountSkeletonHierarchy()
// Desc:   get  ptr table of mount skeleton hierarchy( 骑乘物连接点 )
//-----------------------------------------------------------------------------
cSkeletonHierarchy* cSkeletonSerializer::getMountSkeletonHierarchy(int in_index)
{
	switch(in_index)
	{
	case 1:  return m_pMountLinkSkeletonHierarchy01; // 单人骑乘
	case 2:  return m_pMountLinkSkeletonHierarchy02; // 双人骑乘 前座
	case 3:  return m_pMountLinkSkeletonHierarchy03; // 双人骑乘 后座
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
// Desc:   建立武器片, 特效片, 骑乘连结点等资讯列表
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
	// 计算表格数量
	// 计算并记录武器连结点的数量
	// 
	//m_pSkeletonHierarchyRoot->countSkeletonHierarchy("k_weapon",m_dwWeaponSize);
	cSkeletonHierarchy::SkeletonHierarchyMap* pSkeletonHierarchyWeaponLinkerMap =  
		m_pSkeletonHierarchyRoot->getSkeletonHierarchyLinkerMaps(cSkeletonHierarchy::SkeletonHierarchyWeaponLinker);
	m_dwWeaponSize = pSkeletonHierarchyWeaponLinkerMap->size();

	// 计算并记录特效连结点的数量
	// 
	//m_pSkeletonHierarchyRoot->countSkeletonHierarchy("k_magic",m_dwMagicSize);
	cSkeletonHierarchy::SkeletonHierarchyMap* 
		pSkeletonHierarchyMagicLinkerMap =  
		m_pSkeletonHierarchyRoot->getSkeletonHierarchyLinkerMaps(cSkeletonHierarchy::SkeletonHierarchyMagicLinker);
	m_dwMagicSize = pSkeletonHierarchyMagicLinkerMap->size();

	//---------------------------------------------------
	// 建立武器连结点列表
	if (m_dwWeaponSize > 0) m_pWeaponSkeletonHierarchyTable = n_new_array(cSkeletonHierarchy*, m_dwWeaponSize);
	// 建立特效连结点列表
	if (m_dwMagicSize  > 0) m_pMagicSkeletonHierarchyTable  = n_new_array(cSkeletonHierarchy*, m_dwMagicSize);

	//---------------------------------------------------
	// 依序找出所有的武器连结点
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

	// 依序找出所有的特效连结点
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

	// 找出骑乘连结点
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
// 060606 cyhsieh Bug Fix:动作还在使用却被删除问题
// 计算动作和减去动作保留时间分开
// 因为FrameWork的资料是共用的，可能会删除掉别人在用的动作
#ifdef _CHECK_NOUSE_ANIMATION
//   修改Mount Matrix的计算
void cSkeletonSerializer::computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, D3DXMATRIXA16* pMat, SkeletonHierarchyUsabilityMap* pSkeletonHierarchyUnUsedMap )
#else
//   修改Mount Matrix的计算
void cSkeletonSerializer::computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, D3DXMATRIXA16* pMat )
#endif
{
	// 读取目前使用的动作
	if( param.m_iActionIndex1 >= 0 )
	{
		prepareAnimationKeys( param.m_iActionIndex1 );
	}

	// 读取目前使用的动作
	if( param.m_iActionIndex2 >= 0 )
	{
		prepareAnimationKeys( param.m_iActionIndex2 );
	}

	// 1.计算所有骨骼的矩阵
	if( m_pSkeletonHierarchyRoot )
	{
		g_pStack->Push();

		//   修改Mount Matrix的计算
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
// Desc: 读取每个动作对应的动作档名清单
//       目前全部的动作(不包含招式技能)是 168 个动作, 
//       招式技能动作档列表储存在另一个档案里, 程式读入后
//       再附加在 168 个动作|之后的. 
//-----------------------------------------------------------------------------
// 060502 cyhsieh Act Loading 流程简化
// 原本的流程为读过所有的动作档一次，以取得所需的动作资料(动作起始时间、动作结束时间等等)，
// 简化的流程改为只读取最后一个动作档(招式技能)，所需的动作资料会在真正读取该Act档时更新
// 另外动作的骨头树结构的建立也只作一次
// 所以简化过的流程前提有两个：
// 1.动作档名清单中的最后一个动作档里才有超过一个动作，其他的都是一个动作一个档
// 2.各动作档里的骨头数量都一样
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
	// MOB, NPC, 是一个动作档里有数个动作, 必需要把这些资讯解开才行... 
	pAnimationAction = n_new(cAnimationAction);


	//先建立好每个animation action key frames range	的
	// 读取 AFT 档
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
		case mtPLAYER: // 玩家 Model
		case mtMOUNT:  // 骑乘物 (骑乘物XTB内容与玩家的XTB相同)
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
		// 取得动作档名 (MOB, NPC, 只有一个动作档, 取出第一个即可.
		strFileName.Format("%s%s.act", szPath, m_AnimationActionInfoGroup.getAnimationACTName( 0 ) );
		//不读取key frame data 这里仅读取 Skeleton Hierarchy Name 以及其kyy frame range就好
			//key frame data等到真正用到此动作才读取(prepareActionData)
			// 在GKK2里, 技能动作是全部做在一个档案里面的, 为了知道所有动作的
			// KeyFrame与KeyTime切分, 只好先把整个档案读取一遍... 
			if ( pAnimationAction->importAnimationAction( (char*)strFileName.AsCharPtr(), true ) < 0 )
			{
				sprintf( szMessage, "读取动作档 %s 错误", (char*)strFileName.AsCharPtr() );
				iReturnValue = -2;
				goto FW_LAF_QUIT1;
			}
			// 
			//int iCount = pAction->ActTableList->GetCount();
			// 混合动作档被解析之后的动作总数...
			int i, iCount = pAnimationAction->getAnimationActionInfoCount();
			for ( i = 0; i < iCount; i++ )
			{
				pAnimationActionInfo = (cAnimationActionInfo*)pAnimationAction->getAnimationActionInfo( i );
				//iActionBaseIndex为act档index(从0开始)表示第几个act档,用来反查key frames data他是在第几个act档
					//由于一个act档内包含多个动作,因此他把所有动作的key frames data全部集合在一个key frames data,
					//因此要取的此act档内任何动作,都必须反查key frames data在第几个act档,然后再用key frames range去查到此动作
					//是由key frames data的哪个key frame开始结束,从哪个key time开始结束,以取的这个动作正确的key frames data去计算动作.
					pAnimationActionInfo->setAnimationACTIndex( 0 );
				//m_ActionTable.AddActTable( pAnimationActionInfo );
				//iActionBaseIndex为act档index(从0开始)表示第几个act档,由于一个act档内包含多个招式技能动作.
					//因此要把此档所有招式技能动作的key frame range存起来,这样才能找到该档内该动作的key frame data ,
					//iActionOffsetIndex就是该act档内每个key frame range的index,所以iActionBaseIndex + iActionOffsetIndex的意义
					//就是第iActionBaseIndex的act档内的第iActionOffsetIndex个key frame range.
					//举例之,假如招式技能动作档是第168 index,因此里面第2个招式动作的 index of key frame range就是190.
					m_AnimationActionInfoGroup.addAnimationActionInfo( i, pAnimationActionInfo );
			} // end of for (i = 0; i < iCount; i++)
	} // end of if (pAnimationAction)


	// 建立动作骨头树
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
				// 输出没有此skeleton却有此skeleton's animation的错误
				sprintf( szMessage, "动作档 %s animation error: \n  没有此skeleton( %s: %s ),却有此skeleton's animation.", 
				strFileName.AsCharPtr() , m_szSkeletonFileName.AsCharPtr() , pAnimationKeys->getSkeletonHierarchyName() );
			}
		} // end of if ( pAnimationKeys )
	} // end of for (

	n_delete_array( m_pACTFileInfo );
	// 依动作档个数, 配置 ActFileLifeTime 阵列
	if ( m_AnimationActionInfoGroup.getAnimationACTCount() > 0 )
	{
		m_pACTFileInfo = n_new_array(stACTFILEINFO, m_AnimationActionInfoGroup.getAnimationACTCount());
		if ( m_pACTFileInfo )
		{
			memset(m_pACTFileInfo, 0, sizeof(stACTFILEINFO) * 
				m_AnimationActionInfoGroup.getAnimationACTCount());
		} // end of if (
	}

	// 音效新规格 SndTable
	{
		strFileName = m_strFilePath + szActionTableFileName;
		strFileName += ".stb";
		cSndTable sndTable;
		sndTable.LoadTable( (char*)strFileName.AsCharPtr(), &m_AnimationActionInfoGroup );
	} // end of 音效新规格

	// 读取事件表
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

	// 清除skeletomn hierarchy root
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
	// 读取ATF识别码
	// 读取版本
	stream->Read(ATFChar, sizeof(char)*3);
	ATFChar[3] = '\0';
	stream->Read(&m_iVersion, sizeof(int));

	if (lstrcmp(ATFChar,"ATF"))  // 检查ATF识别码
	{
		stream->Close();
		// ATF识别码不符
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

	//// 清除skeletomn hierarchy root
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


	//fread(ATFChar,3, 1, fp);        		// 读取ATF识别码
	//fread(&m_iVersion,sizeof(int), 1, fp);         // 读取版本

	//ATFChar[3] = '\0';

	//if (lstrcmp(ATFChar,"ATF"))  // 检查ATF识别码
	//{
	//	// ATF识别码不符
	//	LOGS(LoadAtfFile,"ATF识别码不符");

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
// Desc: Act Loading 流程简化
//-----------------------------------------------------------------------------
void cSkeletonSerializer::prepareAnimationKeys( int iActionIndex )
{
	//cAnimationActionKeyframesRange* pAnimationActionKeyframesRange = m_ActionTable.GetActTable( iActionIndex );
	cAnimationActionInfo* pAnimationActionInfo = m_AnimationActionInfoGroup.getAnimationActionInfo( iActionIndex );

	// Bug Fix:动作还在使用却被删除问题
	if ( (pAnimationActionInfo == NULL) || 
		(m_pAnimationRoot == NULL)     || 
		(m_pACTFileInfo == NULL) ) 
	{
		return;
	}
	//int iActID = pActTable->m_iActionIndex;
	unsigned int iActID = pAnimationActionInfo->getAnimationACTIndex();

	// 该档案不存在, 读取该档
	// Bug Fix:动作还在使用却被删除问题
	if ( m_pACTFileInfo[iActID].m_fLifeTime <= 0.0f )
	{
		cAnimationAction* pAnimationAction = n_new(cAnimationAction);

		// 无法再配罝记忆体, 离开函式...
		if ( pAnimationAction == NULL ) return;

		String strFileName;

		// 读取动作档
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

		// 更新Act Table资料
		int iCount = pAnimationAction->getAnimationActionInfoCount();

		// 这个ACT档里面只有 1 个动作, 代表这个动作有可能被共用, 
		// 玩家的动作规格, 是 1 个ACT档 1 个动作.
		// MOB, NPC 的动作规格, 是 1 个ACT档里有 20 个动作.
		// 有必要记录下 StartTime, EndTime, StartFrame, EndFrame 等资讯
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

	// 如果是单一动作, 就有可能共用, 就要注意时间资讯的设定
	if (m_pACTFileInfo[iActID].m_bSingle)
	{
		// 记下时间资讯
		pAnimationActionInfo->setAnimationActionStartTime(m_pACTFileInfo[iActID].m_dwStartTime);
		pAnimationActionInfo->setAnimationActionEndTime(m_pACTFileInfo[iActID].m_dwEndTime);
		pAnimationActionInfo->setAnimationActionStartKeyframe(m_pACTFileInfo[iActID].m_dwStartFrame);
		pAnimationActionInfo->setAnimationActionEndKeyframe(m_pACTFileInfo[iActID].m_dwEndFrame);
	} // end of if (m_pACTFileInfo[iActID].m_bSingle)

	// Bug Fix:动作还在使用却被删除问题
	m_pACTFileInfo[iActID].m_fLifeTime = ACTFILE_RESERVE_TIME;
}

}