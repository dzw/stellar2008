#pragma once
#ifndef MODELS_CHUNK_H
#define MODELS_CHUNK_H
//------------------------------------------------------------------------------
/**
	��ʾһ��adt�ļ���Դ����������ADT�ļ���
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

    