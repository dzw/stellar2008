//------------------------------------------------------------------------------
//  managedmodel.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/managedchunkcacha.h"

namespace Terrain
{
ImplementClass(Terrain::ManagedChunkCacha, 'MDCC', Resources::ManagedResource);

//------------------------------------------------------------------------------
/**
*/
void 
ManagedChunkCacha::CreateCacha(const Resources::ResourceId& resId, SizeT chunkNum)
{
	n_assert(!this->resource.isvalid());
	this->resource = Model::Create();
	Ptr<Model> model = this->resource.downcast<Model>();

	Ptr<ModelNode> cachaNode = TerrainChunkNode::Create();
	cachaNode->CreateChunkCachca(chunkNum);
	model->AttachNode(cachaNode);

	/*Ptr<ModelInstance> modeInstance = model->CreateInstance();
	for (SizeT i = 0; i < chunkNum; i++)
	{
		cachaNode->CreateNodeInstance();
	}*/
}

} // namespace WOW