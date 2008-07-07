#pragma once
#ifndef WOW_MAPTILE_H
#define WOW_MAPTILE_H
//------------------------------------------------------------------------------
/**
	��ʾһ��adt�ļ���Դ����������ADT�ļ���
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

    