#pragma once
#ifndef WOW_CHUNKREADSTREAM_H
#define WOW_CHUNKREADSTREAM_H
//------------------------------------------------------------------------------
/**
    @class Interface::ReadStream
    
    IOInterface message to asynchronously read a stream. Note that the
    caller must provide a mappable stream which consumes the read
    content.
        
    (C) 2006 Radon Labs GmbH
*/
#include "interface/iomsg/iomessage.h"
#include "render/wow/world/adt/maptile.h"

//------------------------------------------------------------------------------
namespace WOW
{
class ChunkReadStream : public Interface::IOMessage
{
    DeclareClass(ChunkReadStream);
    DeclareMsgId;

	///
	~ChunkReadStream();

public:
	ChunkData chunk;
};

} // namespace Interface
//------------------------------------------------------------------------------
#endif
