#pragma once
#ifndef COREGRAPHICS_FONT_H
#define COREGRAPHICS_FONT_H
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::VertexLayout
    
    Describe the layout of vertices in a vertex buffer.
    
    (C) 2006 Radon Labs GmbH
*/
#if __WIN32__
#include "coregraphics/d3d9/d3d9font.h"
namespace CoreGraphics
{
class Font : public Direct3D9::D3D9Font
{
    DeclareClass(Font);
};
}
#elif __XBOX360__

#else
#error "VertexLayout class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
#endif
