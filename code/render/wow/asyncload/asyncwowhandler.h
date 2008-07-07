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
	/// ��adt�ļ���׼����
	void OnOpenMapTile(const Ptr<WOW::OpenMapTile>& msg);
	/// ��adt��chunk��Ϣ
	void OnChunkReadStream(const Ptr<WOW::ChunkReadStream>& msg);
	/// �ر�adt�ļ�
	void OnCloseMapTile(const Ptr<WOW::CloseMapTile>& msg);

	/// ���߳��б������������ݣ�����������Ϣȡ������
	Util::Dictionary<Resources::ResourceId, Ptr<IO::StreamReader>> datas;
	//Ptr<IO::Console> console;
	//Ptr<IO::IoServer> ioServer;
};


} // namespace IO
//------------------------------------------------------------------------------
#endif
    
    