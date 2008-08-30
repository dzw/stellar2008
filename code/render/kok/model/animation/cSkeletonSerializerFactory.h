#ifndef cSkeletonSerializerFactory_H
#define cSkeletonSerializerFactory_H
//------------------------------------------------------------------------------
/**
	�Ǽܲ�������ȫ����

	�������cSkeletonSerializer�Ĳ������Ƴ�
	ȫ���࣬���𴴽�����

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
	//   Mob��Ч����
	void LoadEffectScaleTable( void );                                            
	// ��ȡ��Ч���ű�
	//  �����Ӱ������
	void LoadMountCameraAdjustTable( void );                                      
	// ��ȡ�������Ӱ��������

	Util::HashMap<Util::String, cSkeletonSerializer*, Util::StringHasher> m_pSkeletonSerializerMap;
	//   Mob��Ч����
	Util::HashMap<Util::String, DWORD, Util::StringHasher> m_pSkeletonEffectScaleMap;                                          
	// ��¼��Ч����HashMap
	//  �����Ӱ������
	Util::HashMap<Util::String, sMountCameraAdjust*, Util::StringHasher> m_pSkeletonCameraAdjustMap;                                         
	// ��¼�������Ӱ������HashMap

	bool isOpen;
public:
	cSkeletonSerializerFactory();
	virtual ~cSkeletonSerializerFactory();
	//  create a SkeletonSerializer from SkeletonSerializer Factory
	// iModelType ����� cSkinnedCharacterModel
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
