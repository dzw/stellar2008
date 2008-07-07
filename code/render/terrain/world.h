#pragma once
#ifndef WOW_WORLD_H
#define WOW_WORLD_H
//------------------------------------------------------------------------------
/**
	¼ÓÔØwdtÎÄ¼þ

    (C) cTuo
*/
#include "resources/resource.h"
#include "math/bbox.h"
#include "math/vector3.h"
#include "math/vector.h"
#include "graphics/modelentity.h"


//------------------------------------------------------------------------------
namespace Terrain
{
class World : public Resources::Resource
{
    DeclareClass(World);
public:
    /// constructor
    World();
    /// destructor
    virtual ~World();

    /// unload the resource, or cancel the pending load
    virtual void Unload();

	bool IsValidTile(int i, int j)const;
protected:
	bool maps[64][64];
	Util::String name;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
World::IsValidTile(int i, int j)const
{
	return this->maps[i][j];
}

//------------------------------------------------------------------------------
/**
*/
//inline bool 
//World::IsLoaded()
//{
//	n_assert(0 != wdt && 0 != maptile);
//	if (wdt->IsLoaded() && maptile->IsLoaded())
//		return true;
//	return false;
//}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    