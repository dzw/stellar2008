#pragma once
#ifndef KOK_BEINGNTITY_H
#define KOK_BEINGNTITY_H
//------------------------------------------------------------------------------
/**
	生物实例，与we的cSkeletonSerializer相似

	负责生物的骨骼动画和渲染。

    cTuo
*/
#include "core/singleton.h"
#include "graphics/modelentity.h"
#include "kok/model/modeldef.h"
#include "kok/model/managedbeing.h"
#include "kok/model/animation/cSkeletonSerializer.h"

//------------------------------------------------------------------------------
namespace KOK
{
class BeingEntity : public Graphics::ModelEntity
{
public:
    DeclareClass(BeingEntity);

public:
    /// constructor
    BeingEntity();
    /// destructor
    virtual ~BeingEntity();

    virtual void OnActivate();
    virtual void OnDeactivate();
    virtual void OnUpdate();

	void SetSkelectonType(SkeleteonType type);

	void SetPart(int part, const Ptr<ManagedBeing>& model);
	void SetPart(int partType, const Util::String& modelName, int textureId);
	void ChangePart(int partType, const Util::String& modelName, int textureId);
	void CreateSkeleton(BeingType1& beType, const Util::String& skeletonName, const Util::String& path);

	virtual void AttachVisibleInstance();
	void UpdateSkeleton();

	cSkeletonSerializer* GetSkeleton()const;
	int CreateAnimationAction( int iActionIndex, float fPlaySpeed, 
		AnimationActionOperation animationActionOperation, float fFadeoutTime );
	int CreateAnimationActionIntoBlending( int iActionIndex, float fPlaySpeed, float fFadeoutTime );
protected:
	void UpdateAnimation(float fElapsedTime);
	/// 更新需要更新的部件
	void UpdatePart();
	void ValidateBodyInstance(const Ptr<ManagedBeing>& managed, int partType);
#ifdef _CHECK_NOUSE_ANIMATION
	//SkeletonHierarchyUsabilityMap    m_SkeletonHierarchyUnUsedMap;	// 用来存没有用到的frame
	//void getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, cSubMeshSerializer* pSubMeshSerial );
	//void getUnusedSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy, SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, bool& bParentUse );
#endif
	/// 身体部件,如果是坐骑，只需前两个部件,其它都是空着
	Ptr<ManagedBeing> bodyPart[mptCOUNT];
	/// 部件列表，加载后不释放，退出时才释放
	Util::Array<Ptr<ManagedBeing>> partList;
	/// 需要改变的部件，改变完后从该队列删除，避免不必要的检测
	Util::Array<Ptr<ManagedBeing>> changeList;
	/// 实例对象
	Ptr<Models::ModelInstance> bodyInstance[mptCOUNT];
	
	/// 骨架类型、性别?
	SkeleteonType skinType;
	//static char* s_aszMPName[mptCOUNT];

	/// 骨骼
	Util::String skeletonName;
	cSkeletonSerializer* skeletonSerializer;
	
	//stActionState record index , animation time length , play speed , 
	//and animation played time of animation action 
	typedef struct 
	{
		int   m_iIndex;
		float m_fAnimTimeLength;
		float m_fAnimTime;
		float m_fPlaySpeed;
		void clear()
		{
			m_iIndex          = 0;
			m_fAnimTimeLength = 0;
			m_fAnimTime       = 0;
			m_fPlaySpeed      = 1.0f;
		}
	} stActionState;
	/// 动作
	sComputeAnimParam m_sComputeAnimParam;
	// 目前的动作
	stActionState m_CurnAction;
	// 淡出的动作
	stActionState m_FadeoutAction;
	// 剩余淡出时间...
	float         m_fFadeoutTimeRemain;
	// 总淡出时间
	float         m_fFadeoutTime;
	/// 骨骼层级更新标志，只有可见时才更新，且一帧里面只更新一次
	bool		  skeletonUpdate;

};

inline void 
BeingEntity::SetSkelectonType(SkeleteonType type)
{
	this->skinType = type;
}

inline cSkeletonSerializer* 
BeingEntity::GetSkeleton()const
{
	return this->skeletonSerializer;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    