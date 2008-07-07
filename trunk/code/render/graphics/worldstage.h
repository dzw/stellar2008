#pragma once
#ifndef GRAPHICS_WORLDSTAGE_H
#define GRAPHICS_WORLDSTAGE_H
//------------------------------------------------------------------------------
/**
	��̬entity����Ҫ���£����������ʵ�־�̬entity,��ԭ����Stage�ദ��̬entity
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

protected:

    Util::Array<Ptr<GraphicsEntity> > staticEntities;
};

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
