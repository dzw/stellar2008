#ifndef cSkeletonSerializer_H
#define cSkeletonSerializer_H
//------------------------------------------------------------------------------
/**
	�Ǽܵ�

	cTuo
*/
#include "cSkeletonHierarchy.h"
#include "kok/util/cECReader.h"

namespace KOK
{

//  Bug Fix:��������ʹ��ȴ��ɾ������
//[��λs]Act����ʹ�ú���ʱ�䣬������ᱻɾ��
#define ACTFILE_RESERVE_TIME   10

//  �����Ӱ������
struct sMountCameraAdjust // �����Ӱ��������Ѷ
{
	float m_fAdjustY;       // �ӵ�߶ȵ���
	float m_fZoomDistance;  // ϵͳ��׷Zoom����

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

//  �����¼�
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
	//  �汾
	int m_iVersion;
	//  �������ӵ������
	DWORD m_dwWeaponSize;    
	//  �������ӵ������ 
	DWORD m_dwMagicSize;
	//   Mob��Ч����
	float m_fEffectScale;                                                         
	// ��Ч����ֵ
	//  �����Ӱ������
	sMountCameraAdjust* m_pMountCameraAdjust;                                     
	// �����Ӱ��������Ѷ
	//  �Ǽܵ�����·��
	Util::String m_strFilePath;
	//  �Ǽܵ�����
	Util::String m_szSkeletonFileName;
	//  skeleton hierarchy root ptr
	cSkeletonHierarchy* m_pSkeletonHierarchyRoot;
	//   ptr table of weapon skeleton hierarchy �������ӵ�
	cSkeletonHierarchy** m_pWeaponSkeletonHierarchyTable;
	//   ptr table of magic skeleton hierarchy �������ӵ�
	cSkeletonHierarchy** m_pMagicSkeletonHierarchyTable;
	//   ptr table of mount skeleton hierarchy ������ӵ�
	cSkeletonHierarchy* m_pMountLinkSkeletonHierarchy01;  // �������
	cSkeletonHierarchy* m_pMountLinkSkeletonHierarchy02;  // ˫�����ǰ��
	cSkeletonHierarchy* m_pMountLinkSkeletonHierarchy03;  // ˫����˺���
	//  animation root ptr
	cAnimation*	m_pAnimationRoot;      
	//  Bug Fix:��������ʹ��ȴ��ɾ������
	struct stACTFILEINFO
	{
		bool  m_bSingle;      // ��һ������... 
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
	//  get  ptr table of weapon skeleton hierarchy �������ӵ�
		cSkeletonHierarchy* getWeaponSkeletonHierarchy( unsigned int uiWeaponSkeletonHierarchyIndex = 0);
	//  get  size of weapon skeleton hierarchy �������ӵ�����
		DWORD getWeaponSkeletonHierarchySize();
	//  get  ptr table of magic skeleton hierarchy �������ӵ�
		cSkeletonHierarchy* getMagicSkeletonHierarchy( unsigned int 
		uiMagicSkeletonHierarchyIndex = 0);
	//  get size of magic skeleton hierarchy �������ӵ�����
		DWORD getMagicSkeletonHierarchySize();
	//  get  ptr table of magic skeleton hierarchy �������ӵ�
		cSkeletonHierarchy* getMountSkeletonHierarchy(int in_index = 0);
	//  get animation root ptr 
		cAnimation* getAnimationRoot();
	//  get group of animation action keyframes range
		cAnimationActionInfoGroup* getAnimationActionInfoGroup();
	//  ��������Ƭ, ��ЧƬ, �����������Ѷ�б�
	void constructLinkerSkeletonHierarchyTables();
	/*  find a SkeletonHierarchy by 
	SkeletonHierarchyName
	*SkeletonHierarchy which includes skeleton bone,linker,submesh are 
	conbined an entire skeleton hierarchy,
	*so that szSkeletonHierarchyName of findSkeletonHierarchy have to be 
	key name of skeleton bone,linker and submesh.
	*/
	cSkeletonHierarchy* findSkeletonHierarchy(const char *szSkeletonHierarchyName);

	/*  ����animation���SkeletonHierarchy����
	*@param int iActionIndex1 : ����1
	*@param int iActionIndex2 : ����2
	*@param float fAnimTime1 : ����1�ѽ��е�ʱ��
	*@param float fAnimTime2 : ����2�ѽ��е�ʱ��
	*@param float fLerpValue : ����2��Ӱ��̶�
	*@param SkeletonHierarchyUsabilityMap* pFrameUseMap : û���õ���
	SkeletonHierarchy,���ô�map����animaion����matrix�Ĵ���
	*/
#ifdef _CHECK_NOUSE_ANIMATION
	//   �޸�Mount Matrix�ļ���
	void computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, 
		D3DXMATRIXA16* pMat, SkeletonHierarchyUsabilityMap* pFrameUseMap = NULL );
#else
	//   �޸�Mount Matrix�ļ���
	void computeSkeletonHierarchyMatrix( const sComputeAnimParam& param, D3DXMATRIXA16* pMat );
#endif
	//  ��ȡÿ��������Ӧ�Ķ��������嵥
	int importActionTable(int iModelType, char *szActionTableFileName,const 
		char *szPath);
	//  import skeleton from file or lpq
	int importSkeleton(char *szSkeletonFileName,const char *szPath);
	//  import skeleton from file
	int importSkeletonFromFile(char *szSkeletonFileName,const char *szPath);      
	//  Bug Fix:��������ʹ��ȴ��ɾ������
	void removeAnimationKeysByReserveTime( float fTime );
	// 060502 cyhsieh Act Loading ���̼�
	void prepareAnimationKeys( int iActionIndex );

	//   Mob��Ч����
	void SetEffectScale( float fScale ) { m_fEffectScale = fScale; }              
	// �趨��Ч����ֵ
	float GetEffectScale( void ) { return m_fEffectScale; }                       
	// ȡ����Ч����ֵ

	//  �����Ӱ������
	void SetMountCameraAdjust( sMountCameraAdjust* pMountCameraAdjust ) { 
		m_pMountCameraAdjust = pMountCameraAdjust; }
	sMountCameraAdjust* GetMountCameraAdjust( void ) { return m_pMountCameraAdjust; }
};

}
#endif
