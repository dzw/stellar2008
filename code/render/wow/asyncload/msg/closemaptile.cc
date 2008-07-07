//------------------------------------------------------------------------------
//  adtreadstream.cc
//  cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/asyncload/msg/closemaptile.h"

namespace WOW
{
ImplementClass(WOW::CloseMapTile, 'CMPE', Interface::IOMessage);
ImplementMsgId(CloseMapTile);
} // namespace IO
