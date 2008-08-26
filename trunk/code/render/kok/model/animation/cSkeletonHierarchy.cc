//------------------------------------------------------------------------------
//  cSkeletonHierarchy.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "cSkeletonHierarchy.h"
#include "kok/model/modeldef.h"

namespace KOK
{
using namespace Util;
using namespace IO;
using namespace Math;

//extern ID3DXMatrixStack *g_pStack;
ID3DXMatrixStack *g_pStack = NULL;

// cSkeletonHierarchy count
unsigned int cSkeletonHierarchy::m_uiSkeletonHierarchyCount = 0;
//cSkeletonHierarchy::SkeletonHierarchyLinkerKeyNameMap cSkeletonHierarchy::m_SkeletonHierarchyLinkerKeyNameMap;

// key name of skeleton hierarchy linker type
static string SkeletonHierarchyLinkerTypeKeyName[] = { "k_weapon", "k_magic" ,  "m_light" };

cSkeletonHierarchy::cSkeletonHierarchy()
{
	D3DXCreateMatrixStack(0, &g_pStack);

	m_pSibling = 0;
	m_pChild = 0;
	m_pParent = 0;
	m_pSkeletonTranslation = n_new(D3DXVECTOR3);
	m_pAnimation = 0;
	m_bNextFlag    = false;
	m_bChildFlag   = false;
	m_iFrameType = 0;
	D3DXMatrixIdentity(&m_matTransform);
	m_pSkeletonHierarchyMap = 0;
	m_pSkeletonHierarchyLinkerMaps = 0;
	//increate cSkeletonHierarchy count which news cSkeletonHierarchy.
	m_uiSkeletonHierarchyCount++;
}

cSkeletonHierarchy::~cSkeletonHierarchy()
{
	//delete skeleton hierachy map by root
	if( m_pParent == 0)
	{
		n_delete(m_pSkeletonHierarchyMap );
		n_delete_array( m_pSkeletonHierarchyLinkerMaps );
	}else{
		m_pSkeletonHierarchyMap = 0;
		m_pSkeletonHierarchyLinkerMaps = 0;
	}
	m_pParent = 0;
	m_pAnimation = 0;
	n_delete( m_pChild );
	n_delete( m_pSibling );
	n_delete( m_pSkeletonTranslation );
		//increate cSkeletonHierarchy count which delete cSkeletonHierarchy.
		m_uiSkeletonHierarchyCount--;
}

void
cSkeletonHierarchy::importFromMemory(const Ptr<Stream>& stream)
{
	int iTemp;
	String strTmp;

	// ��ȡSkeleton��������
	stream->Read(&iTemp, sizeof(int));
	// ��ȡSkeleton����
	if (iTemp > 0)
	{
		ReadString(stream, m_strName, iTemp);
		addSkeletinHierarcyIntoMap( getSkeletonHierarchyName() );
		addSkeletinHierarcyLinkerIntoMaps(  getSkeletonHierarchyName() );
	}

	// ��ȡSkeleton��ʽ
	stream->Read(&m_iFrameType, sizeof(int));
	// ��ȡƽ�ƾ�(TX)
	stream->Read(&m_pSkeletonTranslation->x, sizeof(float));
	// ��ȡƽ�ƾ�(TY)
	stream->Read(&m_pSkeletonTranslation->y, sizeof(float));
	// ��ȡƽ�ƾ�(TZ)
	stream->Read(&m_pSkeletonTranslation->z, sizeof(float));

	// ��ȡ�²��ж���
	stream->Read(&m_bNextFlag, sizeof(bool));

	stream->Read(&m_bChildFlag, sizeof(bool));

	if(m_bNextFlag) //
	{
		if(m_pSibling == NULL)
		{
			m_pSibling = n_new(cSkeletonHierarchy);
			m_pSibling->m_pParent = m_pParent;
			m_pSibling->importFromMemory(stream);
		}
	}

	if(m_bChildFlag)
	{
		if(m_pChild == NULL)
		{
			m_pChild = n_new(cSkeletonHierarchy);
			m_pChild->m_pParent = this;
			m_pChild->importFromMemory(stream);
		}
	}
}



//-----------------------------------------------------------------------------
// Name: importFromFile()
// Desc:  ��file����Skeleton Hierarchy
//-----------------------------------------------------------------------------
void
cSkeletonHierarchy::importFromFile(FILE *fp)
{
	//if(!fp) return -1;

	//int iTemp;
	//char *cTempChar  = NULL;

	//// ��ȡSkeleton��������
	//fread(&iTemp,sizeof(int), 1, fp);

	//// ��ȡSkeleton����
	//if (iTemp > 0)
	//{
	//  char* szName  = _NEW char[iTemp];
	//  fread(szName, sizeof(char)*iTemp, 1, fp);
	//  setSkeletonHierarchyName(szName, iTemp);
	////jingjie modified 2006.06.28
	//addSkeletinHierarcyIntoMap( getSkeletonHierarchyName());
	////jingjie modified 2006.06.29
	//addSkeletinHierarcyLinkerIntoMaps(  getSkeletonHierarchyName() );
	//  SAFE_DELETE_ARRAY(szName);
	//}

	//// ��ȡSkeleton��ʽ
	//fread(&m_iFrameType,sizeof(int), 1, fp);
	//// ��ȡƽ�ƾ���(TX)
	//fread(&m_pSkeletonTranslation->x,sizeof(float), 1, fp);
	//// ��ȡƽ�ƾ���(TY)
	//fread(&m_pSkeletonTranslation->y,sizeof(float), 1, fp);
	//// ��ȡƽ�ƾ���(TZ)
	//fread(&m_pSkeletonTranslation->z,sizeof(float), 1, fp);

	//// ��ȡ�²��ж���
	//fread(&m_bNextFlag,  sizeof(bool), 1, fp);
	//fread(&m_bChildFlag, sizeof(bool), 1, fp);

	//if(m_bNextFlag) 
	//{
	//  if(m_pSibling == NULL)
	//  {
	//      m_pSibling = _NEW cSkeletonHierarchy();
	//		m_pSibling->m_pParent = m_pParent;
	//      m_pSibling->importFromFile(fp);
	//  }
	//}

	//if(m_bChildFlag)
	//{
	//  if(m_pChild == NULL)
	//  {
	//		//
	//      m_pChild = _NEW cSkeletonHierarchy();
	//		m_pChild->m_pParent = this;
	//      m_pChild->importFromFile(fp);
	//  }
	//}
}

//-----------------------------------------------------------------------------
// Name: FrameMoveByTime()
// Desc: ��timebase,curtime������Ŀǰ������mtx
//-----------------------------------------------------------------------------
#ifdef _CHECK_NOUSE_ANIMATION
void 
	cSkeletonHierarchy::computeSkeletonHierarchyMatrix( cAnimationActionInfoGroup *pAnimationActionInfoGroup, 
	const sComputeAnimParam& param,
	SkeletonHierarchyUsabilityMap* pSkeletonHierarchyUnUsedMap )
#else
void cSkeletonHierarchy::computeSkeletonHierarchyMatrix( cAnimationActionInfoGroup *pAnimationActionInfoGroup, 
	const sComputeAnimParam& param,
	float fLerpValue )
#endif
{

#ifdef _CHECK_NOUSE_ANIMATION
	if( pSkeletonHierarchyUnUsedMap && pAnimationActionInfoGroup)
	{
		SkeletonHierarchyUsabilityMap::iterator iter = pSkeletonHierarchyUnUsedMap->find( this );
		if( iter != pSkeletonHierarchyUnUsedMap->end() )
		{
			if( m_pSibling )
			{
				((cSkeletonHierarchy*)m_pSibling)->computeSkeletonHierarchyMatrix( pAnimationActionInfoGroup,
							param,
							pSkeletonHierarchyUnUsedMap );
			}
			return;
		}
	}
#endif

	//D3DXMATRIXA16 *pCurnMatrix = NULL;
	D3DXMATRIXA16 matTemp;

	// ȡ�õ�λ����
	D3DXMatrixIdentity(&matTemp);

	n_assert( pAnimationActionInfoGroup );
	if (m_pAnimation && pAnimationActionInfoGroup)
	{
		n_assert( m_pAnimation );
		m_pAnimation->getAnimationMatrix(&matTemp, pAnimationActionInfoGroup, param);
	} // end of if (m_pAnimation)

	g_pStack->Push();
	g_pStack->MultMatrixLocal(&matTemp);

	//pCurnMatrix = g_pStack->GetTop();
	//m_matTransform = (*pCurnMatrix);
	m_matTransform = *g_pStack->GetTop();


	/*if (m_pAnimation)
	{
	m_pAnimation->m_mxMatrix = (*pCurnMatrix);
	}*/

	// child's mtx
	if (m_pChild)
	{
		g_pStack->Push();

#ifdef _CHECK_NOUSE_ANIMATION
		((cSkeletonHierarchy*)m_pChild)->computeSkeletonHierarchyMatrix( pAnimationActionInfoGroup,
						 param,
						 pSkeletonHierarchyUnUsedMap );
#else
		((cSkeletonHierarchy*)m_pChild)->computeSkeletonHierarchyMatrix( pAnimationActionInfoGroup,
							param );
#endif

		g_pStack->Pop();
	} // end of if (m_pChild)

	//  if (m_pAnimation)
	//  {
	g_pStack->Pop();
	//  }

	// next's mtx
	if (m_pSibling)
	{
#ifdef _CHECK_NOUSE_ANIMATION
		((cSkeletonHierarchy*)m_pSibling)->computeSkeletonHierarchyMatrix( pAnimationActionInfoGroup,
							param,
							pSkeletonHierarchyUnUsedMap );
#else
		((cSkeletonHierarchy*)m_pSibling)->computeSkeletonHierarchyMatrix( pAnimationActionInfoGroup,
							param );
#endif
	}
}

//-----------------------------------------------------------------------------
// Name: getSkeletonName
// Desc:
//-----------------------------------------------------------------------------
char*	
	cSkeletonHierarchy::getSkeletonHierarchyName() { return (char*)m_strName.AsCharPtr(); 
}

//-----------------------------------------------------------------------------
// Name: hashIt
// Desc:
//-----------------------------------------------------------------------------
/*DWORD cSkeletonHierarchy::hashIt(char* pStr)
{
DWORD dwHashID = 0;
for (; *pStr ;)
{
dwHashID = dwHashID << 1 ^ toupper( *pStr++ );
}

return dwHashID;
}*/
//-----------------------------------------------------------------------------
// Name: getChild
// Desc:  Skeleton's child
//-----------------------------------------------------------------------------
cSkeletonHierarchy*
	cSkeletonHierarchy::getChild()
{
	return m_pChild;
}
//-----------------------------------------------------------------------------
// Name: getChild
// Desc:  Skeleton's parent
//-----------------------------------------------------------------------------
cSkeletonHierarchy*
	cSkeletonHierarchy::getParent()
{
	return m_pParent;
}
//-----------------------------------------------------------------------------
// Name: getChild
// Desc:  ͬһlevel Skeleton
//-----------------------------------------------------------------------------
cSkeletonHierarchy*
	cSkeletonHierarchy::getSibling()
{
	return m_pSibling;
}
//-----------------------------------------------------------------------------
// Name: getAnimation
// Desc: 
//-----------------------------------------------------------------------------
cAnimation* 
	cSkeletonHierarchy::getAnimation()
{
	return m_pAnimation;
}
//-----------------------------------------------------------------------------
// Name: setAnimation
// Desc: 
//-----------------------------------------------------------------------------
void 
	cSkeletonHierarchy::setAnimation(cAnimation* pAnimation)
{
	m_pAnimation = pAnimation;
}
//-----------------------------------------------------------------------------
// Name: getSkeletonHierarchyMatrix
// Desc: ���SkeletonĿǰ�ļ���animation��ľ���
//-----------------------------------------------------------------------------
//D3DXMATRIXA16* cSkeletonHierarchy::getSkeletonHierarchyMatrix()
//{
//	return &m_matTransform;
//}
D3DXMATRIXA16* 
	cSkeletonHierarchy::getSkeletonHierarchyMatrix()
{
	return &m_matTransform;
}


//-----------------------------------------------------------------------------
// Name: findSkeletonHierarchy()
// Desc: Find skeleton By Name
//-----------------------------------------------------------------------------
cSkeletonHierarchy* 
	cSkeletonHierarchy::findSkeletonHierarchy(char* name)
{
	cSkeletonHierarchy *pSkeletonHierarchy = NULL;
	if (name == NULL) return NULL;
	if (*name == '\0') return NULL;
	if( m_pSkeletonHierarchyMap )
	{
		SkeletonHierarchyMap::iterator i = m_pSkeletonHierarchyMap->find( name );

		if( i != m_pSkeletonHierarchyMap->end() )
		{
			return i->second;
		}else return 0;
	}/*else{
	 if (lstrcmp(name, m_strName.c_str()) == 0) return this;

	 if (m_pSibling)
	 {
	 pSkeletonHierarchy = m_pSibling->findSkeletonHierarchy
	 (name);
	 if (pSkeletonHierarchy) return pSkeletonHierarchy;
	 } // end of if (m_pSibling)

	 if (m_pChild)
	 {
	 pSkeletonHierarchy = m_pChild->findSkeletonHierarchy
	 (name);
	 if (pSkeletonHierarchy) return pSkeletonHierarchy;
	 } // end of if (m_pChild)

	 }*/

	return 0;
}

//-----------------------------------------------------------------------------
// Name: findSkeletonHierarchyWithHash()
// Desc: Find skeleton Hierarchy By hashid
//-----------------------------------------------------------------------------
/*cSkeletonHierarchy* cSkeletonHierarchy::findSkeletonHierarchyWithHash(char* 
name, DWORD hashID)
{
cSkeletonHierarchy *pSkeletonHierarchy = NULL;
if (name == NULL) return NULL;
if (*name == '\0') return NULL;

// �ȱȶ� hash ID, �ٱȶ��ִ���.
if (m_dwHashID == hashID)
{
if (lstrcmp(name, m_strName.c_str()) == 0) return this;
} // end of if (m_dwHashID == hashID)

if (m_pSibling)
{
pSkeletonHierarchy = m_pSibling->findSkeletonHierarchyWithHash
(name, hashID);
if (pSkeletonHierarchy) return pSkeletonHierarchy;
} // end of if (m_pSibling)

if (m_pChild)
{
pSkeletonHierarchy = m_pChild->findSkeletonHierarchyWithHash
(name, hashID);
if (pSkeletonHierarchy) return pSkeletonHierarchy;
} // end of if (m_pChild)

return NULL;
}*/
//-----------------------------------------------------------------------------
// Name: addSkeletinHierarcyIntoMap()
// Desc: jingjie modified 2006.06.28 ��skeleton hierarchy����map,������ѯʹ��.
//-----------------------------------------------------------------------------
void 
	cSkeletonHierarchy::addSkeletinHierarcyIntoMap( char* szSkeletonHierarchyName )
{
	if( m_pParent == 0 && m_pSkeletonHierarchyMap == 0)
	{
		m_pSkeletonHierarchyMap = n_new(SkeletonHierarchyMap);
	}else{
		m_pSkeletonHierarchyMap = m_pParent->getSkeletonHierarchyMap();
	}
	if( m_pSkeletonHierarchyMap )
	{
		SkeletonHierarchyMap::iterator i = m_pSkeletonHierarchyMap->find( szSkeletonHierarchyName );

		if( i == m_pSkeletonHierarchyMap->end() )
		{
			m_pSkeletonHierarchyMap->insert
				(SkeletonHierarchyMap::value_type( szSkeletonHierarchyName, this) );
		}
	}
}
//-----------------------------------------------------------------------------
// Name: removeSkeletinHierarcyFromMap()
// Desc: jingjie modified 2006.06.28 ��skeleton hierarchy��mapɾ��.
//-----------------------------------------------------------------------------
void 
	cSkeletonHierarchy::removeSkeletinHierarcyFromMap( char* szSkeletonHierarchyName)
{
	if( m_pSkeletonHierarchyMap )
	{
		SkeletonHierarchyMap::iterator i = m_pSkeletonHierarchyMap->find( szSkeletonHierarchyName );

		if( i != m_pSkeletonHierarchyMap->end() )
		{
			m_pSkeletonHierarchyMap->erase(i);
		}
	}
}
//-----------------------------------------------------------------------------
// Name: getSkeletonHierarchyMap()
// Desc: jingjie modified 2006.06.28 get SkeletonHierarchyMap ptr.
//-----------------------------------------------------------------------------
cSkeletonHierarchy::SkeletonHierarchyMap* cSkeletonHierarchy::getSkeletonHierarchyMap()
{
	return m_pSkeletonHierarchyMap;
}
//-----------------------------------------------------------------------------
// Name: getSkeletonHierarchyMap()
// Desc: jingjie modified 2006.06.29 add linker into skeleton hierarchy maps with linker type.
//-----------------------------------------------------------------------------
void 
	cSkeletonHierarchy::addSkeletinHierarcyLinkerIntoMaps( char* szSkeletonHierarchyName )
{
	if( m_pParent == 0 && m_pSkeletonHierarchyLinkerMaps == 0)
	{
		m_pSkeletonHierarchyLinkerMaps = n_new_array(SkeletonHierarchyMap, SkeletonHierarchyLinkerCount);
	}else{
		m_pSkeletonHierarchyLinkerMaps = m_pParent->getSkeletonHierarchyLinkerMaps();
	}
	if( m_pSkeletonHierarchyLinkerMaps )
	{
		for( unsigned int j = 0 ; j < SkeletonHierarchyLinkerCount ; j++ )
		{
			if( strstr( szSkeletonHierarchyName , SkeletonHierarchyLinkerTypeKeyName[j].c_str() ) != 0 )
			{	
				SkeletonHierarchyMap::iterator i = m_pSkeletonHierarchyLinkerMaps[j].find( szSkeletonHierarchyName );
				if( i == m_pSkeletonHierarchyLinkerMaps[j].end() )
				{
					m_pSkeletonHierarchyLinkerMaps[j].insert(SkeletonHierarchyMap::value_type( szSkeletonHierarchyName, this) );
				}
			}		
		}
	}
}
//-----------------------------------------------------------------------------
// Name: getSkeletonHierarchyMap()
// Desc: jingjie modified 2006.06.29 get SkeletonHierarchyLinkerMaps ptr
//-----------------------------------------------------------------------------
cSkeletonHierarchy::SkeletonHierarchyMap* cSkeletonHierarchy::getSkeletonHierarchyLinkerMaps(cSkeletonHierarchy::SkeletonHierarchyLinkerType skeletonHierarchyLinkerType)
{
	return &m_pSkeletonHierarchyLinkerMaps[ skeletonHierarchyLinkerType ];
}
}
