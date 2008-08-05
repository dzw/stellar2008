#pragma once
#ifndef INTERFACE_MOUNTLPQARCHIVE_H
#define INTERFACE_MOUNTLPQARCHIVE_H
//------------------------------------------------------------------------------
/**
    @class Interface::MountLpqArchive
    
    (C) 2008 cTuo
*/
#include "interface/iomsg/iomessage.h"

//------------------------------------------------------------------------------
namespace Interface
{
class MountLPQArchive : public IOMessage
{
    DeclareClass(MountLPQArchive);
    DeclareMsgId;
};

} // namespace IO
//------------------------------------------------------------------------------
#endif
    