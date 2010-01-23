#pragma once
#ifndef WOW_STREAMM2LOADER_H
#define WOW_STREAMM2LOADER_H
//------------------------------------------------------------------------------
/**
    @class Models::StreamM2Loader

	cTuo 2010
*/
#include "resources/resourceloader.h"
#include "io/stream.h"
#include "interface/iomsg/readstream.h"

//------------------------------------------------------------------------------
namespace WOW
{
class StreamM2Loader : public Resources::ResourceLoader
{
    DeclareClass(StreamM2Loader);
public:
    /// constructor
    StreamM2Loader();
    /// destructor
    virtual ~StreamM2Loader();
    
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
    bool SetupModelFromStream(const Ptr<IO::Stream>& stream);
	/// setup mesh from m2 file in memory
	bool SetupMeshFromM2(const Ptr<IO::Stream>& stream);

    Ptr<Interface::ReadStream> readStreamMsg;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    