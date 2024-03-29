//------------------------------------------------------------------------------
//  rendertarget.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/rendertarget.h"
#if __WIN32__
namespace CoreGraphics
{
ImplementClass(CoreGraphics::RenderTarget, 'RTGT', Direct3D9::D3D9RenderTarget);
}
#elif __XBOX360__
namespace CoreGraphics
{
ImplementClass(CoreGraphics::RenderTarget, 'RTGT', Xbox360::Xbox360RenderTarget);
}
#elif __WII__
namespace CoreGraphics
{
ImplementClass(CoreGraphics::RenderTarget, 'RTGT', Wii::WiiRenderTarget);
}
#else
#error "RenderTarget class not implemented on this platform!"
#endif
