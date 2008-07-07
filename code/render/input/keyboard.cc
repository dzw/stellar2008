//------------------------------------------------------------------------------
//  keyboard.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "input/keyboard.h"
#if __WIN32__
namespace Input
{
ImplementClass(Input::Keyboard, 'KBRD', Base::KeyboardBase);
}
#elif __XBOX360__
namespace Input
{
ImplementClass(Input::Keyboard, 'KBRD', Xbox360::Xbox360Keyboard);
}
#elif __WII__
namespace Input
{
ImplementClass(Input::Keyboard, 'KBRD', Wii::WiiKeyboard);
}
#else
#error "Keyboard class not implemented on this platform!"
#endif
