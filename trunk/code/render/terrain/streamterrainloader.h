#pragma once
#ifndef TERRAIN_STREAMTERRAINLOADER_H
#define TERRAIN_STREAMTERRAINLOADER_H
//------------------------------------------------------------------------------
/**
    @class 
    
    (C) 
*/
#include "resources/resourceloader.h"
#include "io/stream.h"
#include "interface/iomsg/readstream.h"

//------------------------------------------------------------------------------
namespace Terrain
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
    bool SetupModelFromStream();

	Ptr<Interface::IOMessage> readStreamMsg;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    