#pragma once
#ifndef GRAPHICS_M2MODELENTITY_H
#define GRAPHICS_M2MODELENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::M2ModelEntity
    
    Represents a visible graphics object.
    
    (C) 2007 Radon Labs GmbH
*/
#include "graphics/graphicsentity.h"
#include "resources/resourceid.h"
#include "wow/m2model/managedm2model.h"
#include "wow/m2model/m2modelinstance.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class M2ModelEntity : public GraphicsEntity
{
    DeclareClass(M2ModelEntity);
public:
    /// constructor
    M2ModelEntity();
    /// destructor
    virtual ~M2ModelEntity();

    /// set the model's resource id
    void SetResourceId(const Resources::ResourceId& resId);
    /// get the model's resource id
    const Resources::ResourceId& GetResourceId() const;
    /// get the state of the contained managed model resource
    Resources::Resource::State GetModelResourceState() const;
    /// get pointer to model instance (only valid if already loaded)
    const Ptr<WOW::M2ModelInstance>& GetModelInstance() const;

protected:
    /// called when entity is created
    virtual void OnActivate();
    /// called before entity is destroyed
    virtual void OnDeactivate();
    /// called when transform matrix changed
    virtual void OnTransformChanged();
    /// called to update the entity before rendering
    virtual void OnUpdate();
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();
    /// validate the ModelInstance
    void ValidateModelInstance();

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
    