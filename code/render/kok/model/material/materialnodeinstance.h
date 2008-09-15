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
#include "resources/managedtexture.h"
#include "coregraphics/shaderinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class MaterialNodeInstance : public Models::TransformNodeInstance
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
    Ptr<CoreGraphics::ShaderVariable> CreateShaderVariable(const CoreGraphics::ShaderVariable::Semantic& semantic);

	/// texture
	const Ptr<CoreGraphics::Texture>& GetTexture(IndexT index);
	const Ptr<CoreGraphics::Texture>& GetFakeReflectTexture(IndexT index);
	void CreateTexture(const Util::String& path, int texId = -1);

protected:
    /// called when attached to ModelInstance
	virtual void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void OnRemoveFromModelInstance();

	/// 存放每个材质的纹理，NODE可能使用相同的材质，但是有不同的纹理
	struct MaterialInstance 
	{
		/// 普通纹理
		Ptr<Resources::ManagedTexture> diffMap;
		/// 金属反光纹理
		Ptr<Resources::ManagedTexture> fakeReflect;
	};
	/// 用固定数组，与material数量一致
	Util::FixedArray<MaterialInstance> materialInstances;

	Ptr<CoreGraphics::ShaderInstance> shaderInstance;
};

inline const Ptr<CoreGraphics::Texture>& 
MaterialNodeInstance::GetTexture(IndexT index)
{
	n_assert(index < materialInstances.Size() && materialInstances[index].diffMap.isvalid());
	
	return materialInstances[index].diffMap->GetTexture();
}

inline const Ptr<CoreGraphics::Texture>& 
MaterialNodeInstance::GetFakeReflectTexture(IndexT index)
{
	n_assert(index < materialInstances.Size() && materialInstances[index].fakeReflect.isvalid());

	return materialInstances[index].fakeReflect->GetTexture();
}

}
//------------------------------------------------------------------------------
#endif
