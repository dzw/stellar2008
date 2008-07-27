//------------------------------------------------------------------------------
//  streammodelloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/streamterrainloader.h"
#include "interface/iointerface.h"
#include "interface/iomsg/readstream.h"
#include "Terrain/iomsg/terraintilestream.h"
#include "io/memorystream.h"
#include "attr/attributecontainer.h"
#include "math/vector3.h"
#include "Terrain/terraintile.h"
#include "terrain/worldreader.h"

namespace Terrain
{
ImplementClass(Terrain::StreamTerrainLoader, 'SWDL', Resources::ResourceLoader);

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
		if (this->resource->GetResourceId().Value().GetFileExtension() == "adt")
		{
			this->readStreamMsg = (Interface::IOMessage*)TerrainTileStream::Create();
            this->readStreamMsg.downcast<TerrainTileStream>()->tile = this->resource.downcast<TerrainTile>();
			this->readStreamMsg->SetURI(this->resource->GetResourceId().Value());
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

        // go into Pending state
        this->SetState(Resource::Pending);
        return true;
    }
    else
    {
		// 其它模型不在这里加载，代替模型不可用
		
        // perform synchronous load
        this->stream = IoServer::Instance()->CreateStream(this->resource->GetResourceId().Value());
	    if (this->SetupModelFromStream())
        {
            this->SetState(Resource::Loaded);
            return true;
        }
        //// fallthrough: synchronous loading failed
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
StreamTerrainLoader::SetupModelFromStream()
{
    // first decide what ModelReader to use
    bool isLegacyFile = false;
    String fileExt = this->resource->GetResourceId().Value().GetFileExtension();
    //Ptr<ModelReader> modelReader;
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
		bool isOk = false;
		Ptr<WorldReader> reader = WorldReader::Create();
		reader->SetStream(this->stream);
		reader->SetResource(this->resource);
		if (reader->Open())
		{
			isOk = reader->FillModel();
			reader->Close();
		}
		reader = 0;
		this->stream = 0;
		if (isOk)
			return true;
		return false;
	}
	else if (fileExt == "wdl")
	{
//		modelReader = (ModelReader*)WDLReader::Create();
//		modelReader->SetModelResId(this->resource->GetResourceId());
		//modelReader.downcast<WDLReader>()->SetWDL(this->resource.downcast<WDL>());
	}
	else if (fileExt == "adt")
	{
		// 已在加载线程中解析了数据，在这里加载wmos和models。
		
		return true;
	}
    else
    {
        n_error("StreamTerrainLoader: unrecognized file extension '%s' in '%s'!",
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
        n_error("StreamTerrainLoader::SetupModelFromStream(): Failed to open '%s' (-> %s) as Model stream!", 
            this->resource->GetResourceId().Value().AsCharPtr(),
            IO::IoServer::Instance()->ResolveAssignsInString(this->resource->GetResourceId().Value()).AsCharPtr());
        return false;
    }*/
    return true;
}


} // namespace Models
