#pragma once
#ifndef MODELS_MATERIALNODE_H
#define MODELS_MATERIALNODE_H
//------------------------------------------------------------------------------
/**
    @class Models::MaterialNode
  
    ���ʽڵ㣬��StateNode��һ��ͬ��֮���ͬ�������NODE����ÿ�ζ�����shaderInstance
	���Ǳ���ÿ��״̬��Ϣ���õ�ʱ�����ύ������һ��node�п����ж����ͬ�Ĳ��ʡ�
    
	������һ��node��һ�����ʣ�����KOK��һ����ģ�����˶�����ʣ�������Ҫ��Щ�ı䣬
	֧��һ��NODE��ʹ�ö�����ʵĹ��ܡ�

	���ݵ�ǰ����ʹ���ĸ����ʣ�Ȼ����ApplySharedState��Ӧ�á�

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
	
	/// ����Ŀǰ�����ײ���
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
	/// ֧�ֶ����
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
