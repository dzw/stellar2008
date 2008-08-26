#pragma once
#ifndef MODELS_MATERIALNODEINSTANCE_H
#define MODELS_MATERIALNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class Models::MaterialNodeInstance

    ����Material���ò���
    ��Ⱦǰ����shader&state����ʵ�ֶ������Ⱦ��KokShapeNode��Ҫ)

	ʹ�÷�����
	����ĳ��node��Ҫ����һЩ��Ⱦ״̬����ʣ��͵���CreateShaderVariableInstance������
	Semantic��Ϊ���NODE����һϵ��״̬ʵ��(ShaderVariableInstance)��Ӧ�õ�ʱ��Ͱ���
	Щʵ�����µ�GPU������ÿ��NODEINSTANCE�������в�ͬ��Ч����

	�磺
	��kokshapenodeinstance��Render��ʱ���ж�����ʣ�

	// ��������
	this->diffColor = CreateShaderVariableInstance(ShaderVariable::Semantic("diffuseColor"));
	...
	for (i = 0; i < materialNum; i++)
	{
		// ��Ⱦÿ������
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
