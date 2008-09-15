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

	/// ���ÿ�����ʵ�����NODE����ʹ����ͬ�Ĳ��ʣ������в�ͬ������
	struct MaterialInstance 
	{
		/// ��ͨ����
		Ptr<Resources::ManagedTexture> diffMap;
		/// ������������
		Ptr<Resources::ManagedTexture> fakeReflect;
	};
	/// �ù̶����飬��material����һ��
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
