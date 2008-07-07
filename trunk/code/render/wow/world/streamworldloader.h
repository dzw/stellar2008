#pragma once
#ifndef WOW_STREAMWORLDLOADER_H
#define WOW_STREAMWORLDLOADER_H
//------------------------------------------------------------------------------
/**
    @class Models::StreamWorldLoader
    
    A ResourceLoader class for setting up Models from Streams. Supports
    Nebula3 binary and XML formats, and the legacy Nebula3 nvx2 format.
    Relies on StreamReader classes which implement the actual stream
    parsing.
    
    (C) 2007 Radon Labs GmbH
*/
#include "resources/resourceloader.h"
#include "io/stream.h"
#include "interface/iomsg/readstream.h"

//------------------------------------------------------------------------------
namespace WOW
{
class StreamWorldLoader : public Resources::ResourceLoader
{
    DeclareClass(StreamWorldLoader);
public:
    /// constructor
    StreamWorldLoader();
    /// destructor
    virtual ~StreamWorldLoader();
    
    /// return true if asynchronous loading is supported
    virtual bool CanLoadAsync() const;
    /// called by resource when a load is requested
    virtual bool OnLoadRequested();
    /// called by resource to cancel a pending load
    virtual void OnLoadCancelled();
    /// call frequently while after OnLoadRequested() to put Resource into loaded state
    virtual bool OnPending();

private:
    /// setup Model resource from stream
    bool SetupModelFromStream();

	Ptr<Interface::IOMessage> readStreamMsg;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    