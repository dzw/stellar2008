#pragma once
#ifndef MODELS_MATERIALNODEINSTANCE_H
#define MODELS_MATERIALNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class Models::MaterialNodeInstance

    根据Material设置材质
    渲染前设置shader&state，可实现多材质渲染（KokShapeNode需要)

	使用方法：
	假设某个node需要设置一些渲染状态或材质，就调用CreateShaderVariableInstance并传入
	Semantic，为这个NODE建立一系列状态实例(ShaderVariableInstance)，应用的时候就把这
	些实例更新到GPU，所以每个NODEINSTANCE都可以有不同的效果。

	如：
	在kokshapenodeinstance中Render的时候，有多个材质：

	// 创建变量
	this->diffColor = CreateShaderVariableInstance(ShaderVariable::Semantic("diffuseColor"));
	...
	for (i = 0; i < materialNum; i++)
	{
		// 渲染每个材质
	    //setupMaterial(material[i]);
		this->diffColor->SetVector(material[i].diffuseColor);
		this->diffColor->Apply();
		...
		
		render();
	}
    (C) 2008 cTuo
*/
#include "models/nodes/transformnodeinstance.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shadervariableinstance.h"

//------------------------------------------------------------------------------
namespace Models
{
class MaterialNodeInstance : public TransformNodeInstance
{
    DeclareClass(MaterialNodeInstance);
public:
    /// constructor
    MaterialNodeInstance();
    /// destructor
    virtual ~MaterialNodeInstance();

    /// apply per-instance state prior to rendering
    virtual void ApplyState();

    /// instanciate a shader variable by semantic
    Ptr<CoreGraphics::ShaderVariableInstance> CreateShaderVariableInstance(const CoreGraphics::ShaderVariable::Semantic& semantic);
    /// return true if a shader variable has been instantiated
    bool HasShaderVariableInstance(const CoreGraphics::ShaderVariable::Semantic& semantic) const;
    /// get a shader variable instance
    const Ptr<CoreGraphics::ShaderVariableInstance>& GetShaderVariableInstance(const CoreGraphics::ShaderVariable::Semantic& semantic) const;

protected:
    /// called when attached to ModelInstance
    virtual void OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void OnRemoveFromModelInstance();

    Util::Dictionary<CoreGraphics::ShaderVariable::Semantic, Ptr<CoreGraphics::ShaderVariableInstance> > shaderVariableInstances;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
