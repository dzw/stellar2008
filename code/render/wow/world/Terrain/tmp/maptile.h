#pragma once
#ifndef WOW_MAPTILE_H
#define WOW_MAPTILE_H
//------------------------------------------------------------------------------
/**
	表示一个adt文件资源，用来加载ADT文件。
*/
#include "resources/resource.h"

//------------------------------------------------------------------------------
namespace WOW
{
class MapTile : public Resources::Resource
{
    DeclareClass(MapTile);
public:
    /// constructor
    MapTile();
    /// destructor
    virtual ~MapTile();

	Util::Array<Ptr<ModelNode>> nodes;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    