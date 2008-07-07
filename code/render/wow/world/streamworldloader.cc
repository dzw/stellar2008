//------------------------------------------------------------------------------
//  streammodelloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/streamworldloader.h"
#include "interface/iointerface.h"
#include "interface/iomsg/readstream.h"
#include "io/memorystream.h"
#include "attr/attributecontainer.h"
#include "wow/world/adt/adtreader.h"
#include "wow/world/wdt/wdtreader.h"
#include "wow/world/wdl/wdlreader.h"
//#include "wow/world/worldserver.h"
#include "math/vector3.h"
#include "wow/world/Terrain/terrainchunk.h"
#include "wow/asyncload/msg/chunkreadstream.h"
#include "wow/asyncload/msg/closemaptile.h"
#include "wow/asyncload/msg/openmaptile.h"
#include "wow/world/Terrain/chunk.h"
//#include "wow/asyncload/asyncwowinterface.h"

namespace WOW
{
ImplementClass(WOW::StreamWorldLoader, 'SWDL', Resources::ResourceLoader);

using namespace Core;
using namespace Messaging;
using namespace IO;
using namespace Util;
using namespace Resources;
using namespace Interface;
using namespace Attr;
using namespace Models;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
StreamWorldLoader::StreamWorldLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StreamWorldLoader::~StreamWorldLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamWorldLoader::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamWorldLoader::OnLoadRequested()
{
    n_assert(this->GetState() == Resource::Initial);
    n_assert(this->resource.isvalid());
    if (this->resource->IsAsyncEnabled())
    {
        // perform asynchronous load
        // send off an asynchronous loader job
        n_assert(!this->readStreamMsg.isvalid());
		if (this->resource->GetResourceId().Value().GetFileExtension() == "adt")
		{
			this->readStreamMsg = (Interface::IOMessage*)OpenMapTile::Create();
			this->readStreamMsg->SetURI(this->resource->GetResourceId().Value());
		}
		else if (this->resource->GetResourceId().Value().GetFileExtension() == "chk")
		{
			// load a chunk
            Ptr<ChunkReadStream>ckReadStream = ChunkReadStream::Create();
            ckReadStream->SetURI(this->resource->GetResourceId().Value());
			//ckReadStream->SetChunkId(this->resource.downcast<Chunk>()->terrain->GetChunkId());
			this->readStreamMsg = ckReadStream.upcast<Interface::IOMessage>();
		}
		else
		{
			// 其它模型不在这里加载，代替模型不可用
			this->SetState(Resource::Loaded);
			return true;
			/*this->readStreamMsg = (Interface::IOMessage*)ReadStream::Create();
			this->readStreamMsg->SetURI(this->resource->GetResourceId().Value());
			this->readStreamMsg->SetStream(MemoryStream::Create());*/
		}
        IOInterface::Instance()->Send(this->readStreamMsg.upcast<Message>());
		//AsyncWOWInterface::Instance()->Send(this->readStreamMsg.upcast<Message>());

        // go into Pending state
        this->SetState(Resource::Pending);
        return true;
    }
    else
    {
		// 其它模型不在这里加载，代替模型不可用
		this->SetState(Resource::Loaded);
		return true;
        //// perform synchronous load
        //Ptr<Stream> stream = IoServer::Instance()->CreateStream(this->resource->GetResourceId().Value());
        //if (this->SetupModelFromStream())
        //{
        //    this->SetState(Resource::Loaded);
        //    return true;
        //}
        //// fallthrough: synchronous loading failed
        //this->SetState(Resource::Failed);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamWorldLoader::OnLoadCancelled()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    IOInterface::Instance()->Cancel(this->readStreamMsg.upcast<Message>());
	//AsyncWOWInterface::Instance()->Cancel(this->readStreamMsg.upcast<Message>());
    this->readStreamMsg = 0;
    ResourceLoader::OnLoadCancelled();
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamWorldLoader::OnPending()
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
            if (this->SetupModelFromStream())
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
StreamWorldLoader::SetupModelFromStream()
{
    // first decide what ModelReader to use
    bool isLegacyFile = false;
    String fileExt = this->resource->GetResourceId().Value().GetFileExtension();
    Ptr<ModelReader> modelReader;
	fileExt.ToLower();
	/*if (fileExt == "wmo")
	{
		modelReader = (ModelReader*)WMOReader::Create();
		modelReader->SetModelResId(this->resource->GetResourceId());
		modelReader.downcast<WMOReader>()->SetWMO(this->resource.downcast<WMO>());
	}
	else */if (fileExt == "n2")
	{
		// 替代模型暂时用了n2模型，所以过滤掉
		return true;
	}
	else if (fileExt == "wdt")
	{
		modelReader = (ModelReader*)WDTReader::Create();
		modelReader->SetModelResId(this->resource->GetResourceId());
		//modelReader.downcast<WDTReader>()->SetWDT(this->resource.downcast<WDT>());
	}
	else if (fileExt == "wdl")
	{
		modelReader = (ModelReader*)WDLReader::Create();
		modelReader->SetModelResId(this->resource->GetResourceId());
		//modelReader.downcast<WDLReader>()->SetWDL(this->resource.downcast<WDL>());
	}
	else if (fileExt == "adt")
	{
		// 已在加载线程中解析了数据，在这里加载wmos和models。
		
		return true;
	}
	else if (fileExt == "chk")
	{
		// loaded a chunk
		Ptr<ChunkReadStream> ckmsg = this->readStreamMsg.downcast<ChunkReadStream>();
		Ptr<TerrainChunk> ck = this->resource.downcast<TerrainChunk>();
		ck->UpdateChunks(ckmsg->chunk);
		return true;
	}
    else
    {
        n_error("StreamWorldLoader: unrecognized file extension '%s' in '%s'!",
            fileExt.AsCharPtr(), this->resource->GetResourceId().Value().AsCharPtr());
        return false;
    }

	//n_assert(stream.isvalid());
	//n_assert(stream->CanBeMapped());

    // parse the stream
    /*modelReader->SetStream(stream);
    if (modelReader->Open())
    {
        
    }
    else
    {
        n_error("StreamWorldLoader::SetupModelFromStream(): Failed to open '%s' (-> %s) as Model stream!", 
            this->resource->GetResourceId().Value().AsCharPtr(),
            IO::IoServer::Instance()->ResolveAssignsInString(this->resource->GetResourceId().Value()).AsCharPtr());
        return false;
    }*/
    return true;
}


} // namespace Models
