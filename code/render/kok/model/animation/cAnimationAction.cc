//------------------------------------------------------------------------------
//  cAnimationAction.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "cAnimationAction.h"
#include "kok/util/cECReader.h"
#include "io/ioserver.h"
#include "io/stream.h"

namespace KOK
{

	using namespace IO;
	using namespace Util;

//jingjie modified 2006.07.10 cAnimationKeys count
unsigned int cAnimationKeys::s_uiAnimationKeysCountInGame = 0;
//-----------------------------------------------------------------------------
// Name: cAnimationKeys()
// Desc: cAnimationKeys ���Ľ���Ԫ
//-----------------------------------------------------------------------------
cAnimationKeys::cAnimationKeys()
{
	m_dwSkeletonNameLength = 0;
	m_szSkeletonName = NULL;
	m_aPositionKey  = NULL;
	m_aRotateKey   = NULL;
	m_aScaleKey    = NULL;

	//m_pAnimCache    = NULL;

	m_dwNumPositionKey = 0 ;
	m_dwNumRotateKey   = 0 ;
	m_dwNumScaleKey    = 0 ;
	//jingjie modified 2006.07.06 matrix of keyframe not in use?????
	//D3DXMatrixIdentity(&m_matMatrix);
	//jingjie modified 2006.07.10 cAnimationKeys count
	s_uiAnimationKeysCountInGame++;
}

//-----------------------------------------------------------------------------
// Name: ~cAnimationKeys()
// Desc: cAnimationKeys ���Ľ⹹Ԫ
//-----------------------------------------------------------------------------
cAnimationKeys::~cAnimationKeys()
{
	n_delete_array( m_szSkeletonName );

	n_delete_array( m_aPositionKey );
	n_delete_array( m_aRotateKey );
	n_delete_array( m_aScaleKey );
	//SAFE_DELETE( m_pAnimCache );
	//jingjie modified 2006.07.10 cAnimationKeys count
	s_uiAnimationKeysCountInGame--;
}
//-----------------------------------------------------------------------------
// Name: getSkeletonHierarchyName()
// Desc: jingjie modified 2006.07.06 animation action��Ӧ�Ĺ�ͷ��
//-----------------------------------------------------------------------------
char* cAnimationKeys::getSkeletonHierarchyName()
{
	return m_szSkeletonName;
}
//-----------------------------------------------------------------------------
// Name: getPositionKey()
// Desc: jingjie modified 2006.07.06 get position of animation action by keyframe index
//-----------------------------------------------------------------------------
cAnimationKeys::stPOSITIONKEY* cAnimationKeys::getPositionKey( unsigned int uiKeyframeIndex )
{
	if( uiKeyframeIndex >= 0 && uiKeyframeIndex < m_dwNumPositionKey )
	{
		return &m_aPositionKey[uiKeyframeIndex];
	}else{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: clonePositionKey()
// Desc: jingjie modified 2006.07.06 set position keyframe ptr of animation action
//-----------------------------------------------------------------------------
void cAnimationKeys::clonePositionKey( cAnimationKeys::stPOSITIONKEY* pPositionKey, DWORD dwNumPositionKey )
{
	if( pPositionKey != 0 && dwNumPositionKey > 0)  
	{
		n_delete_array( m_aPositionKey );
		m_dwNumPositionKey = dwNumPositionKey;
		m_aPositionKey = n_new_array(cAnimationKeys::stPOSITIONKEY, m_dwNumPositionKey);
		memcpy( m_aPositionKey, pPositionKey, m_dwNumPositionKey * sizeof(cAnimationKeys::stPOSITIONKEY));
	}
}

//-----------------------------------------------------------------------------
// Name: getRotateKey()
// Desc: jingjie modified 2006.07.06 get rotation of animation action by keyframe index
//-----------------------------------------------------------------------------
cAnimationKeys::stROTATEKEY* cAnimationKeys::getRotateKey( unsigned int uiKeyframeIndex )
{
	if( uiKeyframeIndex >= 0 && uiKeyframeIndex < m_dwNumRotateKey )
	{
		return &m_aRotateKey[uiKeyframeIndex];
	}else{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: cloneRotateKey()
// Desc: jingjie modified 2006.07.06 clone rotation keyframe of animation action
//-----------------------------------------------------------------------------
void cAnimationKeys::cloneRotateKey(cAnimationKeys::stROTATEKEY* pRotateKey, DWORD dwNumRotateKey)
{
	if( pRotateKey != 0 && dwNumRotateKey > 0)  
	{
		n_delete_array( m_aRotateKey );
		m_dwNumRotateKey = dwNumRotateKey;
		m_aRotateKey = n_new_array(cAnimationKeys::stROTATEKEY, m_dwNumRotateKey);
		memcpy( m_aRotateKey, pRotateKey, m_dwNumRotateKey * sizeof(cAnimationKeys::stROTATEKEY));
	}
}
//-----------------------------------------------------------------------------
// Name: getScaleKey()
// Desc: jingjie modified 2006.07.06 clone scale keyframe of animation action
//-----------------------------------------------------------------------------
cAnimationKeys::stSCALEKEY* cAnimationKeys::getScaleKey(unsigned int uiKeyframeIndex )
{
	if( uiKeyframeIndex >= 0 && uiKeyframeIndex < m_dwNumScaleKey )
	{
		return &m_aScaleKey[uiKeyframeIndex];
	}else{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: cloneScaleKey()
// Desc: jingjie modified 2006.07.06 clone scale keyframe of animation action
//-----------------------------------------------------------------------------
void cAnimationKeys::cloneScaleKey(cAnimationKeys::stSCALEKEY* pScaleKey, DWORD 
								   dwNumScaleKey)
{
	if( pScaleKey != 0 && dwNumScaleKey > 0)  
	{
		n_delete_array( m_aScaleKey );
		m_dwNumScaleKey = dwNumScaleKey;
		m_aScaleKey = n_new_array(cAnimationKeys::stSCALEKEY, m_dwNumRotateKey);
		memcpy( m_aScaleKey, pScaleKey, m_dwNumScaleKey * sizeof(cAnimationKeys::stSCALEKEY));
	}
}
//-----------------------------------------------------------------------------
// Name: cloneAnimationKeys()
// Desc: jingjie modified 2006.07.06 clone keyframe data of animation action
//-----------------------------------------------------------------------------
void cAnimationKeys::cloneAnimationKeys( cAnimationKeys* pAnimationKeys)
{
	if( pAnimationKeys )
	{
		n_delete_array( m_szSkeletonName );
		m_dwSkeletonNameLength = strlen(pAnimationKeys->getSkeletonHierarchyName());
		m_szSkeletonName = n_new_array(char, m_dwSkeletonNameLength + 1);
		memcpy(m_szSkeletonName, pAnimationKeys->getSkeletonHierarchyName() , sizeof(char) * m_dwSkeletonNameLength);
		m_szSkeletonName[m_dwSkeletonNameLength] = '\0';
		clonePositionKey( pAnimationKeys->getPositionKey(0),pAnimationKeys->getNumPositionKey() );
		cloneRotateKey( pAnimationKeys->getRotateKey(0),pAnimationKeys->getNumRotateKey() );
		cloneScaleKey( pAnimationKeys->getScaleKey(0),pAnimationKeys->getNumScaleKey() );
	}
}
//-----------------------------------------------------------------------------
// Name: importAnimationKeysFromMemory()
// Desc: jingjie modified 2006.07.06 import AnimationKeys from memory
//-----------------------------------------------------------------------------
void cAnimationKeys::importAnimationKeysFromMemory(const Ptr<Stream>& stream, bool bNameOnly)
{
	// ��ȡ��Ӧ�Ĺ�ͷ���Ƴ���
	stream->Read(&m_dwSkeletonNameLength, sizeof(DWORD));
	if (m_dwSkeletonNameLength > 0)
	{
		// ��ȡ��Ӧ�Ĺ�ͷ����
		n_delete_array( m_szSkeletonName );
		m_szSkeletonName = n_new_array(char, m_dwSkeletonNameLength + 1);
		stream->Read(m_szSkeletonName, sizeof(char) * m_dwSkeletonNameLength);
		m_szSkeletonName[m_dwSkeletonNameLength] = '\0';
	} // end of if (m_dwNameLength > 0)

	if (bNameOnly == false)
	{
		// matrix Offset 4*4
		//jingjie modified 2006.07.06 matrix of keyframe not in use?????
			//memcpy(m_matMatrix, Buffer, sizeof(D3DXMATRIX) );
			stream->Seek(sizeof(D3DXMATRIX), Stream::Current);

		// ��ȡm_pPositionKeys ������
			stream->Read(&m_dwNumPositionKey, sizeof(DWORD));
		n_delete_array( m_aPositionKey );
		if (m_dwNumPositionKey > 0)
		{
			m_aPositionKey = n_new_array(stPOSITIONKEY, m_dwNumPositionKey);
			// ��ȡm_pPositionKeys ������
			stream->Read(m_aPositionKey, m_dwNumPositionKey * sizeof(stPOSITIONKEY));
		} // end of if (m_dwPositionKeys > 0)

		// ��ȡm_pRotateKeys ������
		stream->Read(&m_dwNumRotateKey, sizeof(DWORD));
		n_delete_array( m_aRotateKey );
		if (m_dwNumRotateKey > 0)
		{
			m_aRotateKey = n_new_array(stROTATEKEY, m_dwNumRotateKey);
			// ��ȡm_pRotateKeys ������
			stream->Read(m_aRotateKey, m_dwNumRotateKey*sizeof(stROTATEKEY));
		} // end of if (m_dwRotateKeys > 0)

		// ��ȡm_pScaleKeys ������
		stream->Read(&m_dwNumScaleKey, sizeof(DWORD));
		n_delete_array( m_aScaleKey );
		if(m_dwNumScaleKey > 0)
		{
			m_aScaleKey = n_new_array(stSCALEKEY, m_dwNumScaleKey);

			// ��ȡm_pScaleKeys ������
			stream->Read(m_aScaleKey, m_dwNumScaleKey*sizeof(stSCALEKEY));
		}
	} 
	else 
	{ // ����ȡ FrameName
		// matrix Offset 4*4
		stream->Seek(sizeof(D3DXMATRIX), Stream::Current);

		// ��ȡm_pPositionKeys ������
		stream->Read(&m_dwNumPositionKey, sizeof(DWORD));
		if (m_dwNumPositionKey > 0)
		{
			stream->Seek(m_dwNumPositionKey * sizeof(stPOSITIONKEY), Stream::Current);
		} // end of if (m_dwPositionKeys > 0)

		// ��ȡm_pRotateKeys ������
		stream->Read(&m_dwNumRotateKey, sizeof(DWORD));
		if (m_dwNumRotateKey > 0)
		{
			stream->Seek(m_dwNumRotateKey*sizeof(stROTATEKEY), Stream::Current);
		} // end of if (m_dwRotateKeys > 0)

		// ��ȡm_pScaleKeys ������
		stream->Read(&m_dwNumScaleKey, sizeof(DWORD));
		if (m_dwNumScaleKey > 0)
		{
			stream->Seek(m_dwNumScaleKey*sizeof(stSCALEKEY), Stream::Current);
		}
	} 
}

//-----------------------------------------------------------------------------
// Name: exportAnimationKeysToFile()
// Desc:jingjie modified 2006.07.06 export AnimationKeys into file
//-----------------------------------------------------------------------------
//int cAnimationKeys::exportAnimationKeysToFile(FILE *fp)
//{
//	DWORD i;
//	BYTE *pData;	  //buffer pointer
//	if(!fp) return -1;
//
//	// д���Ӧ�Ĺ�ͷ���Ƴ���
//	pData =(BYTE *)&m_dwSkeletonNameLength;
//	fwrite(pData,sizeof(int),1,fp);
//
//	// д���Ӧ�Ĺ�ͷ����
//	pData =(BYTE *)m_szSkeletonName;
//	fwrite(pData,sizeof(char)*m_dwSkeletonNameLength,1,fp);
//	//jingjie modified 2006.07.06 matrix of keyframe not in use?????
//	D3DXMATRIXA16 matMatrix;
//	D3DXMatrixIdentity(&matMatrix);
//	// matrix Offset 4*4
//	pData =(BYTE *)&matMatrix._11 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._12 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._13 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._14 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	pData =(BYTE *)&matMatrix._21 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._22 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._23 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._24 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	pData =(BYTE *)&matMatrix._31 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._32 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._33 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._34 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	pData =(BYTE *)&matMatrix._41 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._42 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._43 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&matMatrix._44 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	// д��m_pPositionKeys ������
//	pData =(BYTE *)&m_dwNumPositionKey;
//	fwrite(pData,sizeof(DWORD),1,fp);
//	// д��m_pPositionKeys ������
//	for(i=0;i<m_dwNumPositionKey;i++)
//	{
//		pData =(BYTE *)&m_aPositionKey[i].vPos.x ;
//		fwrite(pData,sizeof(float),1,fp);
//		pData =(BYTE *)&m_aPositionKey[i].vPos.y ;
//		fwrite(pData,sizeof(float),1,fp);
//		pData =(BYTE *)&m_aPositionKey[i].vPos.z ;
//		fwrite(pData,sizeof(float),1,fp);
//	}
//
//	// д��m_pRotateKeys ������
//	pData =(BYTE *)&m_dwNumRotateKey;
//	fwrite(pData,sizeof(DWORD),1,fp);
//	// д��m_pRotateKeys ������
//	for(i=0;i<m_dwNumRotateKey;i++)
//	{
//		pData =(BYTE *)&m_aRotateKey[i].quatRotate.x ;
//		fwrite(pData,sizeof(float),1,fp);
//		pData =(BYTE *)&m_aRotateKey[i].quatRotate.y ;
//		fwrite(pData,sizeof(float),1,fp);
//		pData =(BYTE *)&m_aRotateKey[i].quatRotate.z ;
//		fwrite(pData,sizeof(float),1,fp);
//		pData =(BYTE *)&m_aRotateKey[i].quatRotate.w ;
//		fwrite(pData,sizeof(float),1,fp);
//	}
//
//	// д��m_pScaleKeys ������
//	pData =(BYTE *)&m_dwNumScaleKey;
//	fwrite(pData,sizeof(DWORD),1,fp);
//	// д��m_pScaleKeys ������
//	for(i=0;i<m_dwNumScaleKey;i++)
//	{
//		pData =(BYTE *)&m_aScaleKey[i].vScale.x ;
//		fwrite(pData,sizeof(float),1,fp);
//		pData =(BYTE *)&m_aScaleKey[i].vScale.y ;
//		fwrite(pData,sizeof(float),1,fp);
//		pData =(BYTE *)&m_aScaleKey[i].vScale.z ;
//		fwrite(pData,sizeof(float),1,fp);
//	}
//
//	return 0;
//}

//-----------------------------------------------------------------------------
// Name: importAnimationKeysFromFile()
// Desc: jingjie modified 2006.07.06 import AnimationKeys from file
//-----------------------------------------------------------------------------
//int cAnimationKeys::importAnimationKeysFromFile(FILE *fp)
//{
//	DWORD i;
//
//	if(!fp) return -1;
//
//	// ��ȡ��Ӧ�Ĺ�ͷ���Ƴ���
//	fread(&m_dwSkeletonNameLength,sizeof(DWORD), 1, fp);
//
//	if (m_dwSkeletonNameLength > 0)
//	{
//		// ��ȡ��Ӧ�Ĺ�ͷ����
//		SAFE_DELETE_ARRAY( m_szSkeletonName );
//		m_szSkeletonName = n_new(char, m_dwSkeletonNameLength+1);
//		ZeroMemory(m_szSkeletonName , sizeof(char)*(m_dwSkeletonNameLength+1));
//		fread(m_szSkeletonName,sizeof(char)*m_dwSkeletonNameLength, 1, fp);
//		m_szSkeletonName[m_dwSkeletonNameLength] = '\0';
//	}
//
//	// matrix Offset 4*4
//	//jingjie modified 2006.07.06 matrix of keyframe not in use?????
//	D3DXMATRIXA16 matMatrix;
//	fread(&matMatrix._11,sizeof(float),1,fp);
//	fread(&matMatrix._12,sizeof(float),1,fp);
//	fread(&matMatrix._13,sizeof(float),1,fp);
//	fread(&matMatrix._14,sizeof(float),1,fp);
//
//	fread(&matMatrix._21,sizeof(float),1,fp);
//	fread(&matMatrix._22,sizeof(float),1,fp);
//	fread(&matMatrix._23,sizeof(float),1,fp);
//	fread(&matMatrix._24,sizeof(float),1,fp);
//
//	fread(&matMatrix._31,sizeof(float),1,fp);
//	fread(&matMatrix._32,sizeof(float),1,fp);
//	fread(&matMatrix._33,sizeof(float),1,fp);
//	fread(&matMatrix._34,sizeof(float),1,fp);
//
//	fread(&matMatrix._41,sizeof(float),1,fp);
//	fread(&matMatrix._42,sizeof(float),1,fp);
//	fread(&matMatrix._43,sizeof(float),1,fp);
//	fread(&matMatrix._44,sizeof(float),1,fp);
//
//	// ��ȡm_pPositionKeys ������
//	fread(&m_dwNumPositionKey,sizeof(DWORD),1,fp);
//
//	SAFE_DELETE_ARRAY( m_aPositionKey );
//	if(m_dwNumPositionKey > 0)
//	{
//		m_aPositionKey = n_new(stPOSITIONKEY, m_dwNumPositionKey);
//
//		// ��ȡm_pPositionKeys ������
//		for(i=0;i<m_dwNumPositionKey;i++)
//		{
//			fread(&m_aPositionKey[i].dwTime,sizeof(DWORD),1,fp);
//			fread(&m_aPositionKey[i].vPos.x,sizeof(float),1,fp);
//			fread(&m_aPositionKey[i].vPos.y,sizeof(float),1,fp);
//			fread(&m_aPositionKey[i].vPos.z,sizeof(float),1,fp);
//		}
//	}
//
//	// ��ȡm_pRotateKeys ������
//	fread(&m_dwNumRotateKey,sizeof(DWORD),1,fp);
//
//	SAFE_DELETE_ARRAY( m_aRotateKey );
//	if(m_dwNumRotateKey > 0)
//	{
//		m_aRotateKey = n_new(stROTATEKEY, m_dwNumRotateKey);
//
//		// ��ȡm_pRotateKeys ������
//		for(i=0;i<m_dwNumRotateKey;i++)
//		{
//			fread(&m_aRotateKey[i].dwTime,sizeof(DWORD),1,fp);
//			fread(&m_aRotateKey[i].quatRotate.x,sizeof(float),1,fp);
//			fread(&m_aRotateKey[i].quatRotate.y,sizeof(float),1,fp);
//			fread(&m_aRotateKey[i].quatRotate.z,sizeof(float),1,fp);
//			fread(&m_aRotateKey[i].quatRotate.w,sizeof(float),1,fp);
//		}
//	}
//
//	// ��ȡm_pScaleKeys ������
//	fread(&m_dwNumScaleKey,sizeof(DWORD),1,fp);
//
//	SAFE_DELETE_ARRAY( m_aScaleKey );
//	if (m_dwNumScaleKey > 0)
//	{
//		m_aScaleKey = n_new(stSCALEKEY, m_dwNumScaleKey);
//
//		// ��ȡm_pScaleKeys ������
//		for(i=0;i<m_dwNumScaleKey;i++)
//		{
//			fread(&m_aScaleKey[i].dwTime,sizeof(DWORD),1,fp);
//			fread(&m_aScaleKey[i].vScale.x,sizeof(float),1,fp);
//			fread(&m_aScaleKey[i].vScale.y,sizeof(float),1,fp);
//			fread(&m_aScaleKey[i].vScale.z,sizeof(float),1,fp);
//		}
//	}
//
//	return 0;
//}
//jingjie modified 2006.07.10 cAnimationActionInfo count
unsigned int cAnimationActionInfo::s_uiAnimationActionInfoCountInGame = 0;
//-----------------------------------------------------------------------------
// Name: cAnimationActionInfo()
// Desc: 
//-----------------------------------------------------------------------------
cAnimationActionInfo::cAnimationActionInfo()
{
	m_szSoundName = n_new(char);
	m_szSoundName[0] = '\0';
	m_dwSoundDelayTimems = 0;
	m_uiAnimationACTIndex = 0;
	m_dwStartKeyframe = 0;
	m_dwEndKeyframe   = 0;
	m_dwStartTime = 0;
	m_dwEndTime   = 0;

	// 080109 cyhsieh �����¼�
	m_pHurtEventList.Clear();

	//jingjie modified 2006.07.10 cAnimationActionInfo count
	s_uiAnimationActionInfoCountInGame++;

};
//-----------------------------------------------------------------------------
// Name: ~cAnimationActionInfo()
// Desc: 
//-----------------------------------------------------------------------------
cAnimationActionInfo::~cAnimationActionInfo()
{
	//jingjie modified 2006.07.10 cAnimationActionInfo count
	s_uiAnimationActionInfoCountInGame--;

	// 080109 cyhsieh �����¼�
	if( m_pHurtEventList.Size() > 0 )
	{
		int iCount = m_pHurtEventList.Size() ;
		for( int i = 0; i < iCount; i++ )
		{
			n_delete(m_pHurtEventList[i]);
		}
		m_pHurtEventList.Clear();
	}

	n_delete_array( m_szSoundName );
}
//-----------------------------------------------------------------------------
// Name: setSoundName
// Desc: jingjie modified 2006.07.08 set sound name of this animation action.
//-----------------------------------------------------------------------------
void cAnimationActionInfo::setSoundName( char* name , int len )
{
	if( name == 0 ) return;
	if (name != 0 && len <= -1 ) 
	{
		len = strlen( name );
	}
	n_delete_array( m_szSoundName );
	m_szSoundName = n_new_array(char, len + 5);
	memcpy(m_szSoundName, name, len);
	memcpy(m_szSoundName + len, ".wav", 4);
	m_szSoundName[len + 4] = '\0';
}

void cAnimationActionInfo::setSoundDelayTimems(DWORD iDelayTimems)
{
	m_dwSoundDelayTimems = iDelayTimems;
}

//-----------------------------------------------------------------------------
// Name: getSoundName
// Desc: jingjie modified 2006.07.08 get sound name of this animation action.
//-----------------------------------------------------------------------------
char* cAnimationActionInfo::getSoundName( )
{
	return m_szSoundName;
}
//-----------------------------------------------------------------------------
// Name: setAnimationActionIndex
// Desc: jingjie modified 2006.07.08 set index of animation action
//-----------------------------------------------------------------------------
void cAnimationActionInfo::setAnimationACTIndex(unsigned int uiAnimationACTIndex)
{
	m_uiAnimationACTIndex = uiAnimationACTIndex;
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionIndex
// Desc: jingjie modified 2006.07.08 get index of animation action
//-----------------------------------------------------------------------------
unsigned int cAnimationActionInfo::getAnimationACTIndex( )
{
	return m_uiAnimationACTIndex;
}
//-----------------------------------------------------------------------------
// Name: setAnimationActionStartKeyframe
// Desc: jingjie modified 2006.07.08 set start keyframe of animation action
//-----------------------------------------------------------------------------
void cAnimationActionInfo::setAnimationActionStartKeyframe( DWORD dwStartkeyframe)
{
	m_dwStartKeyframe = dwStartkeyframe;
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionStartKeyframe
// Desc: jingjie modified 2006.07.08 get start keyframe of animation action
//-----------------------------------------------------------------------------
DWORD cAnimationActionInfo::getAnimationActionStartKeyframe( )
{
	return m_dwStartKeyframe;
}
//-----------------------------------------------------------------------------
// Name: setAnimationActionEndKeyframe
// Desc: jingjie modified 2006.07.08 set end keyframe of animation action
//-----------------------------------------------------------------------------
void cAnimationActionInfo::setAnimationActionEndKeyframe( DWORD dwEndkeyframe)
{
	m_dwEndKeyframe = dwEndkeyframe;
}
//-----------------------------------------------------------------------------
// Name: setAnimationActionEndKeyframe
// Desc: jingjie modified 2006.07.08 get end keyframe of animation action
//-----------------------------------------------------------------------------
DWORD cAnimationActionInfo::getAnimationActionEndKeyframe( )
{
	return m_dwEndKeyframe;
}
//-----------------------------------------------------------------------------
// Name: setAnimationActionStartTime
// Desc: jingjie modified 2006.07.08 set start time of animation action
//-----------------------------------------------------------------------------
void cAnimationActionInfo::setAnimationActionStartTime( DWORD dwStartTime)
{
	m_dwStartTime = dwStartTime;
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionStartTime
// Desc: jingjie modified 2006.07.08 get start time of animation action
//-----------------------------------------------------------------------------
DWORD cAnimationActionInfo::getAnimationActionStartTime( )
{
	return m_dwStartTime;
}
//-----------------------------------------------------------------------------
// Name: setAnimationActionEndTime
// Desc: jingjie modified 2006.07.08 set end time of animation action
//-----------------------------------------------------------------------------
void cAnimationActionInfo::setAnimationActionEndTime( DWORD dwEndTime)
{
	m_dwEndTime = dwEndTime;
}
//-----------------------------------------------------------------------------
// Name: setAnimationActionEndTime
// Desc: jingjie modified 2006.07.08 get end time of animation action
//-----------------------------------------------------------------------------
DWORD cAnimationActionInfo::getAnimationActionEndTime( )
{
	return m_dwEndTime;
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionTimeLength
// Desc: jingjie modified 2006.07.08 get time length of animation action
//-----------------------------------------------------------------------------
DWORD cAnimationActionInfo::getAnimationActionTimeLength() 
{ 
	return m_dwEndTime - m_dwStartTime; 
}

//-----------------------------------------------------------------------------
// Name: AddHurtEvent
// Desc: ���ӻ����¼�
//-----------------------------------------------------------------------------
// 080109 cyhsieh �����¼�
void cAnimationActionInfo::AddHurtEvent( float fTime )
{
	sHurtEvent* pHurtEvent = n_new(sHurtEvent( fTime ));
	if( pHurtEvent )
	{
		m_pHurtEventList.Append( pHurtEvent );
	}
}

//-----------------------------------------------------------------------------
// Name: GetHurtEventCount
// Desc: ȡ�û����¼�����
//-----------------------------------------------------------------------------
// 080109 cyhsieh �����¼�
int cAnimationActionInfo::GetHurtEventCount( void )
{
	return m_pHurtEventList.Size();
}

//-----------------------------------------------------------------------------
// Name: GetHurtEvent
// Desc: ȡ�û����¼�
//-----------------------------------------------------------------------------
// 080109 cyhsieh �����¼�
sHurtEvent* cAnimationActionInfo::GetHurtEvent( int iIndex )
{
	if( iIndex < m_pHurtEventList.Size() )
		return (sHurtEvent*)m_pHurtEventList[iIndex];

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: cAnimationActionInfoGroup()
// Desc: cAnimationActionInfoGroup ���Ľ���Ԫ
//-----------------------------------------------------------------------------
cAnimationActionInfoGroup::cAnimationActionInfoGroup()
{
	m_iVersion    = 0;
	//û��������   iTrickIDStart   = -1;
	//m_iTrickAniamtionActionIndex = -1;
}

//-----------------------------------------------------------------------------
// Name: ~cAnimationActionInfoGroup()
// Desc: cAnimationActionInfoGroup ���Ľ⹹Ԫ
//-----------------------------------------------------------------------------
cAnimationActionInfoGroup::~cAnimationActionInfoGroup()
{
	removeAllAnimationActionInfo();	
}
//-----------------------------------------------------------------------------
// Name: addAnimationActionInfo
// Desc: jingjie modified 2006.07.07 cAnimationActionInfo map for adding a cAnimationActionInfo.
//-----------------------------------------------------------------------------
void cAnimationActionInfoGroup::addAnimationActionInfo( unsigned int uiIndex , cAnimationActionInfo* pAnimationActionInfo )
{
	if( pAnimationActionInfo )
	{
		tAnimationActionInfoMap::iterator i = m_AnimationActionInfoMap.find( uiIndex );
		if( i == m_AnimationActionInfoMap.end() )
		{
			cAnimationActionInfo* pNewAnimationActionInfo = n_new(cAnimationActionInfo);
			pNewAnimationActionInfo->setAnimationACTIndex( pAnimationActionInfo->getAnimationACTIndex() );
			pNewAnimationActionInfo->setAnimationActionStartTime( pAnimationActionInfo->getAnimationActionStartTime() );
			pNewAnimationActionInfo->setAnimationActionEndTime( pAnimationActionInfo->getAnimationActionEndTime() );
			pNewAnimationActionInfo->setAnimationActionStartKeyframe( pAnimationActionInfo->getAnimationActionStartKeyframe() );
			pNewAnimationActionInfo->setAnimationActionEndKeyframe( pAnimationActionInfo->getAnimationActionEndKeyframe() );

			m_AnimationActionInfoMap.insert(tAnimationActionInfoMap::value_type( uiIndex, pNewAnimationActionInfo) );
		}
	}
}

//-----------------------------------------------------------------------------
// Name: addAnimationActionInfo
// Desc: jingjie modified 2006.07.09 cAnimationActionInfo map for removing a cAnimationActionInfo by animation action index.
//-----------------------------------------------------------------------------
void cAnimationActionInfoGroup::removeAnimationActionInfo( unsigned int uiIndex )
{
	tAnimationActionInfoMap::iterator i = m_AnimationActionInfoMap.find( uiIndex );
	if( i != m_AnimationActionInfoMap.end() )
	{
		n_delete( i->second );
		m_AnimationActionInfoMap.erase(i);
	}
}
//-----------------------------------------------------------------------------
// Name: removeAllAnimationActionInfo
// Desc: jingjie modified 2006.07.07 cAnimationActionInfo map for remove all cAnimationActionInfo.
//-----------------------------------------------------------------------------
void cAnimationActionInfoGroup::removeAllAnimationActionInfo( )
{
	tAnimationActionInfoMap::iterator i, iend;
	i = m_AnimationActionInfoMap.begin();
	iend = m_AnimationActionInfoMap.end();
	for (; i != iend; ++i)
	{
		n_delete( i->second );
	}
	m_AnimationActionInfoMap.clear();
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionKeyframesRange
// Desc: jingjie modified 2006.07.07 get  cAnimationActionKeyframesRange
//-----------------------------------------------------------------------------
cAnimationActionInfo* cAnimationActionInfoGroup::getAnimationActionInfo( unsigned int uiIndex  )
{
	tAnimationActionInfoMap::iterator i = m_AnimationActionInfoMap.find( uiIndex );

	if( i != m_AnimationActionInfoMap.end() )
	{
		return i->second;
	}else{
		return 0;
	}
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionInfoCount
// Desc: jingjie modified 2006.07.12 get size of AnimationActionInfoMap
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::getAnimationActionInfoCount()
{
	// SBUG(ActTableList);
	// return ActTableList->GetCount();
	return m_AnimationActionInfoMap.size();
}
//-----------------------------------------------------------------------------
// Name: getACTFileCount
// Desc: jingjie modified 2006.07.12 get size of AnimationAction
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::getAnimationACTCount()
{
	return m_AnimationACTFileDB.Size();
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionFileName
// Desc: jingjie modified 2006.07.12 get file name of AnimationAction by animation action file index.(animation action file index: 0 ~ 168 )
//-----------------------------------------------------------------------------
char* cAnimationActionInfoGroup::getAnimationACTName( unsigned int uiAnimationACTIndex )
{
	return (char*)m_AnimationACTFileDB[uiAnimationACTIndex].AsCharPtr() ;
}
//-----------------------------------------------------------------------------
// Name: setSoundName
// Desc: jingjie modified 2006.07.12 set sound name of AnimationAction by animation action index
//-----------------------------------------------------------------------------
void cAnimationActionInfoGroup::setSoundInfo(int iAnimationActionIndex, char* szSoundName, int iDelayTimems)
{
	if (szSoundName == NULL) return;

	cAnimationActionInfo* pAnimationActionInfo = getAnimationActionInfo( iAnimationActionIndex );
	if (pAnimationActionInfo)
	{
		pAnimationActionInfo->setSoundName( szSoundName );
		if (iDelayTimems>0)
			pAnimationActionInfo->setSoundDelayTimems(iDelayTimems);
	}
}
//-----------------------------------------------------------------------------
// Name: ClearSoundSetting 
// Desc: jingjie modified 2006.07.12 clear sound name of all animation actions
//-----------------------------------------------------------------------------
void cAnimationActionInfoGroup::clearSoundSetting()
{
	//SBUG(ActTableList);
	cAnimationActionInfo* pAnimationActionInfo = 0;
	tAnimationActionInfoMap::iterator i, iend;
	i = m_AnimationActionInfoMap.begin();
	iend = m_AnimationActionInfoMap.end();
	for (; i != iend; ++i)
	{
		pAnimationActionInfo = i->second;
		pAnimationActionInfo->setSoundName( );
	}
}
//-----------------------------------------------------------------------------
// Name: exportAnimationActionInfoGroup()
// Desc: jingjie modified 2006.07.12 export cAnimationActionInfo Group file.
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::exportAnimationActionInfoGroup(char *szAnimationActionInfoGroupFileName)
{
	//////////////FILE *fp ;
	//////////////BYTE *pData;

	//////////////if ((fp = fopen(szAnimationActionInfoGroupFileName,"wb"))	== NULL ) return -1 ;
	//////////////// д��aft��ʶ����
	//////////////fwrite("AFT",3,1,fp);

	//////////////// д��汾
	//////////////pData =(BYTE *)&m_iVersion;
	//////////////fwrite(pData,sizeof(int),1,fp);

	//////////////// д�붯����������
	//////////////int l_iCount = m_iNumACTFile;
	//////////////pData =(BYTE *)&l_iCount;
	//////////////fwrite(pData,sizeof(int),1,fp);

	//////////////for(int i=0;i<l_iCount;i++)
	//////////////{
	//////////////	// д�뵵�����
	//////////////	int l_iActNo = 0 ;
	//////////////	pData =(BYTE *)&l_iActNo;
	//////////////	fwrite(pData,sizeof(int),1,fp);

	//////////////	// д�뵵�����Ƴ���
	//////////////	int l_iNameSize = m_astrACTFile[i].Length();
	//////////////	pData =(BYTE *)&l_iNameSize;
	//////////////	fwrite(pData,sizeof(int),1,fp);

	//////////////	// д�뵵������
	//////////////	fwrite(m_astrACTFile[i].c_str(),sizeof(char)*l_iNameSize,1,fp);
	//////////////}

	//////////////// д�����
	//////////////fwrite("@",1,1,fp);

	//////////////fclose( fp );
	return 0;
}

//-----------------------------------------------------------------------------
// Name: importAnimationActionInfoGroupFromMemory()
// Desc: ���� XTB ��
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::importAnimationActionInfoGroupFromMemory(char *szAnimationActionInfoGroupFileName)
{
	int i=0;
	int iFileNameLen = 0;

	removeAllAnimationActionInfo();

	cECReader reader;
	// ��ȡ��������  
	if (!reader.LoadFromFile(szAnimationActionInfoGroupFileName))
		return -1;
	// ����
	reader.Reset();
	// 
	bool bLoop = true;
	int iActionIndex; // ��������
	int iAnimationACTIndex; // ����������
	////MOB������	cAnimationActionInfo animationActionInfo;

	for (;bLoop;)
	{
		switch (reader.GetToken())
		{
		case cECReader::ttINT: // ȡ�ö������
			iActionIndex = reader.GetTokenInt();
			// ȡ�� ACT ������, ��������� ACT �������м䲻�����κζ���, ����ע��.
			if (reader.GetToken() == cECReader::ttSTRING)
			{
				String strName(reader.GetTokenStr());
				IndexT i = m_AnimationACTFileDB.FindIndex(strName);
				if (i == InvalidIndex)
				{
					m_AnimationACTFileDB.Append(strName);
					i = m_AnimationACTFileDB.Size()-1;
				}
				iAnimationACTIndex = i;
				//iAnimationACTIndex = m_AnimationACTFileDB.QueryIndexByName(reader.GetTokenStr());
				////MOB������          animationActionInfo.setAnimationACTIndex( iACTFileIndex ); 
				////MOB������          // ��Ϊ�� map, ����ֻҪ������Ч��ż���.
				////MOB������          addAnimationActionInfo( iActionIndex, &animationActionInfo );
			} // end of if (Token == ttSTRING)
			break;
		case cECReader::ttCOMMENT: // ע�ⲻ����.. .
			break;
		default: bLoop = false; break;
		} // end of if (type == ttIDNETIFIER)
	} // end of for (;;

	return 0;
}

//-----------------------------------------------------------------------------
// Name: importAnimationActionInfoGroupFromMemoryEX()
// Desc: ���� XTB ��
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::importAnimationActionInfoGroupFromMemoryEX(char *szAnimationActionInfoGroupFileName)
{
	int i=0;
	int iFileNameLen = 0;

	removeAllAnimationActionInfo();

	
	cECReader reader;
	// ��ȡ��������  
	if (!reader.LoadFromFile(szAnimationActionInfoGroupFileName))
		return -1;
	// ����
	reader.Reset();
	// 
	bool bLoop = true;
	int iActionIndex; // ��������
	int iAnimationACTIndex; // ����������
	cAnimationActionInfo animationActionInfo;

	for (;bLoop;)
	{
		switch (reader.GetToken())
		{
		case cECReader::ttINT: // ȡ�ö������
			iActionIndex = reader.GetTokenInt();
			// ȡ�� ACT ������, ��������� ACT �������м䲻�����κζ���, ����ע��.
			if (reader.GetToken() == cECReader::ttSTRING)
			{
				String strName(reader.GetTokenStr());
				IndexT i = m_AnimationACTFileDB.FindIndex(strName);
				if (i == InvalidIndex)
				{
					m_AnimationACTFileDB.Append(strName);
					i = m_AnimationACTFileDB.Size()-1;
				}
				iAnimationACTIndex = i;
				//iAnimationACTIndex = m_AnimationACTFileDB.QueryIndexByName(reader.GetTokenStr());
				animationActionInfo.setAnimationACTIndex( iAnimationACTIndex ); 
				// ��Ϊ�� map, ����ֻҪ������Ч��ż���.
				addAnimationActionInfo( iActionIndex, &animationActionInfo );
			} // end of if (Token == ttSTRING)
			break;
		case cECReader::ttCOMMENT: // ע�ⲻ����.. .
			break;
		default: bLoop = false; break;
		} // end of if (type == ttIDNETIFIER)
	} // end of for (;;

	return 0;
}

//-----------------------------------------------------------------------------
// Name: importAnimationActionKeyframesRangeGroupWithMergingAnimationKeysFromMemory()
// Desc: jingjie modified 2006.07.12 import cAnimationActionKeyframesRange Group file ( include .aft & trick.aft).
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::importAnimationActionInfoGroup(char *szAnimationActionInfoGroupFileName, const char *szPath )
{
	String strFileName;
	String strFilePath = szPath;
	strFileName = strFilePath + szAnimationActionInfoGroupFileName;
	strFileName += ".xtb";
	{ 
		//PROFILE_SCOPE( AFT_importAnimationActionInfoGroupFromMemory );

		if (importAnimationActionInfoGroupFromMemory( (char*)strFileName.AsCharPtr() ) != 0 )
		{
			/*sprintf( szMessage, "��ȡ(%s) ����", strFileName.c_str() );
			MessageBox(NULL, szMessage, "ERROR", MB_OK);
			LOGS( importAnimationActionInfoGroupFromMemory,	szMessage );*/
			return -1;
		}
	} // end of PROFILE_SCOPE

	return 1;
}

//-----------------------------------------------------------------------------
// Name: importAnimationActionInfoGroupPLAYER()
// Desc: jingjie modified 2006.07.12 import cAnimationActionKeyframesRange Group file ( include .aft & trick.aft).
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::importAnimationActionInfoGroupPLAYER(char *szAnimationActionInfoGroupFileName, const char *szPath )
{
	//char szMessage[256];

	String strFileName;
	String strFilePath = szPath;

	strFileName = strFilePath + szAnimationActionInfoGroupFileName;
	strFileName += ".xtb\0";
	{ 
		//PROFILE_SCOPE( AFT_importAnimationActionInfoGroupFromMemoryEX );

		if(importAnimationActionInfoGroupFromMemoryEX( (char*)strFileName.AsCharPtr() ) != 0 )
		{
			/*sprintf( szMessage, "��ȡ(%s) ����", strFileName.c_str() );
			LOGS( importAnimationActionInfoGroupFromMemoryEX, szMessage );*/
			return -1;
		}
	} // end of PROFILE_SCOPE

	return 1;
}

//-----------------------------------------------------------------------------
// Name: AddHurtEvent
// Desc: ���ӻ����¼�
//-----------------------------------------------------------------------------
void cAnimationActionInfoGroup::AddHurtEvent( int iAnimationActionIndex, float fTime )
{
	cAnimationActionInfo* pAnimationActionInfo = getAnimationActionInfo(iAnimationActionIndex );
	if( pAnimationActionInfo )
	{
		pAnimationActionInfo->AddHurtEvent( fTime );
	}
}


//-----------------------------------------------------------------------------
// Name: cAnimationAction()
// Desc: cAnimationAction ���Ľ���Ԫ
//-----------------------------------------------------------------------------
cAnimationAction::cAnimationAction()
{
	//û��������  m_dwVersion	   = ACT_VERSION;

	//m_aAnimData       = NULL ;
	//m_wNumAnimData    = 0 ;
	m_dwTimeEachKeyframe = 0 ;
	//û��������  TimeMax	   = 0 ;
	//ActTableList = _NEW BTList;
}

//-----------------------------------------------------------------------------
// Name: ~cAnimationAction()
// Desc: cAnimationAction ���Ľ⹹Ԫ
//-----------------------------------------------------------------------------
cAnimationAction::~cAnimationAction()
{
	/*ClearActTableList();
	SAFE_DELETE(ActTableList);
	*/
	//  SAFE_DELETE(BipMatrix);
	//SAFE_DELETE_ARRAY(m_aAnimData);
	removeAllAnimationKeys();
	removeAllAnimationActionInfo();
}

//-----------------------------------------------------------------------------
// Name: importAnimationAction()
// Desc: jingjie modified 2006.07.07 import aniamtion action from file or lpq 
//-----------------------------------------------------------------------------
int cAnimationAction::importAnimationAction(const char *szLoadFileName, bool bFrameNameOnly)
{
	DWORD dwTmp;
	int i=0,j=0,k=0;
	int iVersion= 0;

	Ptr<Stream> stream = IO::IoServer::Instance()->CreateStream(szLoadFileName);
	if (!stream->Open())
	{
		stream = 0;
		return -1;
	}

	// ���ACTʶ����
	char szCode[4];
	String tmp;
	stream->Read(szCode, 3);
	szCode[3] = 0;
	tmp = szCode;
	if (tmp != "ACT")
	{
		stream->Close();
		stream = 0;
		// ACTʶ���벻��
		return -3;
	} 

	// ��ȡ�������汾
	stream->Read(&iVersion, sizeof(int));

	// ��ȡһ��Frame��ʱ�䵥λ
	stream->Read(&m_dwTimeEachKeyframe, sizeof(DWORD));

	// ��ȡ ��animation action ��ͷ��
	WORD wNumAnimData;
	stream->Read(&wNumAnimData, sizeof(WORD));
	removeAllAnimationKeys();

	// ��ȡÿ����ͷ�Ķ���
	for (i = 0; i < wNumAnimData; i++)
	{
		cAnimationKeys animationKeys;
		animationKeys.importAnimationKeysFromMemory(stream, bFrameNameOnly);
		addAnimationKeys( i , &animationKeys);
	} 

	// д�붯���������
	int iNumActTable;
	stream->Read(&iNumActTable, sizeof(int));

	if (iNumActTable > 0)
	{
		removeAllAnimationActionInfo();
		cAnimationActionInfo* pAnimationActionInfo = NULL;
		int iSoundNameLen;
		// Ԥ�� List ��С
		//ActTableList->SetCapacity(iNumActTable);
		for (i = 0; i < iNumActTable;i++)
		{
			pAnimationActionInfo = n_new(cAnimationActionInfo);

			stream->Read(&iSoundNameLen, sizeof(int));
			stream->Seek(iSoundNameLen, Stream::Current);

			// д�붯����ʼʱ��
			stream->Read(&dwTmp, sizeof(DWORD));
			pAnimationActionInfo->setAnimationActionStartTime(dwTmp);
			
			// д�붯������ʱ��
			stream->Read(&dwTmp, sizeof(DWORD));
			pAnimationActionInfo->setAnimationActionEndTime(dwTmp);
			
			// д�붯����ʼFrame
			stream->Read(&dwTmp, sizeof(DWORD));
			pAnimationActionInfo->setAnimationActionStartKeyframe(dwTmp);
			
			// д�붯������Frame
			stream->Read(&dwTmp, sizeof(DWORD));
			pAnimationActionInfo->setAnimationActionEndKeyframe(dwTmp);
			
			addAnimationActionInfo( i , pAnimationActionInfo );
		}
	}
	stream->Close();
	stream = 0;
	return 0;
}

//-----------------------------------------------------------------------------
// Name: importAnimationActionFromFile()
// Desc: jingjie modified 2006.07.07 import aniamtion action from file
//-----------------------------------------------------------------------------
int cAnimationAction::importAnimationActionFromFile(char *LoadFileName)
{
	// cTuo

	//FILE *fp ;

	//int i=0,j=0,k=0;
	//int iVersion= 0;
	//char ACTChar[4];
	//char szMessage[80];

	//ZeroMemory( szMessage , sizeof(char)*80);
	////SAFE_DELETE_ARRAY(m_aAnimData);

	////û��������  TimeMax	= 0;
	////m_wNumAnimData = 0;

	//if (!NFile32::fileExists(LoadFileName))
	//{
	//	sprintf(szMessage,"�Ҳ��� %s �������ϵ�", LoadFileName); 
	//	LOGS(LoadActFile,szMessage);
	//	return -1;
	//}

	//if ((fp = fopen(LoadFileName,"rb")) == NULL ) return -2;

	//// ��ȡACTʶ����
	//fread(ACTChar,3, 1, fp);

	//// ��ȡ�汾
	//fread(&iVersion,sizeof(int), 1, fp);
	//ACTChar[3]= '\0';

	//// ���ACTʶ����
	//if (lstrcmp(ACTChar,"ACT"))
	//{
	//	// ACTʶ���벻��
	//	fclose( fp );
	//	return -3;
	//}

	//// ��ȡһ��Frame��ʱ�䵥λ
	//fread(&m_dwTimeEachKeyframe,sizeof(DWORD), 1, fp);
	//WORD wNumAnimData;
	//// ��ȡ��ͷ����
	//fread((int *)&wNumAnimData, sizeof(WORD),1,fp) ;
	////jingjie test 2006.07.07
	//removeAllAnimationKeys();
	////jingjie test 2006.07.07
	////SAFE_DELETE_ARRAY(m_aAnimData);
	////if (m_wNumAnimData > 0)
	////{
	////  m_aAnimData = _NEW cAnimationKeys[m_wNumAnimData];

	//// ��ȡÿ����ͷ�Ķ���
	//for( i=0 ; i <  wNumAnimData; i++)
	//{
	//	//jingjie test 2006.07.07
	//	//m_aAnimData[i].importAnimationKeysFromFile(fp);
	//	//jingjie test 2006.07.07
	//	cAnimationKeys animationKeys;
	//	animationKeys.importAnimationKeysFromFile(fp);
	//	addAnimationKeys( i , &animationKeys);
	//}
	////}

	//int iNumActTable;
	//// д�붯��range�������
	//fread((int *)&iNumActTable, sizeof(int),1,fp) ;

	//if (iNumActTable > 0)
	//{
	//	removeAllAnimationActionInfo();

	//	cAnimationActionInfo* pAnimationActionInfo = 0;

	//	for (i = 0; i < iNumActTable;i++)
	//	{
	//		pAnimationActionInfo = _NEW cAnimationActionInfo;
	//		// д���Ӧ��������������
	//		int iSoundNameLen = 0;

	//		fread(&iSoundNameLen,sizeof(int),1,fp) ;

	//		if (iSoundNameLen > 0)
	//		{
	//			// д���Ӧ����������
	//			//jingjie test 2006.07.08
	//			/*char *szSoundName = _NEW char[iSoundNameLen+1];

	//			fread(szSoundName,sizeof(char)*iSoundNameLen,1,fp);
	//			szSoundName[iSoundNameLen] = '\0';

	//			sprintf(pActTable->m_szSoundName, "%s.wav", szSoundName);

	//			SAFE_DELETE_ARRAY(szSoundName);
	//			*/

	//			char *szSoundName = _NEW char[iSoundNameLen];
	//			fread(szSoundName,sizeof(char)
	//				*iSoundNameLen,1,fp);
	//			pAnimationActionInfo->setSoundName( szSoundName 
	//				);
	//			SAFE_DELETE_ARRAY(szSoundName);

	//		}
	//		DWORD dwStartTime = 0, dwEndTime = 0;
	//		// д�붯����ʼ
	//		//jingjie test 2006.07.08
	//		//fread(&pActTable->dwStartTime,sizeof(DWORD),1,fp);
	//		fread(&dwStartTime,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionStartTime( 
	//			dwStartTime );
	//		// д�붯������
	//		//jingjie test 2006.07.08
	//		//fread(&pActTable->dwEndTime,sizeof(DWORD),1,fp);
	//		fread(&dwEndTime,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionEndTime( 
	//			dwEndTime );
	//		// д�붯����ʼ
	//		//jingjie test 2006.07.08
	//		DWORD dwStartFrame = 0, dwEndFrame = 0;
	//		fread(&dwStartFrame,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionStartKeyframe( 
	//			dwStartFrame );
	//		// д�붯������
	//		fread(&dwEndFrame,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionEndKeyframe( 
	//			dwEndFrame );
	//		/*fread(&pActTable->dwStartFrame,sizeof(DWORD),1,fp);
	//		// д�붯������
	//		fread(&pActTable->dwEndFrame,sizeof(DWORD),1,fp);
	//		*/

	//		//ActTableList->Add(pAnimationActionInfo);
	//		addAnimationActionInfo( i , pAnimationActionInfo );
	//	}
	//}

	//fclose( fp );
	return 0;
}

int cAnimationAction::importAnimationActionFromMemory(char *buff, unsigned int buff_size)
{
	// cTuo

	//if ((!buff)||(!buff_size)) return -2;

	//int i=0,j=0,k=0;
	//int iVersion= 0;

	//int Offset;

	//char *Buffer,*Mem;

	//Buffer = Mem = (char*)buff;

	//// ���ACTʶ����
	//if (memcmp(Buffer, "ACT", 3))
	//{
	//	// ACTʶ���벻��
	//	//fclose( fp );
	//	return -3;
	//} // end of if (memcmp(Buffer, "ACT", 3))
	//Buffer += 3;

	//// ��ȡ�������汾
	//iVersion = *((int*)Buffer);
	//Buffer += sizeof(int);

	//// ��ȡһ��Frame��ʱ�䵥λ
	//m_dwTimeEachKeyframe = *((DWORD*)Buffer);
	//Buffer += sizeof(DWORD);

	//// ��ȡ ��animation action ��ͷ��
	////jingjie test 2006.07.07
	//WORD wNumAnimData = *((WORD*)Buffer);
	//Buffer += sizeof(WORD);
	//removeAllAnimationKeys();
	////jingjie test 2006.07.07
	////SAFE_DELETE_ARRAY(m_aAnimData);
	////if (m_wNumAnimData > 0)
	////{

	////m_aAnimData = _NEW cAnimationKeys[m_wNumAnimData];

	//// ��ȡÿ����ͷ�Ķ���
	////jingjie test 2006.07.07
	//for (i = 0; i < wNumAnimData; i++)
	//{
	//	//jingjie test 2006.07.07
	//	//Offset = m_aAnimData[i].importAnimationKeysFromMemory
	//	((BYTE*)Buffer, bFrameNameOnly);
	//	//jingjie test 2006.07.07
	//	cAnimationKeys animationKeys;
	//	Offset = animationKeys.importAnimationKeysFromMemory((BYTE*)
	//		Buffer, false); //bFrameNameOnly);
	//	addAnimationKeys( i , &animationKeys);
	//	Buffer += Offset;
	//} // end of for (i = 0; i < m_wNumAnimData; i++)
	////} // end of if (m_wNumAnimData > 0)

	//// д�붯���������
	////fread((int *)&iActionTableSize,sizeof(int),1,fp) ;
	//int iNumActTable = *((int*)Buffer);
	//Buffer += sizeof(int);

	//if (iNumActTable > 0)
	//{
	//	removeAllAnimationActionInfo();
	//	cAnimationActionInfo* pAnimationActionInfo = NULL;
	//	int iSoundNameLen;
	//	// Ԥ�� List ��С
	//	//ActTableList->SetCapacity(iNumActTable);
	//	for (i = 0; i < iNumActTable;i++)
	//	{
	//		pAnimationActionInfo = _NEW cAnimationActionInfo;

	//		iSoundNameLen = *((int*)Buffer);
	//		Buffer += sizeof(int);
	//		if (iSoundNameLen > 0)
	//		{
	//			// д���Ӧ����������
	//			/*char* szSoundName = _NEW char[iSoundNameLen + 
	//			4 ];
	//			memcpy(szSoundName, Buffer, iSoundNameLen);
	//			memcpy(szSoundName + iSoundNameLen, ".wav", 4);
	//			szSoundName[iSoundNameLen + 4] = '\0';
	//			*/
	//			//pAnimationActionInfo->setSoundName( Buffer , 
	//			iSoundNameLen );
	//			//SAFE_DELETE_ARRAY( szSoundName );

	//			//jingjie test 2006.07.08
	//			/*memcpy(pActTable->m_szSoundName, Buffer, 
	//			iSoundNameLen);
	//			memcpy(pActTable->m_szSoundName + 
	//			iSoundNameLen, ".wav", 4);
	//			pActTable->m_szSoundName[iSoundNameLen + 4] = 
	//			'\0';
	//			*/			

	//			Buffer += iSoundNameLen;
	//		} // end of if (l_iSoundNameSize > 0)

	//		// д�붯����ʼʱ��
	//		//jingjie test 2006.07.08
	//		//pActTable->dwStartTime = *((DWORD*)Buffer);
	//		pAnimationActionInfo->setAnimationActionStartTime( *((DWORD*)Buffer) );
	//		Buffer += sizeof(DWORD);
	//		// д�붯������ʱ��
	//		//jingjie test 2006.07.08
	//		//pActTable->dwEndTime = *((DWORD*)Buffer);
	//		pAnimationActionInfo->setAnimationActionEndTime( *((DWORD*)Buffer) );
	//		Buffer += sizeof(DWORD);
	//		// д�붯����ʼFrame
	//		//jingjie test 2006.07.08
	//		//pActTable->dwStartFrame = *((DWORD*)Buffer);
	//		pAnimationActionInfo->setAnimationActionStartKeyframe( *((DWORD*)Buffer) );
	//		Buffer += sizeof(DWORD);
	//		// д�붯������Frame
	//		//jingjie test 2006.07.08
	//		//pActTable->dwEndFrame = *((DWORD*)Buffer);
	//		pAnimationActionInfo->setAnimationActionEndKeyframe( *((DWORD*)Buffer) );
	//		Buffer += sizeof(DWORD);
	//		addAnimationActionInfo( i , pAnimationActionInfo );
	//		//ActTableList->Add(pAnimationActionInfo);
	//	}
	//}
	return 0;
}
//-----------------------------------------------------------------------------
// Name: addAnimationKeys
// Desc: jingjie modified 2006.07.07 cAnimationKeys map for adding a cAnimationKeys.
//-----------------------------------------------------------------------------
void cAnimationAction::addAnimationKeys( unsigned int uiIndex , cAnimationKeys* pAnimationKeys )
{
	if( pAnimationKeys )
	{
		tAnimationKeysMap::iterator i = m_AnimationKeysMap.find( uiIndex );
		if( i == m_AnimationKeysMap.end() )
		{
			cAnimationKeys* pNewAnimationKeys = new(cAnimationKeys);
			pNewAnimationKeys->cloneAnimationKeys(pAnimationKeys);

			m_AnimationKeysMap.insert(tAnimationKeysMap::value_type( uiIndex, pNewAnimationKeys) );
		}
	}
}
//-----------------------------------------------------------------------------
// Name: removeAllAnimationKeys
// Desc: jingjie modified 2006.07.07 cAnimationKeys map for remove all cAnimationKeys.
//-----------------------------------------------------------------------------
void cAnimationAction::removeAllAnimationKeys( )
{
	tAnimationKeysMap::iterator i, iend;
	i = m_AnimationKeysMap.begin();
	iend = m_AnimationKeysMap.end();
	for (; i != iend; ++i)
	{
		n_delete( i->second );
	}
	m_AnimationKeysMap.clear();
}
//-----------------------------------------------------------------------------
// Name: addAnimationActionInfo
// Desc: jingjie modified 2006.07.07 cAnimationActionInfo map for adding a cAnimationActionInfo.
//-----------------------------------------------------------------------------
void cAnimationAction::addAnimationActionInfo( unsigned int uiIndex , 
											  cAnimationActionInfo* pAnimationActionInfo )
{
	if( pAnimationActionInfo )
	{
		tAnimationActionInfoMap::iterator i = m_AnimationActionInfoMap.find( uiIndex );
		if( i == m_AnimationActionInfoMap.end() )
		{
			m_AnimationActionInfoMap.insert(tAnimationActionInfoMap::value_type( uiIndex, pAnimationActionInfo) );
		}
	}
}
//-----------------------------------------------------------------------------
// Name: removeAllAnimationActionInfo
// Desc: jingjie modified 2006.07.07 cAnimationActionInfo map for remove all cAnimationActionInfo.
//-----------------------------------------------------------------------------
void cAnimationAction::removeAllAnimationActionInfo( )
{
	tAnimationActionInfoMap::iterator i, iend;
	i = m_AnimationActionInfoMap.begin();
	iend = m_AnimationActionInfoMap.end();
	for (; i != iend; ++i)
	{
		n_delete( i->second );
	}
	m_AnimationActionInfoMap.clear();
}
//-----------------------------------------------------------------------------
// Name: getAnimationKeys
// Desc: jingjie modified 2006.07.07 get  cAnimationKeys
//-----------------------------------------------------------------------------
cAnimationKeys* cAnimationAction::getAnimationKeys( unsigned int uiIndex  )
{
	tAnimationKeysMap::iterator i = m_AnimationKeysMap.find( uiIndex );

	if( i != m_AnimationKeysMap.end() )
	{
		return i->second;
	}else{
		return 0;
	}
}
//-----------------------------------------------------------------------------
// Name: getAnimationActionInfo
// Desc: jingjie modified 2006.07.07 get  cAnimationActionInfo
//-----------------------------------------------------------------------------
cAnimationActionInfo* cAnimationAction::getAnimationActionInfo( unsigned int 
															   uiIndex  )
{
	tAnimationActionInfoMap::iterator i = m_AnimationActionInfoMap.find( uiIndex );

	if( i != m_AnimationActionInfoMap.end() )
	{
		return i->second;
	}else{
		return 0;
	}
}

}