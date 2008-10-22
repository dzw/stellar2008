//------------------------------------------------------------------------------
//  ParticleNodeInstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/particle/particlenodeinstance.h"
#include "coregraphics/transformdevice.h"
#include "models/modelnode.h"

namespace KOK
{
ImplementClass(KOK::ParticleNodeInstance, 'PLIN', KOK::MaterialNodeInstance);

using namespace CoreGraphics;
using namespace Models;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
ParticleNodeInstance::ParticleNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ParticleNodeInstance::~ParticleNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeInstance::Render()
{
	
}

void
ParticleNodeInstance::UpdateBuffer()
{
	if (emitter == NULL)
		return;

	emitter->UpdateParticles();
	const Ptr<VertexBufferPool>& vertexPool = ParticleServer::Instance()->GetVertexBufferPool();
	if (vertexPool.isvalid())
	{
		void* ptr;
		SizeT startVertex;

		SizeT vertexSize = (SizeT)emitter->GetVertexSize();
		if (vertexPool->CheckSpace(vertexSize) == false)
			vertexPool->FlushAtFrameStart();
		
		vertexPool->Lock(ptr, vertexSize, startVertex);
		Memory::Copy(emitter->GetParitcleVertex(), ptr, vertexSize*sizeof(ParticleFVF));
		vertexPool->Unlock();

		/*group.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		group.SetBaseVertex(vertexStart);
		group.SetNumVertices(vertexSize);
		group.SetBaseIndex(0);
		group.SetNumIndices();*/
	}
}

void
ParticleNodeInstance::ApplyState()
{
	MaterialNodeInstance::ApplyState();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
	this->modelNode->AddVisibleNodeInstance(frameIndex, this);
}

void 
ParticleNodeInstance::Update()
{
	MaterialNodeInstance::Update();

	emitter->FrameMove((float)this->GetModelInstance()->GetTime());
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(node->IsA(ParticleNode::RTTI));
	MaterialNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);

	CreateMaterial();
}

void
ParticleNodeInstance::CreateMaterial()
{
	this->diffMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));

	const Ptr<ThingInstance>& inst = this->GetModelInstance().downcast<ThingInstance>();
	String path = inst->GetFilePath();
	if (path.Length() == 0)
		path = "etext:";
	
	CreateTexture(path);
}

}
