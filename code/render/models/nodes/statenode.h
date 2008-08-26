#pragma once
#ifndef MODELS_STATENODE_H
#define MODELS_STATENODE_H
//------------------------------------------------------------------------------
/**
    @class Models::StateNode
  
    A model node which holds shader state information and applies shared shader
    state for its set of StateNodeInstances.
    
	设置渲染状态，每个NODE都会创建一份shaderInstance实例，然后保存每个变量的值，
	用的时候直接应用shaderInstance，而不需要再设置，缺点非常明显，创建的时候太
	慢，每次都要遍历所有effect中的变量。而且也没必要每个NODE都创建一份shaderinstance.

	（猜测）或许每次创建一个shaderinstance在创建的时候把SHADER的参数传到了GPU,渲染的时候不
	需要再提交，可以提高渲染速度，不过创建却实很慢！！！

    (C) 2007 Radon Labs GmbH
*/
#include "models/nodes/transformnode.h"
#include "coregraphics/shaderinstance.h"
#include "resources/managedtexture.h"

//------------------------------------------------------------------------------
namespace Models
{
class StateNode : public TransformNode
{
    DeclareClass(StateNode);
public:
    /// constructor
    StateNode();
    /// destructor
    virtual ~StateNode();

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
    /// setup a new managed texture variable
    void SetupManagedTextureVariable(const Attr::AttrId& resAttrId, const Ptr<CoreGraphics::ShaderVariable>& var);
    /// update managed texture variables
    bool UpdateManagedTextureVariables();

    class ManagedTextureVariable
    {
    public:
        /// default constructor
        ManagedTextureVariable() {};
        /// constructor
        ManagedTextureVariable(const Ptr<Resources::ManagedTexture>& tex, const Ptr<CoreGraphics::ShaderVariable>& var) :
            managedTexture(tex),
            shaderVariable(var)
            { };

            Ptr<Resources::ManagedTexture> managedTexture;
            Ptr<CoreGraphics::ShaderVariable> shaderVariable;
    };

    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
    Util::Array<ManagedTextureVariable> managedTextureVariables;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderInstance>&
StateNode::GetShaderInstance() const
{
    return this->shaderInstance;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
