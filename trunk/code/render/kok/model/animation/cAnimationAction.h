#ifndef cAnimationAction_H
#define cAnimationAction_H
//------------------------------------------------------------------------------
/**
	@class KOK::cAnimationKeys

	animation action products keyframe data and range
	cAnimationKeys is a animation action keyframes data from .act file.

	(C) 2008 cTuo
*/

#include <map>
#include "util/string.h"
#include "util/array.h"
#include "io/stream.h"

namespace KOK
{

using namespace std;

class cAnimationKeys
{
public:
	//  rotation of keyframe
	struct stROTATEKEY
	{
		DWORD dwTime;
		D3DXQUATERNION quatRotate;
	};
	//  postion of keyframe
	struct stPOSITIONKEY
	{
		DWORD dwTime;
		D3DXVECTOR3 vPos;
	};
	//  scale of keyframe
	struct stSCALEKEY
	{
		DWORD dwTime;
		D3DXVECTOR3 vScale;
	};

private:
	//  cAnimationKeys count
	static unsigned int s_uiAnimationKeysCountInGame; // 整个游戏里AnimationKeys物件总数...
	//  animation action对应的骨头名长度
	DWORD	m_dwSkeletonNameLength;
	//  animation action对应的骨头名
	char* m_szSkeletonName;
	//  matrix of keyframe not in use?????
	//D3DXMATRIXA16    m_matMatrix;
	//  array of position of keyframe
	stPOSITIONKEY* m_aPositionKey;       // 位置的Key Frame
	//  array size of position of keyframe
	DWORD	m_dwNumPositionKey;
	//  array of rotation of keyframe
	stROTATEKEY* m_aRotateKey;
	//  array size of rotation of keyframe
	DWORD m_dwNumRotateKey;
	//  array of scale of keyframe
	stSCALEKEY* m_aScaleKey;
	//  array size of scale of keyframe
	DWORD m_dwNumScaleKey;
public:
	cAnimationKeys();
	virtual ~cAnimationKeys();
	//  animation action对应的骨头名
	char* getSkeletonHierarchyName();
	//  get position of animation action by keyframe index
	cAnimationKeys::stPOSITIONKEY* getPositionKey(unsigned int uiKeyframeIndex);
	//  get array size of position of keyframe
  DWORD getNumPositionKey() { return m_dwNumPositionKey; }
	//  clone position keyframe of animation action
	void clonePositionKey(cAnimationKeys::stPOSITIONKEY* pPositionKey, DWORD dwNumPositionKey);
	//  get rotation of animation action by keyframe index
	cAnimationKeys::stROTATEKEY* getRotateKey(unsigned int uiKeyframeIndex);
	//  get array size of rotation of keyframe
  DWORD getNumRotateKey() { return m_dwNumRotateKey; }
	//  clone rotation keyframe of animation action
	void cloneRotateKey(cAnimationKeys::stROTATEKEY* pRotateKey, DWORD dwNumRotateKey); 
	//  get scale of animation action by keyframe index
	cAnimationKeys::stSCALEKEY* getScaleKey(unsigned int uiKeyframeIndex);
	//  get array size of scale of keyframe
  DWORD getNumScaleKey() { return m_dwNumScaleKey; }
	//  clone scale keyframe of animation action
	void cloneScaleKey(cAnimationKeys::stSCALEKEY* pScaleKey, DWORD dwNumScaleKey);
	//  clone keyframe data of animation action
	void cloneAnimationKeys( cAnimationKeys* pAnimationKeys);
	//   not in use
	//cAnimationCache *m_pAnimCache;       // 动作矩阵的快取
	//  export AnimationKeys into file
	//int	exportAnimationKeysToFile(FILE *fp);	// cTuo
	//  import AnimationKeys from file
	//int importAnimationKeysFromFile(FILE *fp);	// cTuo
	//  import AnimationKeys from memory
	void importAnimationKeysFromMemory(const Ptr<IO::Stream>& stream, bool bNameOnly = false);
	//  get cAnimationKeys count
	static unsigned int getAnimationKeysCountInGame(){ return s_uiAnimationKeysCountInGame;}
};

// 080109 cyhsieh 击中事件
struct sHurtEvent
{
  float m_fHurtTime;
  sHurtEvent()
  :m_fHurtTime(0.0f)
  {
  }
  sHurtEvent(float fHurtTime )
  :m_fHurtTime(fHurtTime)
  {
  }
};

/*cAnimationActionInfo
 * 	
 *start keyframe, end keyframe , start and end time ,and it's sound of a aniamtion action 
 */
class cAnimationActionInfo
{
private:
	//  cAnimationActionKeyframesRange count
	static unsigned int s_uiAnimationActionInfoCountInGame;
	//  sound of this animation action.
	char* m_szSoundName; 
  DWORD m_dwSoundDelayTimems;// 音效延迟时间
	//  index of animation action
    // 指向ACTFileDB里的索引
	unsigned int m_uiAnimationACTIndex;  
	//  start keyframe of animation action
	DWORD m_dwStartKeyframe ;
	//  end keyframe of animation action
	DWORD m_dwEndKeyframe ;
	//  start time of animation action
	DWORD m_dwStartTime;     // 动作起始
	//  end time of animation action
	DWORD m_dwEndTime;       // 动作结束
    // 击中事件
	Util::Array<sHurtEvent*> m_pHurtEventList;

public:
	cAnimationActionInfo();
	virtual ~cAnimationActionInfo();
	//  set sound name of this animation action.
	void setSoundName( char* name = 0, int len = -1);
  void setSoundDelayTimems(DWORD iDelayTimems);
	//  get sound name of this animation action.
	char* getSoundName( );
  int getSoundDelayTimems(){ return m_dwSoundDelayTimems;};
	//  set index of animation action
	void setAnimationACTIndex(unsigned int uiAnimationACTIndex);
	//  get index of animation action
	unsigned int getAnimationACTIndex( );
	//  set start keyframe of animation action
	void setAnimationActionStartKeyframe( DWORD dwStartkeyframe);
	//  get start keyframe of animation action
	DWORD getAnimationActionStartKeyframe( );
	//  set end keyframe of animation action
	void setAnimationActionEndKeyframe( DWORD dwEndkeyframe);
	//  get end keyframe of animation action
	DWORD getAnimationActionEndKeyframe( );
	//  set start time of animation action
	void setAnimationActionStartTime( DWORD dwStartTime);
	//  get start time of animation action
	DWORD getAnimationActionStartTime( );
	//  set end time of animation action
	void setAnimationActionEndTime( DWORD dwEndTime);
	//  get end time of animation action
	DWORD getAnimationActionEndTime( );
	//  get time length of animation action
	DWORD getAnimationActionTimeLength();

  // 击中事件
  void AddHurtEvent( float fTime );                         // 增加击中事件
  int GetHurtEventCount( void );                            // 取得击中事件数量
  sHurtEvent* GetHurtEvent( int iIndex );                   // 取得击中事件
	//---------------------------------------------------------------------------
	//  cAnimationActionInfo count
	static unsigned int getAnimationActionInfoCountInGame(){ return s_uiAnimationActionInfoCountInGame; }
};

/*cAnimationActionKeyframesRangeGroup
* 
*group of aniamtion action keyframes range (.aft file describes how many aniamtion action be groupped.)
*/
class cAnimationActionInfoGroup 
{
public:
	//  type of cAnimationActionInfo map.
	typedef std::map< unsigned int , cAnimationActionInfo* > tAnimationActionInfoMap;
private:
	//  cAnimationActionInfo map for managment of all cAnimationActionInfo.
	tAnimationActionInfoMap m_AnimationActionInfoMap;
	//  file version
	int m_iVersion;
	//  招式animation action index
	Util::Array<Util::String> m_AnimationACTFileDB;
	// 载入XTB (Action Table) MOB, NPC专用
	int importAnimationActionInfoGroupFromMemory(char *szAnimationActionInfoGroupFileName);
	// 载入XTB (Action Table)
	int importAnimationActionInfoGroupFromMemoryEX(char *szAnimationActionInfoGroupFileName);
public:
	cAnimationActionInfoGroup();
	virtual ~cAnimationActionInfoGroup();
	//  cAnimationActionInfo map for adding a cAnimationActionInfo.
	void addAnimationActionInfo( unsigned int uiIndex , cAnimationActionInfo* pAnimationActionInfo );
	//  cAnimationActionInfo map for removing a cAnimationActionInfo by animation action index.
	void removeAnimationActionInfo( unsigned int uiIndex  );
	//  cAnimationActionInfo map for remove all cAnimationActionInfo.
	void removeAllAnimationActionInfo( );
	//  get  cAnimationActionInfo by animation action index.
	cAnimationActionInfo* getAnimationActionInfo( unsigned int uiIndex );
	//  get size of AnimationActionInfoMap
	int getAnimationActionInfoCount();
	//  get size of AnimationAction( animation action index: 0 ~ 168 )
  int getAnimationACTCount();
	//  get file name of AnimationAction by animation action index.(animation action file index: 0 ~ 168 )
	char* getAnimationACTName( unsigned int uiAnimationACTIndex = 0 );
	//  clear sound name of all animation actions
	void clearSoundSetting();
	//  set sound name of AnimationAction by animation action index
	void setSoundInfo(int iAnimationActionIndex, char* szSoundName,int iDelayTimems=0);
	//  export cAnimationActionInfo Group file.
	int exportAnimationActionInfoGroup(char *szAnimationActionInfoGroupFileName);
	//  import cAnimationActionInfo Group file ( include .aft & _trick.aft).
	int importAnimationActionInfoGroup(char *szAnimationActionInfoGroupFileName,const char *szPath);
  // 
  int importAnimationActionInfoGroupPLAYER(char *szAnimationActionInfoGroupFileName,const char *szPath);
  void AddHurtEvent( int iAnimationActionIndex, float fTime );  // 增加击中事件
};
/*cAnimationAction
* 
*aniamtion action .act file describes the info of keyframes data and range
*/
class cAnimationAction
{
public:
	//  type of cAnimationKeys map.(小写t开头是template的意思)
	typedef std::map< unsigned int , cAnimationKeys* > tAnimationKeysMap;
	//  type of cAnimationActionKeyframesRange map.
	typedef std::map< unsigned int , cAnimationActionInfo* > tAnimationActionInfoMap;
private:
	//  cAnimationKeys map for managment of all cAnimationKeys.
	tAnimationKeysMap m_AnimationKeysMap;
	//  cAnimationKeys map for adding a cAnimationKeys.
	void addAnimationKeys( unsigned int uiIndex , cAnimationKeys* pAnimationKeys );
	//  cAnimationKeys map for remove all cAnimationKeys.
	void removeAllAnimationKeys( );
	//  cAnimationActionInfo map for managment of all cAnimationActionInfo.
	tAnimationActionInfoMap m_AnimationActionInfoMap;
	//  cAnimationActionInfo map for adding a cAnimationActionInfo.
	void addAnimationActionInfo( unsigned int uiIndex , cAnimationActionInfo* pAnimationActionInfo );
	//  cAnimationActionInfo map for remove all cAnimationActionInfo.
	void removeAllAnimationActionInfo( );
	//  time of each key frame 
	DWORD  m_dwTimeEachKeyframe;              // 一格Frame的时间
	
public:
	cAnimationAction();
	virtual ~cAnimationAction();
	//cAnimationKeys *m_aAnimData;
	//WORD  m_wNumAnimData;                // 有动作的骨头数
	//  get  cAnimationKeys
	cAnimationKeys* getAnimationKeys( unsigned int uiIndex  );
	//  get  cAnimationKeys Map size
	unsigned int getAnimationKeysCount(){ return m_AnimationKeysMap.size(); }
	//  get  cAnimationActionInfo
	cAnimationActionInfo* getAnimationActionInfo( unsigned int uiIndex  );
	//  get  cAnimationActionKeyframesRange Map size
	unsigned int getAnimationActionInfoCount(){ return m_AnimationActionInfoMap.size(); }
	//  clear cAnimationActionKeyframesRange Map but don't delete cAnimationActionKeyframesRange ??????
	//it uses in changing cActionTable to manage them ,so doesn't Carfully to call this function that It ocuurs "Memory Leak" easily.
	void clearAnimationActionInfoMap( ){  m_AnimationActionInfoMap.clear(); };
	//  get time of each key frame 
	DWORD getTimeEachKeyframe(){ return m_dwTimeEachKeyframe; }	
	//  import aniamtion action from file
  int importAnimationActionFromFile(char *LoadFileName);
	//  import aniamtion action from file or lpq 
  int importAnimationAction(const char *LoadFileName, const char* pPathID, bool bFrameNameOnly = false);

	int importAnimationActionFromMemory(char* buff,unsigned int buff_size);
};
}
#endif