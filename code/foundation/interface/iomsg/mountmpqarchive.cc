//------------------------------------------------------------------------------
//  mountziparchive.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iomsg/mountmpqarchive.h"

namespace Interface
{
ImplementClass(Interface::MountMPQArchive, 'MMQA', Interface::IOMessage);
ImplementMsgId(MountMPQArchive);
} // namespace IO
