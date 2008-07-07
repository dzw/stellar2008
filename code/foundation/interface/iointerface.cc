//------------------------------------------------------------------------------
//  iointerface.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iointerface.h"
#include "interface/iointerfacehandler.h"
#include "render/wow/asyncload/asyncwowhandler.h"

namespace Interface
{
ImplementClass(Interface::IOInterface, 'IIOF', Messaging::AsyncPort);
ImplementInterfaceSingleton(Interface::IOInterface);

using namespace IO;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
IOInterface::IOInterface()
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
IOInterface::~IOInterface()
{
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
IOInterface::Open()
{
    this->SetName("IOInterface");
    AsyncPort::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
IOInterface::OnCreateHandlers()
{
    Ptr<IOInterfaceHandler> msgHandler = IOInterfaceHandler::Create();
    this->AttachHandler(msgHandler.upcast<Messaging::Handler>());

	// �����̲߳��ܴ����������������ײ������⣬��ҵ��֪���ĸ���ʽ���ĸ��̼߳��أ������
	//Ptr<WOW::AsyncWOWHandler> wowMsgHandler = WOW::AsyncWOWHandler::Create();
	//this->AttachHandler(wowMsgHandler.upcast<Messaging::Handler>());
}

} // namespace IO