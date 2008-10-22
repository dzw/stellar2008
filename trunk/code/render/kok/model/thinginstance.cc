//------------------------------------------------------------------------------
//  thinginstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/thinginstance.h"
#include "kok/model/thing.h"
#include "kok/model/thingnode.h"
#include "kok/model/thingnodeinstance.h"
#include "kok/model/modeldef.h"

namespace KOK
{
ImplementClass(KOK::ThingInstance, 'THIE', Models::ModelInstance);

using namespace Models;
using namespace Graphics;
using namespace Util;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
ThingInstance::ThingInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
ThingInstance::~ThingInstance()
{
}

void
ThingInstance::SetTexture(int nodeId, int texId)
{
	// ����ÿ��node���ʵ�����
	const Array<Ptr<ModelNodeInstance> >& nodeInstances = this->GetNodeInstances();
	const Array<Ptr<ModelNode>>& nodes = this->GetModel()->GetNodes();

	if (nodeId >= nodes.Size() || nodeId < 0)
		return;
	if (texId >= 100 || texId < 0)
		return;

	// node��nodeInstance��������ȣ�
	// ����ֻ���ֶ��ҵ�node��Ӧ��nodeinstance��������
	for (SizeT i = 0; i < nodeInstances.Size(); i++)
	{
		if (!nodeInstances[i]->IsVisible())
			continue;

		if (nodes[nodeId] == nodeInstances[i]->GetModelNode())
		{
			const Ptr<ThingNodeInstance>& t = nodeInstances[i].downcast<ThingNodeInstance>();
			t->CreateTexture(filePath, texId);
		}
	}
}

void 
ThingInstance::OnAttachToModel(const Ptr<Model>& model)
{
	n_assert(!this->IsAttachedToModel());
	this->model = model;

	// create ModelInstanceNodes
	Ptr<ModelNodeInstance> parent;
	const Array<Ptr<ModelNode> >& modelNodes = this->model->GetNodes();
	SizeT numModelNodes = modelNodes.Size();
	IndexT modelNodeIndex;
	for (modelNodeIndex = 0; modelNodeIndex < numModelNodes; modelNodeIndex++)
	{
		//const Ptr<ModelNode>& modelNode = modelNodes[modelNodeIndex];
		//
		//// lookup optional parent node instance
		//Ptr<ModelNodeInstance> parentNodeInst;
		//if (modelNode->HasParent())
		//{
		//	// NOTE: it is guaranteed that the model node array and the
		//	// model node instance array have the same layout, so we
		//	// can re-use an index into the model node array into
		//	// the model node instance array
		//	const Ptr<ModelNode>& parentNode = modelNode->GetParent();
		//	IndexT parentIndex = modelNodes.FindIndex(parentNode);
		//	n_assert(InvalidIndex != parentIndex);
		//	parentNodeInst = this->nodeInstances[parentIndex];
		//};
		
		const Ptr<ThingNode>& modelNode = modelNodes[modelNodeIndex].downcast<ThingNode>();
		switch (modelNode->GetModelType())
		{
		// ���¼������͵���ģ�Ͳ���Ҫʵ��
		case TYPE_LIGHT:
		case TYPE_SELECT:
		case TYPE_WATER:
		case TYPE_EFF:
		case TYPE_EFLAME:
			continue;
			break;
		}
		
		Ptr<ModelNodeInstance> nodeInstance = modelNode->CreateNodeInstance();
		nodeInstance->OnAttachToModelInstance(this, modelNodes[modelNodeIndex], parent);
		this->nodeInstances.Append(nodeInstance);
	}

	filePath = model->GetResourceId().Value().ExtractDirName();
}

//------------------------------------------------------------------------------
/**
    
*/
void
ThingInstance::Update()
{
	IndexT i;
	for (i = 0; i < this->nodeInstances.Size(); i++)
	{
		if (this->nodeInstances[i]->IsVisible())
			this->nodeInstances[i]->Update();
	}
}

//------------------------------------------------------------------------------
/**

*/
void
ThingInstance::NotifyVisible(IndexT frameIndex)
{
	IndexT i;
	SizeT num = this->nodeInstances.Size();
	for (i = 0; i < num; i++)
	{
		if (this->nodeInstances[i]->IsVisible())
			this->nodeInstances[i]->OnNotifyVisible(frameIndex);
	}
}

//------------------------------------------------------------------------------
/**
	һ��ģ���п��ܴ��ںܶ���ģ�ͣ�����ֻ������һ����ʾ������
	ͨ��repeatId��ȷ��������ʾ�ĸ�������TYPE_REPEAT���͵���ģ�ͣ�ֻ��һ������ʾ��
*/
void
ThingInstance::SetVisiableRepeat(int id)
{
	const Array<Ptr<ModelNodeInstance> >& nodeInstances = this->GetNodeInstances();

	if (id < 0)
		id = 0;
	if (id >= nodeInstances.Size())
		id = nodeInstances.Size() - 1;

	String repeatName, name;
	repeatName.Format("m_repeat%02d", id);

	for (SizeT i = 0; i < nodeInstances.Size(); i++)
	{
		// �ų�����ϵͳ
		if (!nodeInstances[i]->IsA(ThingNodeInstance::RTTI))
		{
			nodeInstances[i]->SetVisible(true);
			continue;
		}

		const Ptr<ThingNode>& thingNode = nodeInstances[i]->GetModelNode().downcast<ThingNode>();
		if (thingNode->GetModelType() == TYPE_REPEAT)
		{
			name = thingNode->GetName().Value();
			if (name.CheckStringExist(repeatName))
				nodeInstances[i]->SetVisible(true);
			else
				nodeInstances[i]->SetVisible(false);
		}
		else if (thingNode->GetModelType() == TYPE_HD || 
			thingNode->GetModelType() == TYPE_NORMAL)
		{
			nodeInstances[i]->SetVisible(true);
		}
		else
			nodeInstances[i]->SetVisible(false);

		if (nodeInstances[i]->IsVisible())
		{
			const Ptr<ThingNodeInstance>& t = nodeInstances[i].downcast<ThingNodeInstance>();
			t->CreateMesh();
		}
	}
}

} // namespace Models
