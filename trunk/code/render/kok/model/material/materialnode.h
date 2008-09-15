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
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"

//------------------------------------------------------------------------------
namespace KOK
{
class MaterialNode : public Models::TransformNode
{
    DeclareClass(MaterialNode);
public:
    /// constructor
    MaterialNode();
    /// destructor
    virtual ~MaterialNode();

    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();
    /// get pointer to contained shader instance
    const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstance() const;
	
	DWORD GetMaterialSize()const;
	const cMaterial* GetMaterial()const;
protected:
    /// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;
    /// called when resources should be loaded
    virtual void LoadResources();
    /// called when resources should be unloaded
    virtual void UnloadResources();

	/// material
	DWORD m_dwNumMaterial;
	cMaterial* m_pMaterial;
	cMaterialAnimator* m_pMaterialAnimator;

	Ptr<CoreGraphics::ShaderInstance> shaderInstance;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderInstance>&
MaterialNode::GetShaderInstance() const
{
    return this->shaderInstance;
}

inline DWORD 
MaterialNode::GetMaterialSize()const
{
	return this->m_dwNumMaterial;
}

inline const cMaterial* 
MaterialNode::GetMaterial()const
{
	return this->m_pMaterial;
}

}
//------------------------------------------------------------------------------
#endif
