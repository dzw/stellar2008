#ifndef cSkeletonHierarchy_H
#define cSkeletonHierarchy_H
//------------------------------------------------------------------------------
/**
	@class cSkeletonHierarchy
		SkeletonHierarchy which includes skeleton bone,linker,submesh are conbined an entire skeleton hierarchy. 
	(C) 2008 cTuo
*/    

#include "cAnimation.h"
#include <map>
#include <string>

namespace KOK
{
#ifdef _CHECK_NOUSE_ANIMATION
class cSkeletonHierarchy;
typedef std::map<cSkeletonHierarchy*, cSkeletonHierarchy*> SkeletonHierarchyUsabilityMap;
#endif

class cSkeletonHierarchy
{
public:
	enum SkeletonHierarchyLinkerType
	{
		SkeletonHierarchyWeaponLinker = 0,
		SkeletonHierarchyMagicLinker = 1,
		SkeletonHierarchyLightLinker = 2,
		SkeletonHierarchyLinkerCount
	};
	// type of skeleton hierarchy map.
	typedef std::map<string, cSkeletonHierarchy*> SkeletonHierarchyMap;
private:
	// cSkeletonHierarchy count
	static unsigned int m_uiSkeletonHierarchyCount;
	//DWORD m_dwHashID;
	// Skeleton名称
	Util::String m_strName;
	// level Skeleton
	cSkeletonHierarchy* m_pSibling;
	// Skeleton's child
	cSkeletonHierarchy* m_pChild;
	// Skeleton's parent
	cSkeletonHierarchy* m_pParent;
	// skeleton原先的平移量???没用到???
	D3DXVECTOR3* m_pSkeletonTranslation;
	// 判断是否有m_pSibling
	bool m_bNextFlag; 
	// 判断是否有m_pChild
	bool m_bChildFlag;
	// skeleton种类
	int m_iFrameType;// 
	// 这个Skeleton对应的animation
	cAnimation *m_pAnimation;
	// 这个Skeleton目前的计算animation后的矩阵
	D3DXMATRIXA16 m_matTransform;
	// skeleton hierarchy map,作查询使用
	SkeletonHierarchyMap* m_pSkeletonHierarchyMap;
	// 将skeleton hierarchy加入map,以作查询使用
	void addSkeletinHierarcyIntoMap( char* szSkeletonHierarchyName);
	// 将skeleton hierarchy从map删除
	void removeSkeletinHierarcyFromMap( char* szSkeletonHierarchyName);
	//skeleton hierarchy maps with linker type.
	//typedef std::map<string, cSkeletonHierarchy*> SkeletonHierarchyLinkerMaps;
	//skeleton hierarchy maps with linker type.作查询linker的skeleton hierarchy用
	SkeletonHierarchyMap* m_pSkeletonHierarchyLinkerMaps;
	//add linker into skeleton hierarchy maps with linker type.
	void addSkeletinHierarcyLinkerIntoMaps( char* szSkeletonHierarchyName );
public :
	cSkeletonHierarchy();
	virtual ~cSkeletonHierarchy();
	// Skeleton's child
	cSkeletonHierarchy *getChild();
	// Skeleton's parent
	cSkeletonHierarchy *getParent();
	// 同一level Skeleton
	cSkeletonHierarchy *getSibling();
	// get skeleton name
	char*	getSkeletonHierarchyName();
	// get animation
	cAnimation* getAnimation();
	// set animation
	void setAnimation(cAnimation* pAnimation);
	// 这个Skeleton目前的计算animation后的矩阵
	D3DXMATRIXA16* getSkeletonHierarchyMatrix();
	// produce hash id;
	//DWORD	hashIt(char* pStr); 
	// get hash id
	//DWORD getHashID() { return m_dwHashID; }
	// find skeleton hierarcy by name
	virtual cSkeletonHierarchy* findSkeletonHierarchy(TCHAR* name);
	// find skeleton hierarcy by hasid
	//virtual cSkeletonHierarchy* findSkeletonHierarchyWithHash(char* name, DWORD hashID);
	// count skeleton hierarcy by name, not efficient code!!!!!!!!
	//void countSkeletonHierarchy(char *pName,DWORD &dwCount);
	// get SkeletonHierarchyMap ptr
	SkeletonHierarchyMap* getSkeletonHierarchyMap();	
	// get SkeletonHierarchyLinkerMaps ptr
	SkeletonHierarchyMap* getSkeletonHierarchyLinkerMaps(SkeletonHierarchyLinkerType skeletonHierarchyLinkerType = SkeletonHierarchyWeaponLinker);

	// 计算animation后的SkeletonHierarchy矩阵
#ifdef _CHECK_NOUSE_ANIMATION
	void computeSkeletonHierarchyMatrix( cAnimationActionInfoGroup *pAnimationActionInfoGroup, 
                                       const sComputeAnimParam& param,
                                       SkeletonHierarchyUsabilityMap* pSkeletonHierarchyUnUsedMap = NULL );
#else
	void computeSkeletonHierarchyMatrix( cAnimationActionInfoGroup *pAnimationActionInfoGroup, 
                                       const sComputeAnimParam& param/*,
                                       float fLerpValue*/ );
#endif
	// 从file建立Skeleton Hierarchy
	virtual void importFromFile(FILE *fp);
	// 从memory建立Skeleton Hierarchy
	virtual void importFromMemory(const Ptr<IO::Stream>& stream);
	// 这个skeleton的原先的平移量？？？没用到？？
	/*D3DXVECTOR3* getSkeletonTranslation()
	{
		return m_pSkeletonTranslation;
	}*/
	// get cSkeletonHierarchy count
	static unsigned int getSkeletonHierarchyCount(){ return m_uiSkeletonHierarchyCount; }
	
};
}
#endif