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
#include "resources/managedtexture.h"
#include "math/transform44.h"

#include "kok/model/equip/managedequip.h"
#include "kok/model/equip/equipinstance.h"

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
	virtual void AttachVisibleInstance();

	void SetSkelectonType(SkeleteonType type);

	/// �����㼶
	void SetPart(int part, const Ptr<ManagedBeing>& model);
	void SetPart(int partType, const Util::String& modelName, int textureId);
	void ChangePart(int partType, const Util::String& modelName, int textureId);
	void CreateSkeleton(BeingType1& beType, const Util::String& skeletonName, const Util::String& path);
	cSkeletonSerializer* GetSkeleton()const;
	void UpdateSkeleton();

	/// ����
	int CreateAnimationAction( int iActionIndex, float fPlaySpeed, 
		AnimationActionOperation animationActionOperation, float fFadeoutTime );
	int CreateAnimationActionIntoBlending( int iActionIndex, float fPlaySpeed, float fFadeoutTime );

	// ������ͼ����
	void SetFakeReflectTexture(const Ptr<Resources::ManagedTexture>& tex);
	const Ptr<Resources::ManagedTexture>& GetFakeReflectTexture()const;

	/// װ��
	void SetEquipPart(const Util::String& modelName, EquipLinkerType equipPartType, int texId);
	void RemoveEquipPart(EquipLinkerType equipPartType);
	void SetEquipActionIndex(int actionIndex);

	/// ͸��,��camera��ײʱҪ��͸ʱ��ʾ
	//void SetFadeTypeEasy(float color_dest);

	/// ���±任����
	void SetDirection(float dir);
	void SetPosition(float posX, float posY, float posZ);
	void SetScale(float scaleX, float scaleY, float scaleZ);
protected:
	void UpdateAnimation(float fElapsedTime);
	void UpdatePart();
	void UpdateEquipPart();
	void UpdateTransform();
	void ValidateBodyInstance(const Ptr<ManagedBeing>& managed);
	void ValidateEquipInstance(const Ptr<ManagedEquip>& managed);
#ifdef _CHECK_NOUSE_ANIMATION
	//SkeletonHierarchyUsabilityMap    m_SkeletonHierarchyUnUsedMap;	// ������û���õ���frame
	//void getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, cSubMeshSerializer* pSubMeshSerial );
	//void getUnusedSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy, SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, bool& bParentUse );
#endif


	/// ���岿��,��������ֻ��ǰ��������,�������ǿ���
	Ptr<ManagedBeing> bodyPart[mptCOUNT];
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
	/// �����㼶���±�־��ֻ�пɼ�ʱ�Ÿ��£���һ֡����ֻ����һ��
	bool		  skeletonUpdate;


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
	

	// �ٷ�����ͼ
	Ptr<Resources::ManagedTexture> fakeReflectTexture;

	/// װ��
	Ptr<ManagedEquip> equipPart[eptCOUNT];
	Ptr<EquipInstance> equipInstance[eptCOUNT];
	/// ��װ�б�װ������ʱ������ԭ����
	Util::Array<Ptr<ManagedEquip>> changeEquipList;

	/// �任����
	Math::transform44 tform;
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

inline const Ptr<Resources::ManagedTexture>& 
BeingEntity::GetFakeReflectTexture()const
{
	return this->fakeReflectTexture;
}

inline void 
BeingEntity::SetFakeReflectTexture(const Ptr<Resources::ManagedTexture>& tex)
{
	return this->fakeReflectTexture = tex;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    