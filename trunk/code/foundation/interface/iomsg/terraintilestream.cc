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

	//����tile
	
}

//------------------------------------------------------------------------------
/**
	����ÿ��chunk
*/
void TerrainTileStream::ParseChunk()
{

}

} // namespace IO
