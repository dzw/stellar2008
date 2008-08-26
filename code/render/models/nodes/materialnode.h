#pragma once
#ifndef MODELS_MATERIALNODE_H
#define MODELS_MATERIALNODE_H
//------------------------------------------------------------------------------
/**
    @class Models::MaterialNode
  
    材质节点，与StateNode有一曲同工之妙，不同的是这个NODE不会每次都创建shaderInstance
	而是保存每个状态信息，用的时候再提交。而且一个node中可以有多个不同的材质。
    
    (C) 2008 cTuo
*/
#include "models/nodes/transformnode.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/material.h"

//------------------------------------------------------------------------------
namespace Models
{
class MaterialNode : public TransformNode
{
    DeclareClass(MaterialNode);
public:
    /// constructor
    MaterialNode();
    /// destructor
    virtual ~MaterialNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();
    /// get pointer to contained shader instance
    const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstance() const;

protected:
    /// create a model node instance
    virtual Ptr<ModelNodeInstance> CreateNodeInstance() const;
    /// called when resources should be loaded
    virtual void LoadResources();
    /// called when resources should be unloaded
    virtual void UnloadResources();

    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
	Util::Array<CoreGraphics::Material> materialList;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderInstance>&
MaterialNode::GetShaderInstance() const
{
    return this->shaderInstance;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
