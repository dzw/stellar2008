#ifndef cSkeletonSerializerFactory_H
#define cSkeletonSerializerFactory_H
//------------------------------------------------------------------------------
/**
	骨架产生器，全局类

	负责管理cSkeletonSerializer的产生与移除
	全局类，负责创建骨骼

	cTuo
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/hashmap.h"
#include "cSkeletonHierarchy.h"
#include "cSkeletonSerializer.h"
#include "kok/util/cECReader.h"

namespace KOK
{

class cSkeletonSerializerFactory :public Core::RefCounted
{
	DeclareClass(cSkeletonSerializerFactory);
	DeclareSingleton(cSkeletonSerializerFactory);
private:
	//   Mob特效缩放
	void LoadEffectScaleTable( void );                                            
	// 读取特效缩放表
	//  骑乘摄影机调整
	void LoadMountCameraAdjustTable( void );                                      
	// 读取骑乘物摄影机调整表

	Util::HashMap<Util::String, cSkeletonSerializer*, Util::StringHasher> m_pSkeletonSerializerMap;
	//   Mob特效缩放
	Util::HashMap<Util::String, DWORD, Util::StringHasher> m_pSkeletonEffectScaleMap;                                          
	// 记录特效缩放HashMap
	//  骑乘摄影机调整
	Util::HashMap<Util::String, sMountCameraAdjust*, Util::StringHasher> m_pSkeletonCameraAdjustMap;                                         
	// 记录骑乘物摄影机调整HashMap

	bool isOpen;
public:
	cSkeletonSerializerFactory();
	virtual ~cSkeletonSerializerFactory();
	//  create a SkeletonSerializer from SkeletonSerializer Factory
	// iModelType 请参照 cSkinnedCharacterModel
	cSkeletonSerializer* createSkeletonSerializer(int iModelType, const 
	char *szSkeletonSerializerName,const char *szPath);
	//  remove a SkeletonSerializer from SkeletonSerializer Factory
	void removeSkeletonSerializer(const char *szSkeletonSerializerName);
	//  remove all SkeletonSerializers from SkeletonSerializer Factory
	void removeAllSkeletonSerializer();
	//  SkeletonSerializer Factory remove animation action keyframes data by reserve time
	void removeAnimationKeysByReserveTime(float m_fTimes);
	//  get SkeletonSerializers Size from SkeletonSerializer Factory
	int getSkeletonSerializersSize();

	/// open the input server
	void Open();
	/// close the input server
	void Close();
	bool IsOpen()const;
};

inline bool 
cSkeletonSerializerFactory::IsOpen()const
{
	return this->isOpen;
}
}
#endif
