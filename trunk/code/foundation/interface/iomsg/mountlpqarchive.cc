//------------------------------------------------------------------------------
//  mountziparchive.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iomsg/mountlpqarchive.h"

namespace Interface
{
ImplementClass(Interface::MountLPQArchive, 'MLQA', Interface::IOMessage);
ImplementMsgId(MountLPQArchive);
} // namespace IO
