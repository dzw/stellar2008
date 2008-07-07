#pragma once
#ifndef WOW_CLOSEMAPTILE_H
#define WOW_CLOSEMAPTILE_H
//------------------------------------------------------------------------------
/**
    @class Interface::ReadStream
    
    IOInterface message to asynchronously read a stream. Note that the
    caller must provide a mappable stream which consumes the read
    content.
        
    (C) 2006 Radon Labs GmbH
*/
#include "interface/iomsg/iomessage.h"

//------------------------------------------------------------------------------
namespace WOW
{
class CloseMapTile : public Interface::IOMessage
{
    DeclareClass(CloseMapTile);
    DeclareMsgId;
};

} // namespace Interface
//------------------------------------------------------------------------------
#endif
