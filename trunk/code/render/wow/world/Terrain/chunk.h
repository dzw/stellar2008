#pragma once
#ifndef MODELS_CHUNK_H
#define MODELS_CHUNK_H
//------------------------------------------------------------------------------
/**
	表示一个adt文件资源，用来加载ADT文件。
*/
#include "resources/resource.h"
#include "wow/world/Terrain/terrainchunk.h"

//------------------------------------------------------------------------------
namespace Models
{
class Chunk : public Resources::Resource
{
    DeclareClass(Chunk);
public:
    /// constructor
    Chunk();
    /// destructor
    virtual ~Chunk();
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    