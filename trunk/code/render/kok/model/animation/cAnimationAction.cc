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
// Desc: cAnimationKeys 类别的建构元
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
// Desc: cAnimationKeys 类别的解构元
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
// Desc: jingjie modified 2006.07.06 animation action对应的骨头名
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
	// 读取对应的骨头名称长度
	stream->Read(&m_dwSkeletonNameLength, sizeof(DWORD));
	if (m_dwSkeletonNameLength > 0)
	{
		// 读取对应的骨头名称
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

		// 读取m_pPositionKeys 的数量
			stream->Read(&m_dwNumPositionKey, sizeof(DWORD));
		n_delete_array( m_aPositionKey );
		if (m_dwNumPositionKey > 0)
		{
			m_aPositionKey = n_new_array(stPOSITIONKEY, m_dwNumPositionKey);
			// 读取m_pPositionKeys 的资料
			stream->Read(m_aPositionKey, m_dwNumPositionKey * sizeof(stPOSITIONKEY));
		} // end of if (m_dwPositionKeys > 0)

		// 读取m_pRotateKeys 的数量
		stream->Read(&m_dwNumRotateKey, sizeof(DWORD));
		n_delete_array( m_aRotateKey );
		if (m_dwNumRotateKey > 0)
		{
			m_aRotateKey = n_new_array(stROTATEKEY, m_dwNumRotateKey);
			// 读取m_pRotateKeys 的资料
			stream->Read(m_aRotateKey, m_dwNumRotateKey*sizeof(stROTATEKEY));
		} // end of if (m_dwRotateKeys > 0)

		// 读取m_pScaleKeys 的数量
		stream->Read(&m_dwNumScaleKey, sizeof(DWORD));
		n_delete_array( m_aScaleKey );
		if(m_dwNumScaleKey > 0)
		{
			m_aScaleKey = n_new_array(stSCALEKEY, m_dwNumScaleKey);

			// 读取m_pScaleKeys 的资料
			stream->Read(m_aScaleKey, m_dwNumScaleKey*sizeof(stSCALEKEY));
		}
	} 
	else 
	{ // 仅读取 FrameName
		// matrix Offset 4*4
		stream->Seek(sizeof(D3DXMATRIX), Stream::Current);

		// 读取m_pPositionKeys 的数量
		stream->Read(&m_dwNumPositionKey, sizeof(DWORD));
		if (m_dwNumPositionKey > 0)
		{
			stream->Seek(m_dwNumPositionKey * sizeof(stPOSITIONKEY), Stream::Current);
		} // end of if (m_dwPositionKeys > 0)

		// 读取m_pRotateKeys 的数量
		stream->Read(&m_dwNumRotateKey, sizeof(DWORD));
		if (m_dwNumRotateKey > 0)
		{
			stream->Seek(m_dwNumRotateKey*sizeof(stROTATEKEY), Stream::Current);
		} // end of if (m_dwRotateKeys > 0)

		// 读取m_pScaleKeys 的数量
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
//	// 写入对应的骨头名称长度
//	pData =(BYTE *)&m_dwSkeletonNameLength;
//	fwrite(pData,sizeof(int),1,fp);
//
//	// 写入对应的骨头名称
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
//	// 写入m_pPositionKeys 的数量
//	pData =(BYTE *)&m_dwNumPositionKey;
//	fwrite(pData,sizeof(DWORD),1,fp);
//	// 写入m_pPositionKeys 的资料
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
//	// 写入m_pRotateKeys 的数量
//	pData =(BYTE *)&m_dwNumRotateKey;
//	fwrite(pData,sizeof(DWORD),1,fp);
//	// 写入m_pRotateKeys 的资料
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
//	// 写入m_pScaleKeys 的数量
//	pData =(BYTE *)&m_dwNumScaleKey;
//	fwrite(pData,sizeof(DWORD),1,fp);
//	// 写入m_pScaleKeys 的资料
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
//	// 读取对应的骨头名称长度
//	fread(&m_dwSkeletonNameLength,sizeof(DWORD), 1, fp);
//
//	if (m_dwSkeletonNameLength > 0)
//	{
//		// 读取对应的骨头名称
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
//	// 读取m_pPositionKeys 的数量
//	fread(&m_dwNumPositionKey,sizeof(DWORD),1,fp);
//
//	SAFE_DELETE_ARRAY( m_aPositionKey );
//	if(m_dwNumPositionKey > 0)
//	{
//		m_aPositionKey = n_new(stPOSITIONKEY, m_dwNumPositionKey);
//
//		// 读取m_pPositionKeys 的资料
//		for(i=0;i<m_dwNumPositionKey;i++)
//		{
//			fread(&m_aPositionKey[i].dwTime,sizeof(DWORD),1,fp);
//			fread(&m_aPositionKey[i].vPos.x,sizeof(float),1,fp);
//			fread(&m_aPositionKey[i].vPos.y,sizeof(float),1,fp);
//			fread(&m_aPositionKey[i].vPos.z,sizeof(float),1,fp);
//		}
//	}
//
//	// 读取m_pRotateKeys 的数量
//	fread(&m_dwNumRotateKey,sizeof(DWORD),1,fp);
//
//	SAFE_DELETE_ARRAY( m_aRotateKey );
//	if(m_dwNumRotateKey > 0)
//	{
//		m_aRotateKey = n_new(stROTATEKEY, m_dwNumRotateKey);
//
//		// 读取m_pRotateKeys 的资料
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
//	// 读取m_pScaleKeys 的数量
//	fread(&m_dwNumScaleKey,sizeof(DWORD),1,fp);
//
//	SAFE_DELETE_ARRAY( m_aScaleKey );
//	if (m_dwNumScaleKey > 0)
//	{
//		m_aScaleKey = n_new(stSCALEKEY, m_dwNumScaleKey);
//
//		// 读取m_pScaleKeys 的资料
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

	// 080109 cyhsieh 击中事件
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

	// 080109 cyhsieh 击中事件
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
// Desc: 增加击中事件
//-----------------------------------------------------------------------------
// 080109 cyhsieh 击中事件
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
// Desc: 取得击中事件数量
//-----------------------------------------------------------------------------
// 080109 cyhsieh 击中事件
int cAnimationActionInfo::GetHurtEventCount( void )
{
	return m_pHurtEventList.Size();
}

//-----------------------------------------------------------------------------
// Name: GetHurtEvent
// Desc: 取得击中事件
//-----------------------------------------------------------------------------
// 080109 cyhsieh 击中事件
sHurtEvent* cAnimationActionInfo::GetHurtEvent( int iIndex )
{
	if( iIndex < m_pHurtEventList.Size() )
		return (sHurtEvent*)m_pHurtEventList[iIndex];

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: cAnimationActionInfoGroup()
// Desc: cAnimationActionInfoGroup 类别的建构元
//-----------------------------------------------------------------------------
cAnimationActionInfoGroup::cAnimationActionInfoGroup()
{
	m_iVersion    = 0;
	//没看到在用   iTrickIDStart   = -1;
	//m_iTrickAniamtionActionIndex = -1;
}

//-----------------------------------------------------------------------------
// Name: ~cAnimationActionInfoGroup()
// Desc: cAnimationActionInfoGroup 类别的解构元
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
	//////////////// 写入aft的识别码
	//////////////fwrite("AFT",3,1,fp);

	//////////////// 写入版本
	//////////////pData =(BYTE *)&m_iVersion;
	//////////////fwrite(pData,sizeof(int),1,fp);

	//////////////// 写入动作档的数量
	//////////////int l_iCount = m_iNumACTFile;
	//////////////pData =(BYTE *)&l_iCount;
	//////////////fwrite(pData,sizeof(int),1,fp);

	//////////////for(int i=0;i<l_iCount;i++)
	//////////////{
	//////////////	// 写入档案编号
	//////////////	int l_iActNo = 0 ;
	//////////////	pData =(BYTE *)&l_iActNo;
	//////////////	fwrite(pData,sizeof(int),1,fp);

	//////////////	// 写入档案名称长度
	//////////////	int l_iNameSize = m_astrACTFile[i].Length();
	//////////////	pData =(BYTE *)&l_iNameSize;
	//////////////	fwrite(pData,sizeof(int),1,fp);

	//////////////	// 写入档案名称
	//////////////	fwrite(m_astrACTFile[i].c_str(),sizeof(char)*l_iNameSize,1,fp);
	//////////////}

	//////////////// 写入结束
	//////////////fwrite("@",1,1,fp);

	//////////////fclose( fp );
	return 0;
}

//-----------------------------------------------------------------------------
// Name: importAnimationActionInfoGroupFromMemory()
// Desc: 载入 XTB 档
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::importAnimationActionInfoGroupFromMemory(char *szAnimationActionInfoGroupFileName)
{
	int i=0;
	int iFileNameLen = 0;

	removeAllAnimationActionInfo();

	cECReader reader;
	// 读取档案内容  
	if (!reader.LoadFromFile(szAnimationActionInfoGroupFileName))
		return -1;
	// 重置
	reader.Reset();
	// 
	bool bLoop = true;
	int iActionIndex; // 动作索引
	int iAnimationACTIndex; // 动作档索引
	////MOB不用做	cAnimationActionInfo animationActionInfo;

	for (;bLoop;)
	{
		switch (reader.GetToken())
		{
		case cECReader::ttINT: // 取得动作编号
			iActionIndex = reader.GetTokenInt();
			// 取得 ACT 档名称, 动作编号与 ACT 档名称中间不能有任何东西, 包括注解.
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
				////MOB不用做          animationActionInfo.setAnimationACTIndex( iACTFileIndex ); 
				////MOB不用做          // 因为是 map, 所以只要新增有效编号即可.
				////MOB不用做          addAnimationActionInfo( iActionIndex, &animationActionInfo );
			} // end of if (Token == ttSTRING)
			break;
		case cECReader::ttCOMMENT: // 注解不处理.. .
			break;
		default: bLoop = false; break;
		} // end of if (type == ttIDNETIFIER)
	} // end of for (;;

	return 0;
}

//-----------------------------------------------------------------------------
// Name: importAnimationActionInfoGroupFromMemoryEX()
// Desc: 载入 XTB 档
//-----------------------------------------------------------------------------
int cAnimationActionInfoGroup::importAnimationActionInfoGroupFromMemoryEX(char *szAnimationActionInfoGroupFileName)
{
	int i=0;
	int iFileNameLen = 0;

	removeAllAnimationActionInfo();

	
	cECReader reader;
	// 读取档案内容  
	if (!reader.LoadFromFile(szAnimationActionInfoGroupFileName))
		return -1;
	// 重置
	reader.Reset();
	// 
	bool bLoop = true;
	int iActionIndex; // 动作索引
	int iAnimationACTIndex; // 动作档索引
	cAnimationActionInfo animationActionInfo;

	for (;bLoop;)
	{
		switch (reader.GetToken())
		{
		case cECReader::ttINT: // 取得动作编号
			iActionIndex = reader.GetTokenInt();
			// 取得 ACT 档名称, 动作编号与 ACT 档名称中间不能有任何东西, 包括注解.
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
				// 因为是 map, 所以只要新增有效编号即可.
				addAnimationActionInfo( iActionIndex, &animationActionInfo );
			} // end of if (Token == ttSTRING)
			break;
		case cECReader::ttCOMMENT: // 注解不处理.. .
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
			/*sprintf( szMessage, "读取(%s) 错误", strFileName.c_str() );
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
			/*sprintf( szMessage, "读取(%s) 错误", strFileName.c_str() );
			LOGS( importAnimationActionInfoGroupFromMemoryEX, szMessage );*/
			return -1;
		}
	} // end of PROFILE_SCOPE

	return 1;
}

//-----------------------------------------------------------------------------
// Name: AddHurtEvent
// Desc: 增加击中事件
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
// Desc: cAnimationAction 类别的建构元
//-----------------------------------------------------------------------------
cAnimationAction::cAnimationAction()
{
	//没看到在用  m_dwVersion	   = ACT_VERSION;

	//m_aAnimData       = NULL ;
	//m_wNumAnimData    = 0 ;
	m_dwTimeEachKeyframe = 0 ;
	//没看到在用  TimeMax	   = 0 ;
	//ActTableList = _NEW BTList;
}

//-----------------------------------------------------------------------------
// Name: ~cAnimationAction()
// Desc: cAnimationAction 类别的解构元
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

	// 检查ACT识别码
	char szCode[4];
	String tmp;
	stream->Read(szCode, 3);
	szCode[3] = 0;
	tmp = szCode;
	if (tmp != "ACT")
	{
		stream->Close();
		stream = 0;
		// ACT识别码不符
		return -3;
	} 

	// 读取动作档版本
	stream->Read(&iVersion, sizeof(int));

	// 读取一格Frame的时间单位
	stream->Read(&m_dwTimeEachKeyframe, sizeof(DWORD));

	// 读取 有animation action 骨头数
	WORD wNumAnimData;
	stream->Read(&wNumAnimData, sizeof(WORD));
	removeAllAnimationKeys();

	// 读取每个骨头的动作
	for (i = 0; i < wNumAnimData; i++)
	{
		cAnimationKeys animationKeys;
		animationKeys.importAnimationKeysFromMemory(stream, bFrameNameOnly);
		addAnimationKeys( i , &animationKeys);
	} 

	// 写入动作表的数量
	int iNumActTable;
	stream->Read(&iNumActTable, sizeof(int));

	if (iNumActTable > 0)
	{
		removeAllAnimationActionInfo();
		cAnimationActionInfo* pAnimationActionInfo = NULL;
		int iSoundNameLen;
		// 预设 List 大小
		//ActTableList->SetCapacity(iNumActTable);
		for (i = 0; i < iNumActTable;i++)
		{
			pAnimationActionInfo = n_new(cAnimationActionInfo);

			stream->Read(&iSoundNameLen, sizeof(int));
			stream->Seek(iSoundNameLen, Stream::Current);

			// 写入动作起始时间
			stream->Read(&dwTmp, sizeof(DWORD));
			pAnimationActionInfo->setAnimationActionStartTime(dwTmp);
			
			// 写入动作结束时间
			stream->Read(&dwTmp, sizeof(DWORD));
			pAnimationActionInfo->setAnimationActionEndTime(dwTmp);
			
			// 写入动作起始Frame
			stream->Read(&dwTmp, sizeof(DWORD));
			pAnimationActionInfo->setAnimationActionStartKeyframe(dwTmp);
			
			// 写入动作结束Frame
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

	////没看到在用  TimeMax	= 0;
	////m_wNumAnimData = 0;

	//if (!NFile32::fileExists(LoadFileName))
	//{
	//	sprintf(szMessage,"找不到 %s 动作资料档", LoadFileName); 
	//	LOGS(LoadActFile,szMessage);
	//	return -1;
	//}

	//if ((fp = fopen(LoadFileName,"rb")) == NULL ) return -2;

	//// 读取ACT识别码
	//fread(ACTChar,3, 1, fp);

	//// 读取版本
	//fread(&iVersion,sizeof(int), 1, fp);
	//ACTChar[3]= '\0';

	//// 检查ACT识别码
	//if (lstrcmp(ACTChar,"ACT"))
	//{
	//	// ACT识别码不符
	//	fclose( fp );
	//	return -3;
	//}

	//// 读取一格Frame的时间单位
	//fread(&m_dwTimeEachKeyframe,sizeof(DWORD), 1, fp);
	//WORD wNumAnimData;
	//// 读取骨头总数
	//fread((int *)&wNumAnimData, sizeof(WORD),1,fp) ;
	////jingjie test 2006.07.07
	//removeAllAnimationKeys();
	////jingjie test 2006.07.07
	////SAFE_DELETE_ARRAY(m_aAnimData);
	////if (m_wNumAnimData > 0)
	////{
	////  m_aAnimData = _NEW cAnimationKeys[m_wNumAnimData];

	//// 读取每个骨头的动作
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
	//// 写入动作range表的数量
	//fread((int *)&iNumActTable, sizeof(int),1,fp) ;

	//if (iNumActTable > 0)
	//{
	//	removeAllAnimationActionInfo();

	//	cAnimationActionInfo* pAnimationActionInfo = 0;

	//	for (i = 0; i < iNumActTable;i++)
	//	{
	//		pAnimationActionInfo = _NEW cAnimationActionInfo;
	//		// 写入对应的声音档名长度
	//		int iSoundNameLen = 0;

	//		fread(&iSoundNameLen,sizeof(int),1,fp) ;

	//		if (iSoundNameLen > 0)
	//		{
	//			// 写入对应的声音档名
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
	//		// 写入动作起始
	//		//jingjie test 2006.07.08
	//		//fread(&pActTable->dwStartTime,sizeof(DWORD),1,fp);
	//		fread(&dwStartTime,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionStartTime( 
	//			dwStartTime );
	//		// 写入动作结束
	//		//jingjie test 2006.07.08
	//		//fread(&pActTable->dwEndTime,sizeof(DWORD),1,fp);
	//		fread(&dwEndTime,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionEndTime( 
	//			dwEndTime );
	//		// 写入动作起始
	//		//jingjie test 2006.07.08
	//		DWORD dwStartFrame = 0, dwEndFrame = 0;
	//		fread(&dwStartFrame,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionStartKeyframe( 
	//			dwStartFrame );
	//		// 写入动作结束
	//		fread(&dwEndFrame,sizeof(DWORD),1,fp);
	//		pAnimationActionInfo->setAnimationActionEndKeyframe( 
	//			dwEndFrame );
	//		/*fread(&pActTable->dwStartFrame,sizeof(DWORD),1,fp);
	//		// 写入动作结束
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

	//// 检查ACT识别码
	//if (memcmp(Buffer, "ACT", 3))
	//{
	//	// ACT识别码不符
	//	//fclose( fp );
	//	return -3;
	//} // end of if (memcmp(Buffer, "ACT", 3))
	//Buffer += 3;

	//// 读取动作档版本
	//iVersion = *((int*)Buffer);
	//Buffer += sizeof(int);

	//// 读取一格Frame的时间单位
	//m_dwTimeEachKeyframe = *((DWORD*)Buffer);
	//Buffer += sizeof(DWORD);

	//// 读取 有animation action 骨头数
	////jingjie test 2006.07.07
	//WORD wNumAnimData = *((WORD*)Buffer);
	//Buffer += sizeof(WORD);
	//removeAllAnimationKeys();
	////jingjie test 2006.07.07
	////SAFE_DELETE_ARRAY(m_aAnimData);
	////if (m_wNumAnimData > 0)
	////{

	////m_aAnimData = _NEW cAnimationKeys[m_wNumAnimData];

	//// 读取每个骨头的动作
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

	//// 写入动作表的数量
	////fread((int *)&iActionTableSize,sizeof(int),1,fp) ;
	//int iNumActTable = *((int*)Buffer);
	//Buffer += sizeof(int);

	//if (iNumActTable > 0)
	//{
	//	removeAllAnimationActionInfo();
	//	cAnimationActionInfo* pAnimationActionInfo = NULL;
	//	int iSoundNameLen;
	//	// 预设 List 大小
	//	//ActTableList->SetCapacity(iNumActTable);
	//	for (i = 0; i < iNumActTable;i++)
	//	{
	//		pAnimationActionInfo = _NEW cAnimationActionInfo;

	//		iSoundNameLen = *((int*)Buffer);
	//		Buffer += sizeof(int);
	//		if (iSoundNameLen > 0)
	//		{
	//			// 写入对应的声音档名
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

	//		// 写入动作起始时间
	//		//jingjie test 2006.07.08
	//		//pActTable->dwStartTime = *((DWORD*)Buffer);
	//		pAnimationActionInfo->setAnimationActionStartTime( *((DWORD*)Buffer) );
	//		Buffer += sizeof(DWORD);
	//		// 写入动作结束时间
	//		//jingjie test 2006.07.08
	//		//pActTable->dwEndTime = *((DWORD*)Buffer);
	//		pAnimationActionInfo->setAnimationActionEndTime( *((DWORD*)Buffer) );
	//		Buffer += sizeof(DWORD);
	//		// 写入动作起始Frame
	//		//jingjie test 2006.07.08
	//		//pActTable->dwStartFrame = *((DWORD*)Buffer);
	//		pAnimationActionInfo->setAnimationActionStartKeyframe( *((DWORD*)Buffer) );
	//		Buffer += sizeof(DWORD);
	//		// 写入动作结束Frame
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