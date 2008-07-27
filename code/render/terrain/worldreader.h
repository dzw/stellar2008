#pragma once
#ifndef TERRAIN_WORLDREADER_H
#define TERRAIN_WORLDREADER_H
//------------------------------------------------------------------------------
/**
    cTuo
*/
#include "io\streamreader.h"
#include "terrain\world.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class WorldReader : public IO::StreamReader
{
	DeclareClass(WorldReader);
public:
	/// destructor
	virtual ~WorldReader();

	/// begin reading from the stream
	virtual bool Open();
	/// end reading from the stream
	virtual void Close();

	void SetResource(const Ptr<Resources::Resource> res);
	/// parse resource
	bool FillModel();
protected:
	Ptr<World> world;
};

inline void 
WorldReader::SetResource(const Ptr<Resources::Resource> res)
{
	this->world = res.downcast<World>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    