//------------------------------------------------------------------------------
//  adtreadstream.cc
//  cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/asyncload/msg/openmaptile.h"

namespace WOW
{
ImplementClass(WOW::OpenMapTile, 'OMTE', Interface::IOMessage);
ImplementMsgId(OpenMapTile);
} // namespace IO
