#pragma once
#ifndef KOK_THINGENTITY_H
#define KOK_THINGENTITY_H
//------------------------------------------------------------------------------
/**
	��̬������ʵ����we��cThing����
    cTuo
*/
#include "core/singleton.h"
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace KOK
{

/// ����ÿ���ڵ��Ӧ�������ţ�û������Դǰ��Ҫ������Щ��ţ����ص�ʱ�����ö�ӦNode
struct NodeTextureParm 
{
	int nodeId;
	int texId;
};

class ThingEntity : public Graphics::ModelEntity
{
public:
    DeclareClass(ThingEntity);

public:
    /// constructor
    ThingEntity();
    /// destructor
    virtual ~ThingEntity();
	/// set the model's resource id
	void SetResourceId(const Resources::ResourceId& resId);
	/// get the model's resource id
	const Resources::ResourceId& GetResourceId() const;

    virtual void OnActivate();
    virtual void OnDeactivate();
    virtual void OnUpdate();

	virtual void AttachVisibleInstance();

	void SetTextureId(int nodeId, int texId);
	void SetRepeatId(int id);
protected:
	void SetVisiableRepeat();

	/// ���
	int		category;
	/// �����ID
	DWORD	m_dwObjId;
	/// ���������
	int		m_iObjType;

	/// �����ţ���Դ�����������
	Util::Array<NodeTextureParm> texs;
	/// ��Ҫ��ʾ����ģ��Id
	int repeatId;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ThingEntity::SetResourceId(const Resources::ResourceId& id)
{
	n_assert(!this->IsActive());
	this->resId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
ThingEntity::GetResourceId() const
{
	return this->resId;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    