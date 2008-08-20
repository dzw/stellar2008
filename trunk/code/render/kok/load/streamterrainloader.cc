//------------------------------------------------------------------------------
//  streammodelloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/load/streamterrainloader.h"
#include "interface/iointerface.h"
#include "interface/iomsg/readstream.h"
#include "io/memorystream.h"
#include "models/model.h"
#include "models/load/binarymodelreader.h"
#include "models/load/xmlmodelreader.h"
#include "models/load/n2modelreader.h"
#include "attr/attributecontainer.h"
//#include "models/nodes/shapenode.h"
#include "kok/load/terrainreader.h"

namespace KOK
{
ImplementClass(KOK::StreamTerrainLoader, 'KTDL', Resources::ResourceLoader);

using namespace Core;
using namespace Messaging;
using namespace IO;
using namespace Util;
using namespace Resources;
using namespace Interface;
using namespace Attr;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
StreamTerrainLoader::StreamTerrainLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StreamTerrainLoader::~StreamTerrainLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamTerrainLoader::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamTerrainLoader::OnLoadRequested()
{
    n_assert(this->GetState() == Resource::Initial);
    n_assert(this->resource.isvalid());
    if (this->resource->IsAsyncEnabled())
    {
        // perform asynchronous load
        // send off an asynchronous loader job
        n_assert(!this->readStreamMsg.isvalid());
        this->readStreamMsg = ReadStream::Create();
        this->readStreamMsg->SetURI(this->resource->GetResourceId().Value());
        this->readStreamMsg->SetStream(MemoryStream::Create());
        IOInterface::Instance()->Send(this->readStreamMsg.upcast<Message>());

        // go into Pending state
        this->SetState(Resource::Pending);
        return true;
    }
    else
    {
        // perform synchronous load
        Ptr<Stream> stream = IoServer::Instance()->CreateStream(this->resource->GetResourceId().Value());
        if (this->SetupFromStream(stream))
        {
            this->SetState(Resource::Loaded);
            return true;
        }
        // fallthrough: synchronous loading failed
        this->SetState(Resource::Failed);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamTerrainLoader::OnLoadCancelled()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    IOInterface::Instance()->Cancel(this->readStreamMsg.upcast<Message>());
    this->readStreamMsg = 0;
    ResourceLoader::OnLoadCancelled();
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamTerrainLoader::OnPending()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    bool retval = false;

    // check if asynchronous loader job has finished
    if (this->readStreamMsg->Handled())
    {
        // ok, loader job has finished
        if (this->readStreamMsg->GetResult())
        {
            // IO operation was successful
            if (this->SetupFromStream(this->readStreamMsg->GetStream()))
            {
                // everything ok!
                this->SetState(Resource::Loaded);                
                retval = true;
            }
            else
            {
                // this probably wasn't a Model file...
                this->SetState(Resource::Failed);
            }
        }
        else
        {
            // error during IO operation
            this->SetState(Resource::Failed);
        }
        // we no longer need the loader job message
        this->readStreamMsg = 0;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    This method actually setups the Model object from the data in the stream.
*/
bool
StreamTerrainLoader::SetupFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());

    // first decide what ModelReader to use
    bool isLegacyFile = false;
    String fileExt = this->resource->GetResourceId().Value().GetFileExtension();
	fileExt.ToLower();

    Ptr<ModelReader> modelReader;
    if (fileExt == "sce")
    {
        modelReader = (ModelReader*) TerrainReader::Create();
        modelReader->SetModelResId(this->resource->GetResourceId());
    }
    else
    {
        n_error("StreamTerrainLoader: unrecognized file extension '%s' in '%s'!",
            fileExt.AsCharPtr(), this->resource->GetResourceId().Value().AsCharPtr());
        return false;
    }

    // parse the stream
    modelReader->SetStream(stream);
    Ptr<Model> model = this->resource.downcast<Model>();
    modelReader->SetModel(model);
    if (modelReader->Open())
    {
        if (modelReader->FillModel())
        {   
            // load all resources of all nodes like shaderstates etc.
            //model->LoadResources();
        }
        else
        {
            n_error("StreamTerrainLoader::SetupFromStream(): ModelReader::SetToFirstModel() failed for '%s'!",
                this->resource->GetResourceId().Value().AsCharPtr());
        }
    }
    else
    {
        n_error("StreamTerrainLoader::SetupFromStream(): Failed to open '%s' (-> %s) as Model stream!", 
            this->resource->GetResourceId().Value().AsCharPtr(),
            IO::IoServer::Instance()->ResolveAssignsInString(this->resource->GetResourceId().Value()).AsCharPtr());
        return false;
    }
    return true;
}

} // namespace Models
