#pragma once
#ifndef GRAPHICS_MAPTILEENTITY_H
#define GRAPHICS_MAPTILEENTITY_H
//------------------------------------------------------------------------------
/**
*/
#include "graphics/Modelentity.h"
#include "resources/resourceid.h"
#include "coregraphics/mesh.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class MapTileEntity : public ModelEntity
{
    DeclareClass(MapTileEntity);
public:
    /// constructor
    MapTileEntity();
    /// destructor
    virtual ~MapTileEntity();

	///
	void Reset(const Resources::ResourceId& resId, int x, int y);
protected:

	int x, y;
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    