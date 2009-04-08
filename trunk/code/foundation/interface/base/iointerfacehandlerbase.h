#pragma once
#ifndef BASE_IOINTERFACEHANDLERBASE_H
#define BASE_IOINTERFACEHANDLERBASE_H
//------------------------------------------------------------------------------
/**
    @class IO::IOInterfaceHandlerBase
    
    Handler class for io interfaces.
    
    (C) 2006 Radon Labs GmbH
*/
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
#include "interface/iomsg/mountmpqarchive.h"
#include "interface/iomsg/parsestream.h"

//------------------------------------------------------------------------------
namespace Base
{
    
class IOInterfaceHandlerBase : public Messaging::Handler
{
    DeclareClass(IOInterfaceHandlerBase);
public:
    /// constructor
    IOInterfaceHandlerBase();
    /// destructor
    virtual ~IOInterfaceHandlerBase();
    
    /// open the handler
    virtual void Open();
    /// close the handler
    virtual void Close();
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    /// handle CreateDirectory message
    void OnCreateDirectory(const Ptr<Interface::CreateDirectory>& msg);
    /// handle DeleteDirectory message
    void OnDeleteDirectory(const Ptr<Interface::DeleteDirectory>& msg);
    /// handle DeleteFile message
    void OnDeleteFile(const Ptr<Interface::DeleteFile>& msg);
    /// handle WriteStream message
    void OnWriteStream(const Ptr<Interface::WriteStream>& msg);
    /// handle ReadStream message
    void OnReadStream(const Ptr<Interface::ReadStream>& msg);
    /// handle CopyFile message
    void OnCopyFile(const Ptr<Interface::CopyFile>& msg);
    /// handle MountZipArchive message
    void OnMountZipArchive(const Ptr<Interface::MountZipArchive>& msg);
	/// handle MountMPQArchive message
	void OnMountMPQArchive(const Ptr<Interface::MountMPQArchive>& msg);
	///
	void OnParseStream(const Ptr<Interface::ParseStream>& msg);

    Ptr<IO::Console> console;
    Ptr<IO::IoServer> ioServer;
};


} // namespace IO
//------------------------------------------------------------------------------
#endif
    
    