#pragma once
#ifndef KOK_BEINGNTITY_H
#define KOK_BEINGNTITY_H
//------------------------------------------------------------------------------
/**
	����ʵ������we��cSkeletonSerializer����

	��������Ĺ�����������Ⱦ��

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
	/// ������Ҫ���µĲ���
	void UpdatePart();
	void ValidateBodyInstance(const Ptr<ManagedBeing>& managed, int partType);
#ifdef _CHECK_NOUSE_ANIMATION
	//SkeletonHierarchyUsabilityMap    m_SkeletonHierarchyUnUsedMap;	// ������û���õ���frame
	//void getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, cSubMeshSerializer* pSubMeshSerial );
	//void getUnusedSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy, SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, bool& bParentUse );
#endif
	/// ���岿��,��������ֻ��ǰ��������,�������ǿ���
	Ptr<ManagedBeing> bodyPart[mptCOUNT];
	/// �����б����غ��ͷţ��˳�ʱ���ͷ�
	Util::Array<Ptr<ManagedBeing>> partList;
	/// ��Ҫ�ı�Ĳ������ı����Ӹö���ɾ�������ⲻ��Ҫ�ļ��
	Util::Array<Ptr<ManagedBeing>> changeList;
	/// ʵ������
	Ptr<Models::ModelInstance> bodyInstance[mptCOUNT];
	
	/// �Ǽ����͡��Ա�?
	SkeleteonType skinType;
	//static char* s_aszMPName[mptCOUNT];

	/// ����
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
	/// ����
	sComputeAnimParam m_sComputeAnimParam;
	// Ŀǰ�Ķ���
	stActionState m_CurnAction;
	// �����Ķ���
	stActionState m_FadeoutAction;
	// ʣ�൭��ʱ��...
	float         m_fFadeoutTimeRemain;
	// �ܵ���ʱ��
	float         m_fFadeoutTime;
	/// �����㼶���±�־��ֻ�пɼ�ʱ�Ÿ��£���һ֡����ֻ����һ��
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
    