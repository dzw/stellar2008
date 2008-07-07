//------------------------------------------------------------------------------
//  iointerface.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/asyncload/asyncwowhandler.h"
#include "io/filestream.h"
#include "io/ioserver.h"
#include "io/memorystream.h"
#include "wow/asyncload/parser/terrainparser.h"

#include "timing/timer.h"
#include "foundation/core/sysfunc.h"

namespace WOW
{
ImplementClass(WOW::AsyncWOWHandler, 'TENH', Messaging::Handler);

using namespace IO;
using namespace Messaging;
using namespace Interface;
using namespace Util;

//------------------------------------------------------------------------------
/**    
*/
AsyncWOWHandler::AsyncWOWHandler()
{
}

//------------------------------------------------------------------------------
/**    
*/
AsyncWOWHandler::~AsyncWOWHandler()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
    Opens the Interface message handler which does all the interesting stuff.
    This method already runs in the handler thread.
    The method initializes a minimal thread local Nebula3 runtime, just
    enough to handle the IO messages.
*/
void
AsyncWOWHandler::Open()
{
    Handler::Open();
	/*this->console = Console::Create();
	this->console->Open();
	this->ioServer = IO::IoServer::Create();
	this->ioServer->RegisterUriScheme("file", FileStream::RTTI);*/
}

//------------------------------------------------------------------------------
/**
    Closes the Interface message handler. This will shut down the
    minimal Nebula3 runtime, the method runs in the handler thread and is
    called just before the thread shuts down.
*/
void
AsyncWOWHandler::Close()
{
	//this->ioServer = 0;
	//this->console = 0;
    Handler::Close();
}

//------------------------------------------------------------------------------
/**
    Handles incoming messages. This method runs in the handler thread.
*/
bool
AsyncWOWHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
	if (msg->CheckId(MountMPQArchive::Id))
	{
		this->OnMountMPQArchive(msg.downcast<Interface::MountMPQArchive>());
	}
	else if (msg->CheckId(OpenMapTile::Id))
	{
		this->OnOpenMapTile(msg.downcast<WOW::OpenMapTile>());
	}
	else if (msg->CheckId(ChunkReadStream::Id))
	{
		this->OnChunkReadStream(msg.downcast<WOW::ChunkReadStream>());
	}
	else if (msg->CheckId(CloseMapTile::Id))
	{
		this->OnCloseMapTile(msg.downcast<WOW::CloseMapTile>());
	}
	/*else if (msg->CheckId(MountMPQArchive::Id))
	{
		this->OnMountMPQArchive(msg.downcast<Interface::MountMPQArchive>());
	}*/
    else
    {
        // unknown message
        return false;        
    }
    // fallthrough: message was handled
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
AsyncWOWHandler::OnMountMPQArchive(const Ptr<Interface::MountMPQArchive>& msg)
{
	n_printf("IOInterface: MountMPQArchive %s\n", msg->GetURI().AsString().AsCharPtr());
	IO::IoServer::Instance()->MountMPQArchive(msg->GetURI());
}

//------------------------------------------------------------------------------
/**
*/
void
AsyncWOWHandler::OnOpenMapTile(const Ptr<WOW::OpenMapTile>& msg)
{
	//n_printf("IOInterface: ReadStream %s\n", msg->GetURI().AsString().AsCharPtr());

	// 加载ADT文件
	// create a file stream which reads in the data from disc
	msg->SetResult(false);
	Ptr<Stream> srcStream = IO::IoServer::Instance()->CreateStream(msg->GetURI());
	srcStream->SetAccessMode(Stream::ReadAccess);
	if (srcStream->Open())
	{
		const Ptr<Stream>& dstStream = MemoryStream::Create();//msg->GetStream();
		n_assert(dstStream.isvalid());
		n_assert(dstStream->CanBeMapped());
		dstStream->SetAccessMode(Stream::ReadWriteAccess);
		Stream::Size srcSize = srcStream->GetSize();
		n_assert(srcSize > 0);
		dstStream->SetSize(srcSize);
		if (dstStream->Open())
		{
			void* ptr = dstStream->Map();
			n_assert(0 != ptr);
			srcStream->Read(ptr, srcSize);
			dstStream->Unmap();
			dstStream->Close();

			// 解析ADT
			Ptr<TerrainParse> tp = TerrainParse::Create();
			tp->SetStream(dstStream);
			tp->ParseData();
			datas.Add(Resources::ResourceId(msg->GetURI().AsString()), tp.upcast<IO::StreamReader>());

			msg->SetResult(true);
		}
		srcStream->Close();
	}
}

//------------------------------------------------------------------------------
/**
	解析chunk,并返回一个chunk
	
	2_3_001.chk
*/
void
AsyncWOWHandler::OnChunkReadStream(const Ptr<WOW::ChunkReadStream>& msg)
{
	Timing::Timer tt;
	tt.Reset();
	tt.Start();

	String name = msg->GetURI().AsString();
	String chkId;
	
	chkId = name.ExtractRange(name.Length()-7, 3);
	name.TerminateAtIndex(name.Length()-8);
	name.Append(".adt");
	
	IndexT index = datas.FindIndex(name);
	if (InvalidIndex == index)
		return;

	
	const Ptr<TerrainParse> tp = datas.ValueAtIndex(index).downcast<TerrainParse>();
	
	tp->ParseChunk(atoi(chkId.AsCharPtr()), msg->chunk);
	msg->SetResult(true);

	tt.Stop();
	float t = (float)tt.GetTime();
	String output;
	output = "OnChunkReadStream: ";
	output.AppendFloat(t);
	output.Append("\n");
	Core::SysFunc::DebugOut(output.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void
AsyncWOWHandler::OnCloseMapTile(const Ptr<WOW::CloseMapTile>& msg)
{
	IndexT index = datas.FindIndex(msg->GetURI().AsString());
	if (InvalidIndex == index)
		return;

	datas.EraseAtIndex(index);
}

} // namespace IO