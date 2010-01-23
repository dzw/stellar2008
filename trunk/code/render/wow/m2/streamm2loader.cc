//------------------------------------------------------------------------------
//  streammodelloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/streamm2loader.h"
#include "wow/m2/m2streamreader.h"
#include "interface/iointerface.h"
#include "interface/iomsg/readstream.h"
#include "io/memorystream.h"
#include "attr/attributecontainer.h"
#include "coregraphics/mesh.h"

namespace WOW
{
ImplementClass(WOW::StreamM2Loader, 'SM2L', Resources::ResourceLoader);

using namespace Core;
using namespace Messaging;
using namespace IO;
using namespace Util;
using namespace Resources;
using namespace Interface;
using namespace Attr;
using namespace Math;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
StreamM2Loader::StreamM2Loader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StreamM2Loader::~StreamM2Loader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamM2Loader::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamM2Loader::OnLoadRequested()
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
StreamM2Loader::OnLoadCancelled()
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
StreamM2Loader::OnPending()
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
StreamM2Loader::SetupModelFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());

    // first decide what ModelReader to use
    bool isLegacyFile = false;
    String fileExt = this->resource->GetResourceId().Value().GetFileExtension();
	fileExt.ToLower();
	if (fileExt == "m2")
	{
		return this->SetupMeshFromM2(stream);
	}
    return false;
}

//------------------------------------------------------------------------------
/**
	Setup the mesh resource from a m2 file (wow model file).
*/
bool
StreamM2Loader::SetupMeshFromM2(const Ptr<Stream>& stream)
{
	n_assert(stream.isvalid());    
	Ptr<Legacy::M2StreamReader> m2Reader = Legacy::M2StreamReader::Create();
	m2Reader->SetStream(stream);
	if (m2Reader->Open())
	{
		const Ptr<Mesh>& res = this->resource.downcast<Mesh>();
		n_assert(!res->IsLoaded());
		res->SetVertexBuffer(m2Reader->GetVertexBuffer());
		res->SetIndexBuffer(m2Reader->GetIndexBuffer());
		res->SetPrimitiveGroups(m2Reader->GetPrimitiveGroups());
		m2Reader->Close();
		return true;
	}
	return false;
}
} // namespace Models
