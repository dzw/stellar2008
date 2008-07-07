#pragma once
#ifndef INTERFACE_MOUNTMPQARCHIVE_H
#define INTERFACE_MOUNTMPQARCHIVE_H
//------------------------------------------------------------------------------
/**
    @class Interface::MountZipArchive
    
    IOInterface message to mount a zip archive.
    
    (C) 2007 Radon Labs GmbH
*/
#include "interface/iomsg/iomessage.h"

//------------------------------------------------------------------------------
namespace Interface
{
class MountMPQArchive : public IOMessage
{
    DeclareClass(MountMPQArchive);
    DeclareMsgId;
};

} // namespace IO
//------------------------------------------------------------------------------
#endif
    