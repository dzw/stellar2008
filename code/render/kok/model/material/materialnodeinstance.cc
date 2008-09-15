//------------------------------------------------------------------------------
//  MaterialNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/material/MaterialNodeInstance.h"
#include "kok/model/material/MaterialNode.h"
#include "coregraphics/shader.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/transformdevice.h"
#include "resources/resourcemanager.h"
#include "io/lpqfs/lpqarchive.h"
#include "io/lpqfs/lpqfilesystem.h"

namespace KOK
{
ImplementClass(KOK::MaterialNodeInstance, 'MNIE', Models::TransformNodeInstance);

using namespace Util;
using namespace CoreGraphics;
using namespace Models;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
MaterialNodeInstance::MaterialNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MaterialNodeInstance::~MaterialNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
    TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	n_assert(node->IsA(MaterialNode::RTTI));
	const Ptr<MaterialNode>& materialNode = node.downcast<MaterialNode>();
	this->materialInstances.SetSize(materialNode->GetMaterialSize());

	this->shaderInstance = node.downcast<MaterialNode>()->GetShaderInstance();
	n_assert(shaderInstance.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNodeInstance::OnRemoveFromModelInstance()
{
	// discard managed textures
	ResourceManager* resManager = ResourceManager::Instance();
	IndexT i;
	for (i = 0; i < this->materialInstances.Size(); i++)
	{
		if (this->materialInstances[i].diffMap.isvalid())
			resManager->DiscardManagedResource(this->materialInstances[i].diffMap.upcast<ManagedResource>());
		if (this->materialInstances[i].fakeReflect.isvalid())
			resManager->DiscardManagedResource(this->materialInstances[i].fakeReflect.upcast<ManagedResource>());
	}
	this->materialInstances.SetSize(0);
	
	this->shaderInstance = 0;

    TransformNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialNodeInstance::ApplyState()
{
    TransformNodeInstance::ApplyState();
 
    // apply any needed model transform state to shader
    TransformDevice* transformDevice = TransformDevice::Instance();
    transformDevice->ApplyModelTransforms();    
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ShaderVariable>
MaterialNodeInstance::CreateShaderVariable(const ShaderVariable::Semantic& sem)
{
    // create new shader variable instance
    n_assert(shaderInstance->HasVariableBySemantic(sem));
    return shaderInstance->GetVariableBySemantic(sem);
}

//------------------------------------------------------------------------------
/**
	texId����ͼ��ţ����������ģ����Ҫ��ţ�����Ч����Ҫ
*/
void 
MaterialNodeInstance::CreateTexture(const Util::String& path, int texId)
{
	if (path.Length() == 0)
		return;

	const Ptr<MaterialNode>& node = this->modelNode.downcast<MaterialNode>();
	const cMaterial* materials = node->GetMaterial();
	String fileName;
	for (int i = 0; i < node->GetMaterialSize(); i++)
	{
		if (materials[i].m_pszTextName.Length() == 0)
			continue;

		// ���Դ��룬ˮ��Ҫ���������
		if (strstr(materials[i].m_pszTextName.AsCharPtr(), "Water"))
			continue;
		// ��������Ҳ�������ʱ����
		if (strstr(materials[i].m_pszTextName.AsCharPtr(), "m_hbs053"))
			continue;


		if (texId == -1)
		{
			fileName.Format("%s%s.dds", path.AsCharPtr(), materials[i].m_pszTextName.AsCharPtr());
		}
		else
		{
			fileName.Format("%s%s%02d.dds", path.AsCharPtr(), materials[i].m_pszTextName.AsCharPtr(), texId);

			// ���жϷ�����ͼ�Ƿ���ڣ�ֻ�д��ڵ�ʱ��ż��أ���ط�������Ч�����⣡��
			String fakeReflectName;
			fakeReflectName.Format("%s%s%02d_s.dds", path.AsCharPtr(), materials[i].m_pszTextName.AsCharPtr(), texId);
			const Ptr<IO::LPQArchive>& pack =  IO::LPQFileSystem::Instance()->FindLPQArchiveByName(fakeReflectName);
			if (pack.isvalid())
				this->materialInstances[i].fakeReflect = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, fakeReflectName).downcast<ManagedTexture>();
		}	
		this->materialInstances[i].diffMap = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, fileName).downcast<ManagedTexture>();
	}
}

} // namespace Models
