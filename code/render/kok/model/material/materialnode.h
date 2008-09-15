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
