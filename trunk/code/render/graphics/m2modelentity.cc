//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/m2modelentity.h"
#include "wow/m2model/m2modelserver.h"
#include "coregraphics/shaperenderer.h"

namespace Graphics
{
ImplementClass(Graphics::M2ModelEntity, 'M2LE', Graphics::GraphicsEntity);

using namespace CoreGraphics;
using namespace Math;
using namespace WOW;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
M2ModelEntity::M2ModelEntity()
{
    this->SetType(ModelType);
}

//------------------------------------------------------------------------------
/**
*/
M2ModelEntity::~M2ModelEntity()
{
    n_assert(!this->modelInstance.isvalid());
}

//------------------------------------------------------------------------------
/**
    This will initialize the managed model inside the M2ModelEntity.
*/
void
M2ModelEntity::OnActivate()
{
    n_assert(!this->IsActive());
    n_assert(this->resId.IsValid());
    GraphicsEntity::OnActivate();
    
    // note: we will remain invalid until at least our model has loaded
    this->SetValid(false);
    this->managedModel = M2ModelServer::Instance()->LoadManagedM2Model(this->resId);
    this->ValidateModelInstance();
}

//------------------------------------------------------------------------------
/**
    Cleanup our managed model, and ModelInstance if it is already 
    initialized.
*/
void
M2ModelEntity::OnDeactivate()
{
    n_assert(this->IsActive());
    n_assert(this->managedModel.isvalid());

    // discard our model instance (if exists)
    if (this->modelInstance.isvalid())
    {
        this->modelInstance->Discard();
        this->modelInstance = 0;
    }

    // discard our managed model
    M2ModelServer::Instance()->DiscardManagedM2Model(this->managedModel);
    this->managedModel = 0;

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    In OnTransformChanged() we need to update the transformation
    of our ModelInstance (if it has already been initialised).
*/
void
M2ModelEntity::OnTransformChanged()
{
    if (this->modelInstance.isvalid())
    {
        this->modelInstance->SetTransform(this->GetTransform());
    }
}

//------------------------------------------------------------------------------
/**
    In OnUpdate() the M2ModelEntity first needs to check whether the
    ManagedModel has already been loaded, and if yes, a ModelInstance
    will be created, finally, the ModelInstance will be prepared for
    rendering.
*/
void
M2ModelEntity::OnUpdate()
{
    // first check if our model has been loaded already, if yes
    // our model instance must be initialized
    this->ValidateModelInstance();

    // if our model instance is valid, let it update itself
    if (this->modelInstance.isvalid())
    {
        this->modelInstance->Update();
    }

    // important: call parent class!
    GraphicsEntity::OnUpdate();
}

//------------------------------------------------------------------------------
/**
    This creates and initializes our ModelInstance if needed. Since Model 
    loading happens asynchronously this may happen at any time after
    the M2ModelEntity is activated.
*/
void
M2ModelEntity::ValidateModelInstance()
{
    if (!this->modelInstance.isvalid())
    {
        if (this->managedModel->GetState() == Resource::Loaded)
        {
            const Ptr<M2Model> model = this->managedModel->GetM2Model();
            n_assert(model->IsLoaded());
            this->SetLocalBoundingBox(model->GetBoundingBox());
            this->modelInstance = model->CreateInstance();
            this->modelInstance->SetTransform(this->GetTransform());
            this->modelInstance->SetModelEntity(this);
            this->SetValid(true);
        }
        else
        {
            // @todo: check for load failed!
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render our debug visualization (the bounding box).
*/
void
M2ModelEntity::OnRenderDebug()
{
    float4 color(1.0f, 0.0f, 0.0f, 0.5f);
    matrix44 shapeTransform = this->globalBox.to_matrix44();
    ShapeRenderer::Instance()->DrawShape(shapeTransform, ShapeRenderer::Box, color);
}

} // namespace Graphics
