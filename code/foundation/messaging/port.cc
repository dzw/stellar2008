//------------------------------------------------------------------------------
//  port.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "messaging/port.h"

namespace Messaging
{
ImplementClass(Messaging::Port, 'PORT', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
Port::SetupAcceptedMessages()
{
    // overwrite in subclass
}

//------------------------------------------------------------------------------
/**
    Attach a message handler to the port. 
*/
void
Port::AttachHandler(const Ptr<Handler>& h)
{
    n_assert(h.isvalid());
    n_assert(InvalidIndex != this->handlers.FindIndex(h));
    this->handlers.Append(h);
}

//------------------------------------------------------------------------------
/**
    Remove a message handler from the port. 
*/
void
Port::RemoveHandler(const Ptr<Handler>& h)
{
    n_assert(h.isvalid());
    IndexT index = this->handlers.FindIndex(h);
    n_assert(InvalidIndex != index);
    this->handlers.EraseIndex(index);
}

//------------------------------------------------------------------------------
/**
    Send a message to the port. This will immediately call the HandleMessage()
    method of all attached handlers. If the message has been handled by at least
    one of the handlers, the Handled() flag of the message will be set to true.
*/
void
Port::Send(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
    Array<Ptr<Handler> >::Iterator iter;
    for (iter = this->handlers.Begin(); iter < this->handlers.End(); iter++)
    {
        if ((*iter)->HandleMessage(msg))
        {
            // message had been handled by this handler
            /// @todo should we stop here, once the message had been handled...?
            msg->SetHandled(true);
        }
    }
}
//------------------------------------------------------------------------------
/**
    Handle a specific message. Overwrite this method in a subclass.
    It is guaranteed that this method will only be called for messages
    which are accepted by AcceptMessage().
*/
void
Port::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    // empty
}

} // namespace Messaging
