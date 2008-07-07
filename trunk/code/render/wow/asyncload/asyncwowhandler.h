#pragma once
#ifndef WOW_ASYNCWOWHANDLER_H
#define WOW_ASYNCWOWHANDLER_H
//------------------------------------------------------------------------------
/**
    @class IO::IOInterfaceHandlerBase
    
    Handler class for io interfaces.
    
    (C) 2006 Radon Labs GmbH
*/
#include "messaging/handler.h"
#include "render/wow/asyncload/msg/chunkreadstream.h"
#include "render/wow/asyncload/msg/closemaptile.h"
#include "render/wow/asyncload/msg/openmaptile.h"
#include "render/resources/resourceid.h"
#include "util/dictionary.h"
#include "interface/iomsg/mountmpqarchive.h"
//#include "io/console.h"
//#include "io/ioserver.h"

//------------------------------------------------------------------------------
namespace WOW
{
    
class AsyncWOWHandler : public Messaging::Handler
{
    DeclareClass(AsyncWOWHandler);
public:
    /// constructor
    AsyncWOWHandler();
    /// destructor
    virtual ~AsyncWOWHandler();
    
    /// open the handler
    virtual void Open();
    /// close the handler
    virtual void Close();
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
	/// handle MountMPQArchive message
	void OnMountMPQArchive(const Ptr<Interface::MountMPQArchive>& msg);
	/// 打开adt文件，准备读
	void OnOpenMapTile(const Ptr<WOW::OpenMapTile>& msg);
	/// 从adt读chunk信息
	void OnChunkReadStream(const Ptr<WOW::ChunkReadStream>& msg);
	/// 关闭adt文件
	void OnCloseMapTile(const Ptr<WOW::CloseMapTile>& msg);

	/// 在线程中保存解析后的数据，方便其它消息取得数据
	Util::Dictionary<Resources::ResourceId, Ptr<IO::StreamReader>> datas;
	//Ptr<IO::Console> console;
	//Ptr<IO::IoServer> ioServer;
};


} // namespace IO
//------------------------------------------------------------------------------
#endif
    
    