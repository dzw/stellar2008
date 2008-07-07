//------------------------------------------------------------------------------
//  vertexlayout.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/font.h"
#if __WIN32__
namespace CoreGraphics
{
ImplementClass(CoreGraphics::Font, 'FOTC', Direct3D9::D3D9Font);
}
#elif __XBOX360__

#else
#error "VertexLayout class not implemented on this platform!"
#endif

