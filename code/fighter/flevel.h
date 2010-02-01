#pragma once
#ifndef FIGHTER_FLEVEL_H
#define FIGHTER_FLEVEL_H
//------------------------------------------------------------------------------
/**

*/
#include "core/refcounted.h"
#include "core/types.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FLevel : public Core::RefCounted
{
	DeclareClass(FLevel);
public:
    /// constructor
    FLevel();
    /// destructor
    virtual ~FLevel();
	
	void Init(const Ptr<Graphics::Stage>& s);
	void Clear();
private:
	Ptr<Graphics::Stage> stage;
	/// µØÐÎ
	Ptr<Graphics::ModelEntity> model;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
