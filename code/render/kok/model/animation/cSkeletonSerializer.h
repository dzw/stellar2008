#ifndef cSkeletonSerializer_H
#define cSkeletonSerializer_H
//------------------------------------------------------------------------------
/**
	骨架档

	cTuo
*/
#include "cSkeletonHierarchy.h"
#include "kok/util/cECReader.h"

namespace KOK
{

//  Bug Fix:动作还在使用却被删除问题
//[单位s]Act档案使用后保留时间，超过后会被删除
#define ACTFILE_RESERVE_TIME   10

//  骑乘摄影机调整
struct sMountCameraAdjust // 骑乘摄影机调整资讯
{
	float m_fAdjustY;       // 视点高度调整
	float m_fZoomDistance;  // 系统背追Zoom调整

	sMountCameraAdjust( float fAdjustY, float fZoomDistance )
		:m_fAdjustY(fAdjustY), m_fZoomDistance(fZoomDistance)
	{
	}
};

class cSndTable : public cECReader
{
protected:
	cAnimationActionInfoGroup* m_pAnimationActionInfoGroup;

	bool ParseCommand();
	bool ParseSound();
public:
	cSndTable();

	bool LoadTable(char* szFileName, cAnimationActionInfoGroup* 
		pAnimationActionInfoGroup);
};

//  击中事件
class cEventTableReader : public cECReader
{
protected:
	cAnimationActionInfoGroup* m_pAnimationActionInfoGroup;

	bool ParseEvent();
	bool ParseHurtEvent( int iActionIndex );

public:
	cEventTableReader();

	bool LoadTable( char* pFileName, cAnimationActionInfoGroup* 
		pAnimationActionInfoGroup );
};

// 
class cSkeletonSerializer 
{
private:
	//  版本
	int m_iVersion;
	//  武器连接点的数量
	DWORD m_dwWeaponSize;    
	//  法术连接点的数量 
	DWORD m_dwMagicSize;
	//   Mob特效缩放
	float m_fEffectScale;                                                         
	// 特效缩放值
	//  骑乘摄影机调整
	sMountCameraAdjust* m_pMountCameraAdjust;                                     
	// 骑乘摄影机调整资讯
	//  骨架档档名路径
	Util::String m_strFilePath;
	//  骨架档档名
	Util::String m_szSkeletonFileName;
	//  skeleton hierarchy root ptr
	cSkeletonHierarchy* m_pSkeletonHierarchyRoot;
	//   ptr table of weapon skeleton hierarchy 武器连接点
	cSkeletonHierarchy** m_pWeaponSkeletonHierarchyTable;
	//   ptr table of magic skeleton hierarchy 法术连接点
	cSkeletonHierarchy** m_pMagicSkeletonHierarchyTable;
	//   ptr table of mount skeleton hierarchy 骑乘连接点
	cSkeletonHierarchy* m_pMountLinkSkeletonHierarchy01;  // 单人骑乘
	cSkeletonHierarchy* m_pMountLinkSkeletonHierarchy02;  // 双人骑乘前座
	cSkeletonHierarchy* m_pMountLinkSkeletonHierarchy03;  // 双人骑乘后座
	//  animation root ptr
	cAnimation*	m_pAnimationRoot;      
	//  Bug Fix:动作还在使用却被删除问题
	struct stACTFILEINFO
	{
		bool  m_bSingle;      // 单一动作档... 
		float m_fLifeTime;
		DWORD m_dwStartTime;
		DWORD m_dwEndTime;
		DWORD m_dwStartFrame;
		DWORD m_dwEndFrame;
	} * m_pACTFileInfo;
	//  group of  animation action key frame range 
cAnimationActionInfoGroup m_AnimationActionInfoGroup; 

public:
	cSkeletonSerializer();
	virtual ~cSkeletonSerializer();
	//  get skeleton hierarchy root ptr
	cSkeletonHierarchy* getSkeletonHierarchyRoot();
	//  get  ptr table of weapon skeleton hierarchy 武器连接点
		cSkeletonHierarchy* getWeaponSkeletonHierarchy( unsigned int uiWeaponSkeletonHierarchyIndex = 0);
	//  get  size of weapon skeleton hierarchy 武器连接点总数
		DWORD getWeaponSkeletonHierarchySize();
	//  get  ptr table of magic skeleton hierarchy 法术连接点
		cSkeletonHierarchy* getMagicSkeletonHierarchy( unsigned int 
		uiMagicSkeletonHierarchyIndex = 0);
	//  get size of magic skeleton hierarchy 法术连接点总数
		DWORD getMagicSkeletonHierarchySize();
	//  get  ptr table of magic skeleton hierarchy 法术连接点
		cSkeletonHierarchy* getMountSkeletonHierarchy(int in_index = 0);
	//  get animation root ptr 
		cAnimation* getAnimationRoot();
	//  get group of animation action keyframes range
		cAnimationActionInfoGroup* getAnimationActionInfoGroup();
	//  建立武器片, 特效片, 骑乘连结点等资讯列表
	void constructLinkerSkeletonHierarchyTables();
	/*  find a SkeletonHierarchy by 
	SkeletonHierarchyName
	*SkeletonHierarchy which includes skeleton bone,linker,submesh are 
	conbined an entire skeleton hierarchy,
	*so that szSkeletonHierarchyName of findSkeletonHierarchy have to be 
	key name of skeleton bone,linker and submesh.
	*/
	cSkeletonHierarchy* findSkeletonHierarchy(const char *szSkeletonHierarchyName);

	/*  计算animation后的SkeletonHierarchy矩阵
	*@param int iActionIndex1 : 动作1
	*@param int iActionIndex2 : 动作2
	*@param float fAnimTime1 : 动作1已进行的时间
	*@param float fAnimTime2 : 动作2已进行的时间
	*@param float fLerpValue : 动作2的影响程度
	*@param SkeletonHierarchyUsabilityMap* pFrameUseMap : 没有用到的
	SkeletonHierarchy,利用此map减少animaion运算matrix的次数
	*/
#ifdef _CHECK_NOUSE_ANIMATION
	//   修改Mount Matrix的计算
	void computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, 
		D3DXMATRIXA16* pMat, SkeletonHierarchyUsabilityMap* pFrameUseMap = NULL );
#else
	//   修改Mount Matrix的计算
	void computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, D3DXMATRIXA16* pMat );
#endif
	//  读取每个动作对应的动作档名清单
	int importActionTable(int iModelType, char *szActionTableFileName,const 
		char *szPath);
	//  import skeleton from file or lpq
	int importSkeleton(char *szSkeletonFileName,const char *szPath);
	//  import skeleton from file
	int importSkeletonFromFile(char *szSkeletonFileName,const char *szPath);      
	//  Bug Fix:动作还在使用却被删除问题
	void removeAnimationKeysByReserveTime( float fTime );
	// 060502 cyhsieh Act Loading 流程简化
	void prepareAnimationKeys( int iActionIndex );

	//   Mob特效缩放
	void SetEffectScale( float fScale ) { m_fEffectScale = fScale; }              
	// 设定特效缩放值
	float GetEffectScale( void ) { return m_fEffectScale; }                       
	// 取得特效缩放值

	//  骑乘摄影机调整
	void SetMountCameraAdjust( sMountCameraAdjust* pMountCameraAdjust ) { 
		m_pMountCameraAdjust = pMountCameraAdjust; }
	sMountCameraAdjust* GetMountCameraAdjust( void ) { return m_pMountCameraAdjust; }
};

}
#endif
