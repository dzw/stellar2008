//------------------------------------------------------------------------------
//  ParticleNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/particle/particlenode.h"
#include "kok/model/particle/particlenodeinstance.h"

namespace KOK
{
ImplementClass(KOK::ParticleNode, 'PLNE', KOK::MaterialNode);

using namespace IO;
using namespace Util;
using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Memory;
using namespace CoreGraphics;
using namespace Attr;

//------------------------------------------------------------------------------
/**
*/
ParticleNode::ParticleNode()
{
}

//------------------------------------------------------------------------------
/**
*/
ParticleNode::~ParticleNode()
{
	
}

//------------------------------------------------------------------------------
/**
*/
Resource::State
ParticleNode::GetResourceState() const
{
	return Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNode::LoadResources()
{
    // call parent class
    MaterialNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNode::UnloadResources()
{    
    MaterialNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
ParticleNode::CreateNodeInstance()const
{
	Ptr<ModelNodeInstance> instance = (ModelNodeInstance*)ParticleNodeInstance::Create();
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
bool
ParticleNode::ApplySharedState()
{
	MaterialNode::ApplySharedState();

	return true;
}

void 
ParticleNode::SetEmitData(EmitterData* data)
{
	this->emitData = data;

	this->m_dwNumMaterial = data->m_dwNumMaterial;
	this->m_pMaterial = data->m_pMaterial;
	this->m_pMaterialAnimator = data->m_pMaterialAnimator;
}
}