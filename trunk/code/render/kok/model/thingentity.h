#pragma once
#ifndef KOK_THINGENTITY_H
#define KOK_THINGENTITY_H
//------------------------------------------------------------------------------
/**
	静态物（地上物）实例，we的cThing相似
    cTuo
*/
#include "core/singleton.h"
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace KOK
{

/// 保存每个节点对应的纹理编号，没加载资源前需要保存这些编号，加载的时候设置对应Node
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
protected:
	/// 类别
	int		category;
	/// 物件的ID
	DWORD	m_dwObjId;
	/// 物件的类型
	int		m_iObjType;

	/// 纹理
	Util::Array<NodeTextureParm> texs;
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
    