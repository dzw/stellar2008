//------------------------------------------------------------------------------
//  adtreadstream.cc
//  cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "interface/iomsg/terraintilestream.h"

namespace Interface
{
ImplementClass(Interface::TerrainTileStream, 'TTSM', Interface::IOMessage);
ImplementMsgId(TerrainTileStream);

//------------------------------------------------------------------------------
/**
*/
TerrainTileStream::~TerrainTileStream()
{
}

//------------------------------------------------------------------------------
/**
*/
void TerrainTileStream::ParseData(const Ptr<Stream>& stream)
{
	if (stream->GetSize() <= 0 || this->tile == NULL)
		return;

	//解析tile
	
}

//------------------------------------------------------------------------------
/**
	解析每个chunk
*/
void TerrainTileStream::ParseChunk()
{

}

} // namespace IO
