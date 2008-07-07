#pragma once
#ifndef GRAPHICS_WORLDENTITY_H
#define GRAPHICS_WORLDENTITY_H
//------------------------------------------------------------------------------
/**
    º”‘ÿ“ª∏ˆWorld
*/
#include "graphics/graphicsentity.h"
#include "resources/resourceid.h"
#include "wow/world/managedWorld.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class WorldEntity : public GraphicsEntity
{
    DeclareClass(WorldEntity);
public:
    /// constructor
    WorldEntity();
    /// destructor
    virtual ~WorldEntity();

    /// set the model's resource id
    void SetResourceId(const Resources::ResourceId& resId);
    /// get the model's resource id
    const Resources::ResourceId& GetResourceId() const;
    /// get the state of the contained managed model resource
    Resources::Resource::State GetModelResourceState() const;

protected:
    /// called when entity is created
    virtual void OnActivate();
    /// called before entity is destroyed
    virtual void OnDeactivate();
    /// called to update the entity before rendering
    virtual void OnUpdate();
    /// validate the ModelInstance
    void ValidateMapTile();

    Resources::ResourceId resId;
    Ptr<WOW::ManagedM2Model> managedModel;
    Ptr<WOW::M2ModelInstance> modelInstance;
};

//------------------------------------------------------------------------------
/**
*/
inline void
M2ModelEntity::SetResourceId(const Resources::ResourceId& id)
{
    n_assert(!this->IsActive());
    this->resId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
M2ModelEntity::GetResourceId() const
{
    return this->resId;
}

//------------------------------------------------------------------------------
/**
*/
inline Resources::Resource::State
M2ModelEntity::GetModelResourceState() const
{
    return this->managedModel->GetState();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<WOW::M2ModelInstance>&
M2ModelEntity::GetModelInstance() const
{
    return this->modelInstance;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    