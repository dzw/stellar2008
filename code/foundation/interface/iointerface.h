#pragma once
#ifndef INTERFACE_IOINTERFACE_H
#define INTERFACE_IOINTERFACE_H
//------------------------------------------------------------------------------
/**
    @class IO::Interface
    
    Implements the asynchronous interface to the IO subsystem. This will
    run a minimal Nebula3 runtime with an IO subsystem in an extra thread.
    Communication with the IO::Interface happens by sending messages to
    the Interface object. Messages are guaranteed to be handled sequentially 
    in FIFO order (there's exactly one handler thread which handles all 
    messages).
    
    (C) 2006 Radon Labs GmbH
*/
#include "messaging/asyncport.h"
#include "messaging/handler.h"
#include "io/console.h"
#include "io/ioserver.h"
#include "interface/iomsg/createdirectory.h"
#include "interface/iomsg/deletedirectory.h"
#include "interface/iomsg/deletefile.h"
#include "interface/iomsg/writestream.h"
#include "interface/iomsg/readstream.h"
#include "interface/iomsg/copyfile.h"
#include "interface/iomsg/mountziparchive.h"

//------------------------------------------------------------------------------
namespace Interface
{
class IOInterface : public Messaging::AsyncPort
{
    DeclareClass(IOInterface);
    DeclareInterfaceSingleton(IOInterface);
public:
    /// constructor
    IOInterface();
    /// destructor
    virtual ~IOInterface();
    /// open the interface object
    virtual void Open();

protected:
    /// derive in subclass, create and attach handlers from here
    virtual void OnCreateHandlers();
};

} // namespace IO
//------------------------------------------------------------------------------
#endif
    
    