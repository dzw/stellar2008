//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/fhero.h"

namespace Fighter
{
ImplementClass(Fighter::FHero, 'HERO', Core::RefCounted);

using namespace Graphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
FHero::FHero()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FHero::~FHero()
{

}

void 
FHero::Init()
{

}

void 
FHero::Clear()
{
	this->model = 0;
	this->stage = 0;
}

} // namespace Tools
