#pragma once
#ifndef MODELS_MATERIALNODE_H
#define MODELS_MATERIALNODE_H
//------------------------------------------------------------------------------
/**
    @class Models::MaterialNode
  
    材质节点，与StateNode有一曲同工之妙，不同的是这个NODE不会每次都创建shaderInstance
	而是保存每个状态信息，用的时候再提交。而且一个node中可以有多个不同的材质。
    
	理论上一个node就一个材质，不过KOK中一个子模型用了多个材质，所以需要做些改变，
	支持一个NODE中使用多个材质的功能。

	根据当前设置使用哪个材质，然后在ApplySharedState中应用。

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
	
	/// 设置目前用哪套材质
	/*void SetCurrentMaterial(int cur);
	SizeT GetMaterialSize()const;
	const Util::Array<Material>& GetMaterials()const*/;
protected:
    /// create a model node instance
    virtual Ptr<ModelNodeInstance> CreateNodeInstance() const;
    /// called when resources should be loaded
    virtual void LoadResources();
    /// called when resources should be unloaded
    virtual void UnloadResources();

	/*struct Material 
	{
		Util::String name;
		Ptr<CoreGraphics::ShaderInstance> shader;
	};*/

	IndexT curMaterial;
    Ptr<CoreGraphics::ShaderInstance> shaderInstance;
	/// 支持多材质
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
//
//inline void 
//MaterialNode::SetCurrentMaterial(int cur)
//{
//	this->curMaterial = cur;
//}
//
//SizeT 
//MaterialNode::GetMaterialSize()const
//{
//	return this->materialList.Size();
//}
//
//const Util::Array<Material>& 
//MaterialNode::GetMaterials()const
//{
//	return this->materialList;
//}

} // namespace Models
//------------------------------------------------------------------------------
#endif
