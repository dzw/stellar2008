//------------------------------------------------------------------------------
//  readstream.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iomsg/parsestream.h"

namespace Interface
{
ImplementAbstractClass(Interface::ParseStream, 'PEST', Interface::IOMessage);
ImplementMsgId(ParseStream);

} // namespace IO
