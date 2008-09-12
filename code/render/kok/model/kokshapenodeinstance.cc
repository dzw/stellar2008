//------------------------------------------------------------------------------
//  KokShapeNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/kokshapenodeinstance.h"
#include "kok/model/kokshapenode.h"
#include "coregraphics/transformdevice.h"

#include "coregraphics/texture.h"
#include "resources/resourcemanager.h"

namespace KOK
{
ImplementClass(KOK::KokShapeNodeInstance, 'KSNI', Models::TransformNodeInstance);

using namespace Models;
using namespace CoreGraphics;
using namespace Resources;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
KokShapeNodeInstance::KokShapeNodeInstance():
	shaderInstance(0),
	diffuseColor(0),
	ambientColor(0),
	specularColor(0),
	emissiveColor(0),
	diffMap(0),
	tex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
KokShapeNodeInstance::~KokShapeNodeInstance()
{
	this->shaderInstance = 0;
	this->diffuseColor   = 0;
	this->ambientColor   = 0;
	this->specularColor  = 0;
	this->emissiveColor  = 0;

	if (this->tex.isvalid())
	{
		ResourceManager::Instance()->DiscardManagedResource(this->tex.upcast<ManagedResource>());
		this->tex = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNodeInstance::Render()
{
	//this->modelNode.downcast<KokShapeNode>()->Render();
}    

void
KokShapeNodeInstance::ApplyState()
{
	TransformNodeInstance::ApplyState();

	//TransformDevice* transformDevice = TransformDevice::Instance();
	//transformDevice->ApplyModelTransforms();
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
KokShapeNodeInstance::Update()
{
	TransformNodeInstance::Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
KokShapeNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(node->IsA(KokShapeNode::RTTI));
	TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	CreateMaterial();
}

void
KokShapeNodeInstance::CreateMaterial()
{
	this->shaderInstance = this->modelNode.downcast<KokShapeNode>()->GetShaderInstance();
	n_assert(this->shaderInstance.isvalid());

	this->diffuseColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffuseColor"));
	this->ambientColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("AmbientColor"));
	this->specularColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("SpecularColor"));
	this->emissiveColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("EmissiveColor"));
	this->diffMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));
}

//------------------------------------------------------------------------------
/**
	每个node使用相同的材质，但是可能有不同的纹理，所以把纹理放这里
*/
void 
KokShapeNodeInstance::LoadTexture(const String& path, int texId)
{
	const Ptr<KokShapeNode>& node = this->modelNode.downcast<KokShapeNode>();

	// 特效不在这里加载
	if (node->GetEffectType() > 0)
		return;

	if (path.Length() == 0)
		return;
	
	const cMaterial* materials = node->GetMaterial();
	String fileName;
	for (int i = 0; i < node->GetMaterialSize(); i++)
	{
		if (materials[i].m_pszTextName.Length() == 0)
			continue;

		// 测试代码，水不要在这里加载
		if (strstr(materials[i].m_pszTextName.AsCharPtr(), "Water"))
			continue;
		// 这个纹理找不到，暂时屏掉
		if (strstr(materials[i].m_pszTextName.AsCharPtr(), "m_hbs053"))
			continue;


		/*if (texId != -1)
			materials[i].iNo = texId;*/
		fileName.Format("%s%s%02d.dds", path.AsCharPtr(), materials[i].m_pszTextName.AsCharPtr(), texId);
		this->tex = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, fileName).downcast<ManagedTexture>();
	}
}

}
