//------------------------------------------------------------------------------
//  adtreadstream.cc
//  cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iomsg/worldstream.h"

namespace Interface
{
ImplementClass(Interface::WorldStream, 'WDSM', Interface::IOMessage);
ImplementMsgId(WorldStream);

WorldStream::~WorldStream()
{
}

void WorldStream::ParseData(const Ptr<Stream>& stream)
{
	if (stream->GetSize() <= 0 || this->world == NULL)
		return;

	
}

} // namespace IO
