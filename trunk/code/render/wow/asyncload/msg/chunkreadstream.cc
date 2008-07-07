//------------------------------------------------------------------------------
//  adtreadstream.cc
//  cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/asyncload/msg/chunkreadstream.h"

namespace WOW
{
ImplementClass(WOW::ChunkReadStream, 'CRSM', Interface::IOMessage);
ImplementMsgId(ChunkReadStream);

ChunkReadStream::~ChunkReadStream()
{
	if (NULL != chunk.dataBuf)
	{
		n_delete_array(chunk.dataBuf);
		chunk.dataBuf = NULL;
	}
	if (NULL != chunk.blendbuf)
	{
		n_delete_array(chunk.blendbuf);
		chunk.blendbuf = NULL;
	}
}

} // namespace IO
