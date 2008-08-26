#ifndef cAnimationH
#define cAnimationH
//------------------------------------------------------------------------------
/**
	@class KOK::cAnimation

	compute aniamtion matrix that maked by cAnimationKeys of Animation Action for every frame.

	(C) 2008 cTuo
*/

#include "cAnimationAction.h"
#include <map>
#include <string>
#include "util/string.h"

namespace KOK
{

using namespace std;

struct sComputeAnimParam
{
  int   m_iActionIndex1;  // 动作1索引
  int   m_iActionIndex2;  // 动作2索引
  float m_fAnimTime1;     // 动作1动画时间
  float m_fAnimTime2;     // 动作2动画时间
  float m_fLerpValue;     // 动作1动作2混合比例

  sComputeAnimParam()
  :m_iActionIndex1(-1), m_iActionIndex2(-1), m_fAnimTime1(0.0f), m_fAnimTime2(0.0f), m_fLerpValue(0.0f)
  {
  }
  sComputeAnimParam( int iIndex1, int iIndex2, float fTime1, float fTime2, float fLerp )
  :m_iActionIndex1(iIndex1), m_iActionIndex2(iIndex2), m_fAnimTime1(fTime1), m_fAnimTime2(fTime2),
   m_fLerpValue(fLerp)
  {
  }
};

class cAnimation
{
public:
	// type of cAnimationKeys map.
	typedef std::map< unsigned int , cAnimationKeys* > tAnimationKeysMap;
	// type of cAnimation map.
	typedef std::map< string , cAnimation* > tAnimationMap;
private:
	//  aniamtion count.
	static unsigned int m_uiAnimationCount;
	// skeleton name for this aniamtion.
	Util::String m_szSkeletonHierarchyName;
	// cAnimationKeys map for managment of all cAnimationKeys.
	tAnimationKeysMap m_AnimationKeysMap;
	// interplate animation keyframe data between two keyframe.
	bool m_bInterpolateKeyFrame; 
	// parent of this animation
	cAnimation *m_pParent;
	// child of this animation
	cAnimation *m_pChild;
	// animation map ptr
	// 骨架MAP
	tAnimationMap* m_pAnimationMap;

	// get scale of quaternion 
	void quaternionScale(D3DXVECTOR3* pVec, cAnimationActionInfoGroup *pAnimationActionInfoGroup, DWORD dwBaseIndex,DWORD dwTime);
	// get rotation of quaternion 
	void quaternionRotate(D3DXQUATERNION* pQuat,cAnimationActionInfoGroup *pAnimationActionInfoGroup, DWORD dwBaseIndex,DWORD dwTime);
	// get position of quaternion 
	void quaternionPosition(D3DXVECTOR3* pVec, cAnimationActionInfoGroup *pAnimationActionInfoGroup, DWORD dwBaseIndex,DWORD dwTime);
	// get animation matrix by quaternion on current time by action
	void getQuaternionToMatrix(D3DXMATRIXA16* pMatrix, cAnimationActionInfoGroup *pAnimationActionInfoGroup,DWORD dwActionIndex,float curtime );
	// get animation lerp matrix by quaternion on current time between action1 and action2
	void getQuaternionToMatrixLerp(D3DXMATRIXA16* pMatrix,  // 回传的TransformMatrix
																cAnimationActionInfoGroup *pAnimationActionInfoGroup, // 笆
																int iActionIndex1,    // 动作一
																int iActionIndex2, // 动作二
																float fCurTime,     // 动作一已进行的时间
																float fBakCurTime,  // 动作二已进行的时间
																float fLerpValue,     // 动作二的影响程度
																bool bFixed = false);
public:
	cAnimation();
	virtual ~cAnimation();
	// cAnimationKeys map for adding a cAnimationKeys.
	void addAnimationKeys( unsigned int uiIndex , cAnimationKeys* pAnimationKeys );
	// cAnimationKeys map for remove a cAnimationKeys.
	void removeAnimationKeys( unsigned int uiIndex );
	// remove all cAnimationKeys of some action id on AnimationMap.
	void removeAnimationKeysFromAnimationMap( unsigned int uiIndex );
	// cAnimationKeys map for remove all cAnimationKeys.
	void removeAllAnimationKeys( );
	// get  cAnimationKeys
	cAnimationKeys* getAnimationKeys( unsigned int uiIndex  );
	// interplate animation keyframe data between two keyframe.
	void setInterpolateAnimationKeys( bool bInterpolateKeyFrame ){ m_bInterpolateKeyFrame = bInterpolateKeyFrame; }
	// set skeleton name for this aniamtion.
	void	setSkeletonHierarchyName(char* name, int len = -1);
	// get skeleton name for this aniamtion.
	char* getSkeletonHierarchyName() { return (char*)m_szSkeletonHierarchyName.AsCharPtr(); }
	// add cAnimation's child
	cAnimation* addAnimationChild( char* szSkeletonHierarchyName );
	// remove cAnimation's child
	void removeAnimationChild();
	// set cAnimation's child
	void setChild( cAnimation* pChild );
	// get cAnimation's child
	cAnimation *getChild();
	// set cAnimation's parent
	void setParent( cAnimation* pParent );
	// get cAnimation's parent
	cAnimation *getParent();
	// get cAnimation map 
	tAnimationMap* getAnimationMap(){ return m_pAnimationMap; }
	// find cAnimation from cAnimation map.
	cAnimation* findAniamtion( char* szSkeletonHierarchyName );
	// compute cAnimation matrix for computing matrix of skeleton hierarchy
	void getAnimationMatrix(D3DXMATRIXA16* pMatrix,
													cAnimationActionInfoGroup *pAnimationActionInfoGroup,
													const sComputeAnimParam& param);
	//  aniamtion count.
	static unsigned int getAnimationCount(){ return m_uiAnimationCount; }

  bool FindScaleKey(cAnimationKeys* pAnimationKeys, DWORD dwTime, DWORD* pdwKeyBefore, DWORD* pdwKeyAfter);
  bool FindRotateKey(cAnimationKeys* pAnimationKeys, DWORD dwTime, DWORD* pdwKeyBefore, DWORD* pdwKeyAfter);
  bool FindPositionKey(cAnimationKeys* pAnimationKeys, DWORD dwTime, DWORD* pdwKeyBefore, DWORD* pdwKeyAfter);
};
}

#endif