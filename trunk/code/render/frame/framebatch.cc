//------------------------------------------------------------------------------
//  framebatch.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/framebatch.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "models/visresolver.h"
#include "models/model.h"
#include "models/modelnodeinstance.h"
#include "graphics/modelentity.h"
#include "lighting/lightserver.h"

#include "coregraphics/debugview.h"

namespace Frame
{
ImplementClass(Frame::FrameBatch, 'FBTH', Core::RefCounted);

using namespace Graphics;
using namespace CoreGraphics;
using namespace Models;
using namespace Util;
using namespace Lighting;

//------------------------------------------------------------------------------
/**
*/
FrameBatch::FrameBatch() :
    batchType(BatchType::InvalidBatchType),
    nodeFilter(ModelNodeType::InvalidModelNodeType),
    lightingMode(LightingMode::None),
    sortingMode(SortingMode::None),
    shaderFeatures(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FrameBatch::~FrameBatch()
{
    // make sure Discard() has been called
    n_assert(!this->shader.isvalid());
    n_assert(this->shaderVariables.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameBatch::Discard()
{
    if (this->shader.isvalid())
    {
        this->shader->Discard();
        this->shader = 0;
    }
    this->shaderVariables.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
FrameBatch::Render()
{
    RenderDevice* renderDevice = RenderDevice::Instance();

    // apply shader variables
    IndexT varIndex;
    for (varIndex = 0; varIndex < this->shaderVariables.Size(); varIndex++)
    {
        this->shaderVariables[varIndex]->Apply();
    }

    // render the batch
    renderDevice->BeginBatch(this->batchType, this->shader);
    this->RenderBatch();
    renderDevice->EndBatch();
}

//------------------------------------------------------------------------------
/**
*/
void
FrameBatch::RenderBatch()
{
	VisResolver* visResolver = VisResolver::Instance();      

    // for each visible model...
    const Array<Ptr<Model> >& models = visResolver->GetVisibleModels(this->nodeFilter);
    IndexT modelIndex;
    for (modelIndex = 0; modelIndex < models.Size(); modelIndex++)
    {
        models[modelIndex]->Render(this->nodeFilter, this->lightingMode, this->shaderFeatures);
    }
}

} // namespace Frame
