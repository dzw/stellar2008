#pragma once
#ifndef INTERFACE_WORLDSTREAM
#define INTERFACE_WORLDSTREAM
//------------------------------------------------------------------------------
/**
    @class Interface::WorldStream
    
    º”‘ÿ≤¢Ω‚Œˆ
        
    (C) 2006 Radon Labs GmbH
*/
#include "interface/iomsg/iomessage.h"
#include "render/terrain/world.h"

//------------------------------------------------------------------------------
namespace Interface
{
class WorldStream : public IOMessage
{
    DeclareClass(WorldStream);
    DeclareMsgId;

	///
	~WorldStream();
	///
	void ParseData(const Ptr<Stream>& stream);

public:

	Ptr<Terrain::World> world;
};

} // namespace Interface
//------------------------------------------------------------------------------
#endif
