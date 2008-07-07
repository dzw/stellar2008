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

	// 加载线程不能存在两个，否则容易产生问题，因业不知道哪个格式在哪个线程加载，会很乱
	//Ptr<WOW::AsyncWOWHandler> wowMsgHandler = WOW::AsyncWOWHandler::Create();
	//this->AttachHandler(wowMsgHandler.upcast<Messaging::Handler>());
}

} // namespace IO