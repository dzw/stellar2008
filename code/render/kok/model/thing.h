#pragma once
#ifndef KOK_THING_H
#define KOK_THING_H
//------------------------------------------------------------------------------
/**
    @class KOK::Thing

    (C) 2008 cTuo
*/
#include "models/model.h"
#include "io/stream.h"
#include "kok/model/animation/cAnimation.h"
#include "kok/model/animation/cSkeletonHierarchy.h"
#include "kok/effect/c3dsMaxParticle.h"

//------------------------------------------------------------------------------
namespace KOK
{

class Thing : public Models::Model
{
    DeclareClass(Thing);
public:
	enum ThingType
	{
		STATIC_THING		= 1,
		WAVER_FLOWER_THING  = 2,
		WAVER_TREE_THING	= 3
	};

	

    /// constructor
    Thing();
    /// destructor
    virtual ~Thing();

    /// unload the resource, or cancel the pending load
    virtual void Unload();

	virtual bool SetupFromStream(const Ptr<IO::Stream>& stream);
	//void LoadTexture(const String& path);
protected:
	friend class ThingReader;

	int meshVersion;

	/// ռ�ô�С
	DWORD m_dwHeight;
	DWORD m_dwWidth;

	ThingType m_iType;	                                      // ģ������
	float m_fWaverFactor;                                     // ҡק��factorֵ
	int   m_iRefCount;                                        // ���ο�ʹ�õ�����

	cAnimationActionInfoGroup m_AnimationActionInfoGroup;

	cAnimation*       m_pAnimaRoot;
	cSkeletonHierarchy *m_pSkeletonHierarchyRoot;

	DWORD             m_dwLightSize;                          // ������ӵ������
	cSkeletonHierarchy** m_pLightSkeletonHierarchyTable;      // ������ӵ�

	DWORD m_dw3dsMaxEmitDataSize;
	c3dsMaxEmitDataBase** m_pp3dsMaxEmitDatas;

	DWORD             m_dwStopPoint;                          // �赵��
	bool*             m_pStaticStopPoint;                     // һ���赵��(scope, land, sky, arrow)
	bool*             m_pScopePoint;                          // ��Χ�ĵ�
	bool*             m_pLandStopPoint;                       // ��ŵ����赵��
	bool*             m_pSkyStopPoint;                        // �������赵��
	bool*             m_pArrowStopPoint;                      // ��Ź����赵��
	int               m_iDynamicStopSize;                     // ���ж�̬�赵������
	bool*             m_pDynamicStopPoint;                    // ��Ŷ�̬�赵��
	bool*             m_pClsPoint;                            // ���ǿ�������
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    