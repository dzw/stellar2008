#pragma once
#ifndef GRAPHICS_WORLDSTAGE_H
#define GRAPHICS_WORLDSTAGE_H
//------------------------------------------------------------------------------
/**
	静态entity不需要更新，所在这个类实现静态entity,而原来的Stage类处理动态entity
*/
#include "graphics/stage.h"
#include "graphics/graphicsentity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class WorldStage : public Stage
{
    DeclareClass(WorldStage);
public:
    /// constructor
    WorldStage();
    /// destructor
    virtual ~WorldStage();
    
    /// attach an entity to the stage
    virtual void AttachEntity(const Ptr<GraphicsEntity>& graphicsEntity);
    /// remove an entity from the stage
    virtual void RemoveEntity(const Ptr<GraphicsEntity>& entity);
	virtual void UpdateCameraLinks(const Ptr<CameraEntity>& cameraEntity);
	void AddVisibleCell(int id);
protected:

    Util::Array<Ptr<GraphicsEntity> > staticEntities;
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
