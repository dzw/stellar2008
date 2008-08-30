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

protected:
	/// 类别
	int category;
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
    