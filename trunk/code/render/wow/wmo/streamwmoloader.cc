//------------------------------------------------------------------------------
//  streammodelloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/wmo/streamwmoloader.h"
#include "interface/iointerface.h"
#include "interface/iomsg/readstream.h"
#include "io/memorystream.h"
#include "attr/attributecontainer.h"
#include "wow/wmo/wmoreader.h"
//#include "models/nodes/shapenode.h"
#include "wow/wmo/wmoserver.h"
#include "math/vector3.h"

namespace WOW
{
ImplementClass(WOW::StreamWMOLoader, 'SWMO', Resources::ResourceLoader);

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
StreamWMOLoader::StreamWMOLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StreamWMOLoader::~StreamWMOLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamWMOLoader::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamWMOLoader::OnLoadRequested()
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
        if (this->SetupModelFromStream(stream))
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
StreamWMOLoader::OnLoadCancelled()
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
StreamWMOLoader::OnPending()
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
            if (this->SetupModelFromStream(this->readStreamMsg->GetStream()))
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
StreamWMOLoader::SetupModelFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());

    // first decide what ModelReader to use
    bool isLegacyFile = false;
    String fileExt = this->resource->GetResourceId().Value().GetFileExtension();
    Ptr<ModelReader> modelReader;
	fileExt.ToLower();
	if (fileExt == "wmo")
	{
		modelReader = (ModelReader*)WMOReader::Create();
		modelReader->SetModelResId(this->resource->GetResourceId());
		modelReader.downcast<WMOReader>()->SetWMO(this->resource.downcast<WMO>());
	}
	else if (fileExt == "n2")
	{
		// 替代模型暂时用了n2模型，所以过滤掉
		return true;
	}
    else
    {
        n_error("StreamWMOLoader: unrecognized file extension '%s' in '%s'!",
            fileExt.AsCharPtr(), this->resource->GetResourceId().Value().AsCharPtr());
        return false;
    }

    // parse the stream
    modelReader->SetStream(stream);
    if (modelReader->Open())
    {
        if (modelReader->FillModel())
        {
			// 解析wmo
			modelReader.downcast<WMOReader>()->ReadWMO();

			// 加载group
			Ptr<WMO> wmo = this->resource.downcast<WMO>();
			//const Array<String>& groups = wmo->GetGroupsName();

			vector pos(0, 0, 0);
			vector dir(0, 0, 0);

			wmo->CreateInstance(pos, dir, 0);
			//for (SizeT i = 0; i < groups.Size(); i++)
			{
				//WMOServer::Instance()->CreateWMOInstance(this->resource->GetResourceId(), pos, dir, 0);
			}
        }
        else
        {
            n_error("StreamWMOLoader::SetupModelFromStream(): ModelReader::SetToFirstModel() failed for '%s'!",
                this->resource->GetResourceId().Value().AsCharPtr());
        }
    }
    else
    {
        n_error("StreamWMOLoader::SetupModelFromStream(): Failed to open '%s' (-> %s) as Model stream!", 
            this->resource->GetResourceId().Value().AsCharPtr(),
            IO::IoServer::Instance()->ResolveAssignsInString(this->resource->GetResourceId().Value()).AsCharPtr());
        return false;
    }
    return true;
}


} // namespace Models
