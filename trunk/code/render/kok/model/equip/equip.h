#pragma once
#ifndef KOK_EQUIP_H
#define KOK_EQUIP_H
//------------------------------------------------------------------------------
/**
    @class KOK::Equip

    (C) 2008 cTuo
*/
#include "models/model.h"
#include "io/stream.h"
#include "kok/model/animation/cSkeletonHierarchy.h"
#include "kok/model/animation/cAnimationAction.h"
#include "kok/model/animation/cAnimation.h"

//------------------------------------------------------------------------------
namespace KOK
{

class Equip : public Models::Model
{
    DeclareClass(Equip);
public:
    /// constructor
    Equip();
    /// destructor
    virtual ~Equip();

    /// unload the resource, or cancel the pending load
    virtual void Unload();
	/// 从内存加载/解析资源
	virtual bool SetupFromStream(const Ptr<IO::Stream>& stream);
	int LoadAnimation( const char* pLoadFileName );

	cSkeletonHierarchy* GetSkeletonHierarchyRoot();
	cAnimation* GetAnimRoot()const;
	cAnimationActionInfoGroup* GetActionInfoGroup();
private:

	/// 版本
	int meshVersion;

	/// 占用大小
	DWORD m_dwHeight;
	DWORD m_dwWidth;

	cSkeletonHierarchy* m_pSkeletonHierarchyRoot;

	cAnimationActionInfoGroup m_AnimationActionInfoGroup;
	cAnimation*   m_pAnimRoot;
};

inline cSkeletonHierarchy* 
Equip::GetSkeletonHierarchyRoot()
{
	return this->m_pSkeletonHierarchyRoot;
}

inline cAnimation* 
Equip::GetAnimRoot()const
{
	return this->m_pAnimRoot;
}

inline cAnimationActionInfoGroup* 
Equip::GetActionInfoGroup()
{
	return &this->m_AnimationActionInfoGroup;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    