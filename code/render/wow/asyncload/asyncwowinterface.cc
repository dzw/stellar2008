//------------------------------------------------------------------------------
//  asyncgraphicsinterface.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/asyncload/asyncwowinterface.h"
#include "wow/asyncload/asyncwowhandler.h"
#include "threading/thread.h"

namespace WOW
{
ImplementClass(WOW::AsyncWOWInterface, 'AWIF', Messaging::AsyncPort);
ImplementInterfaceSingleton(WOW::AsyncWOWInterface);

//------------------------------------------------------------------------------
/**
*/
AsyncWOWInterface::AsyncWOWInterface()
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AsyncWOWInterface::~AsyncWOWInterface()
{
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
AsyncWOWInterface::Open()
{
    // we need to run continously, not wait for messages
    this->SetWaitForMessages(false);
    this->SetName("AsyncWOWInterface");
    AsyncPort::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
AsyncWOWInterface::OnCreateHandlers()
{
    Ptr<AsyncWOWHandler> msgHandler = AsyncWOWHandler::Create();
    this->AttachHandler(msgHandler.upcast<Messaging::Handler>());
}

} // namespace AsyncGraphics
