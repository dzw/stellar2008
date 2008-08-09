//------------------------------------------------------------------------------
//  modelinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terraininstance.h"

namespace KOK
{
ImplementClass(KOK::TerrainInstance, 'TNIE', Models::ModelInstance);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
TerrainInstance::TerrainInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TerrainInstance::~TerrainInstance()
{
}

//------------------------------------------------------------------------------
/**
    
*/
void
TerrainInstance::Update()
{
	// UpdateDistrict

	ModelInstance::Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainInstance::RenderDebug()
{
    IndexT i;
    SizeT num = this->nodeInstances.Size();
    for (i = 0; i < num; i++)
    {
        this->nodeInstances[i]->RenderDebug();
    }
}

} // namespace Models
