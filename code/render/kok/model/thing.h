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

	/// 占用大小
	DWORD m_dwHeight;
	DWORD m_dwWidth;

	ThingType m_iType;	                                      // 模型类型
	float m_fWaverFactor;                                     // 摇拽的factor值
	int   m_iRefCount;                                        // 被参考使用的数量

	cAnimationActionInfoGroup m_AnimationActionInfoGroup;

	cAnimation*       m_pAnimaRoot;
	cSkeletonHierarchy *m_pSkeletonHierarchyRoot;

	DWORD             m_dwLightSize;                          // 光的连接点的数量
	cSkeletonHierarchy** m_pLightSkeletonHierarchyTable;      // 光的连接点

	DWORD m_dw3dsMaxEmitDataSize;
	c3dsMaxEmitDataBase** m_pp3dsMaxEmitDatas;

	DWORD             m_dwStopPoint;                          // 阻档点
	bool*             m_pStaticStopPoint;                     // 一般阻档点(scope, land, sky, arrow)
	bool*             m_pScopePoint;                          // 范围的点
	bool*             m_pLandStopPoint;                       // 存放地面阻档点
	bool*             m_pSkyStopPoint;                        // 存放天空阻档点
	bool*             m_pArrowStopPoint;                      // 存放弓箭阻档点
	int               m_iDynamicStopSize;                     // 具有动态阻档的数量
	bool*             m_pDynamicStopPoint;                    // 存放动态阻档点
	bool*             m_pClsPoint;                            // 存放强制清除点
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    