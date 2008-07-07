#pragma once
#ifndef MODELS_MAPTILE_H
#define MODELS_MAPTILE_H
//------------------------------------------------------------------------------
/**
	��ʾһ��adt�ļ���Դ����������ADT�ļ���
*/
#include "resources/resource.h"
#include "math/vector.h"
#include "math/bbox.h"
#include "util/array.h"
#include "wow/world/adt/maptile.h"
#include "wow/world/Terrain/terrainchunk.h"

//------------------------------------------------------------------------------
namespace Models
{
class MapTile : public Resources::Resource
{
    DeclareClass(MapTile);
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    