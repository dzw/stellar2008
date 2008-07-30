//------------------------------------------------------------------------------
//  transformnodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/modelinstance.h"
#include "Terrain/terrainnodeinstance.h"
#include "Terrain/terrainnode.h"
#include "coregraphics/renderdevice.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainNodeInstance, 'CKNI', Models::ModelNodeInstance);

using namespace Math;
using namespace CoreGraphics;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
TerrainNodeInstance::TerrainNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TerrainNodeInstance::~TerrainNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainNodeInstance::Render()
{
    const Ptr<TerrainNode> node = GetModelNode().downcast<TerrainNode>();
    if (node->GetResourceState() == Resources::Resource::Loaded)
    {
        RenderDevice::Instance()->Draw();
    }
}    

//------------------------------------------------------------------------------
/**
*/
void
TerrainNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
    const Ptr<TerrainNode> node = GetModelNode().downcast<TerrainNode>();
    if (this->IsVisible())
    {
        if (node->GetResourceState() == Resources::Resource::Loaded)
        {
            node->AddToRender();
	        this->modelNode->AddVisibleNodeInstance(frameIndex, this);
        }
    }
    else
    {
        node->FreeVertexOffset();
    }
}

} // namespace Models
