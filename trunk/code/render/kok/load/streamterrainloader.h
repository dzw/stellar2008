#pragma once
#ifndef KOK_STREAMTERRAINLOADER_H
#define KOK_STREAMTERRAINLOADER_H
//------------------------------------------------------------------------------
/**
    @class Models::StreamTerrainLoader
    
    (C) 2008 cTuo
*/
#include "resources/resourceloader.h"
#include "io/stream.h"
#include "interface/iomsg/readstream.h"

//------------------------------------------------------------------------------
namespace KOK
{
class StreamTerrainLoader : public Resources::ResourceLoader
{
    DeclareClass(StreamTerrainLoader);
public:
    /// constructor
    StreamTerrainLoader();
    /// destructor
    virtual ~StreamTerrainLoader();
    
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
    bool SetupFromStream(const Ptr<IO::Stream>& stream);

    Ptr<Interface::ReadStream> readStreamMsg;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    