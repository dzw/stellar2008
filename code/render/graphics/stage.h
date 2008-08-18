#pragma once
#ifndef GRAPHICS_STAGE_H
#define GRAPHICS_STAGE_H
//------------------------------------------------------------------------------
/**
    @class Graphics::Stage
    
    A graphics stage groups graphics entities (models, cameras and lights)
    together for rendering. The main job of a Stage is to speed up
    visibility queries between the attached graphics entities. Different
    visibility query strategies are implemented by Stage subclasses. 
    Nebula3 comes with a set of generic Stage subclasses for different
    purposes, but applications are free to derive their own subclasses
    which implement visibility query mechanisms tailored to the application.
    
    Visibility queries exist in the following variations:
    
    - Camera->Light: this finds all light entities visible from a
      given camera
    - Camera->Model: this finds all model entities visible from a
      given camera
    - Light->Model: this finds all model entities which are lit
      by a given light source
    
    Those visibility queries establish so-called visibility links
    between graphics entities which are then used by the lower
    level rendering subsystems to speed up rendering.

    To render the content of a stage, at least one View object is 
    needed. A View object binds renders a stage through a camera entity
    into a render target. Any number of View objects can exist in
    parallel, and may be bound to any Stage. Furthermore, dependencies
    between View objects may be defined (so that a View object will
    first ask the View objects it depends on to render themselves).

    (C) 2007 Radon Labs GmbH

	stage目标就是通过各种裁剪策略找出可见对象，提供给其它低层子系统用于渲染。
	所以其本身应该是个管理裁剪策略的类，如可以管理四叉树、八叉树、portal等。
	但是同一次渲染中只能存在一个Stage，因为View只关联一个Stage,所以一次Stage
	的裁剪过程应该包含了所有需要渲染的对象。继承Stage的子类可以实现一些特殊的
	裁剪策略。
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "graphics/graphicsentity.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class StageBuilder;
class Cell;
class CameraEntity;

class Stage : public Core::RefCounted
{
    DeclareClass(Stage);
public:
    /// constructor
    Stage();
    /// destructor
    virtual ~Stage();

    /// return true if currently attached to graphics server
    bool IsAttachedToServer() const;
    /// get human readable name of the stage
    const Util::StringAtom& GetName() const;
    /// get stage builder object
    const Ptr<StageBuilder>& GetStageBuilder() const;

    /// set the root cell of the stage
    void SetRootCell(const Ptr<Cell>& rootCell);
    /// get the root cell of the stage
    const Ptr<Cell>& GetRootCell() const;
	/// 增加可见cell
	void AddVisibleCell(const Ptr<Cell>& cell);
	/// 获得可见列表
	const Util::Array<Ptr<Cell>>& GetVisibleCell()const;
    
    /// attach an entity to the stage
    virtual void AttachEntity(const Ptr<GraphicsEntity>& graphicsEntity);
    /// remove an entity from the stage
    virtual void RemoveEntity(const Ptr<GraphicsEntity>& entity);
    /// get an array of all entities attached to the stage
    const Util::Array<Ptr<GraphicsEntity> >& GetEntities() const;
    /// get entities by type
    const Util::Array<Ptr<GraphicsEntity> >& GetEntitiesByType(GraphicsEntity::Type type) const;

    /// update (animate) entities in the cell
    virtual void UpdateEntities(Timing::Time curTime, IndexT frameCount);
    /// update camera links for a given camera
    virtual void UpdateCameraLinks(const Ptr<CameraEntity>& cameraEntity);
    /// update light links for all visible lights (after updating camera links!)
    virtual void UpdateLightLinks();

protected:
    friend class GraphicsServer;

    /// set a human readable name on the stage
    void SetName(const Util::StringAtom& name);
    /// set stage builder object
    void SetStageBuilder(const Ptr<StageBuilder>& stageBuilder);
    /// called when the stage is attached to graphics server
    virtual void OnAttachToServer();
    /// called when the stage is detached from graphics server
    virtual void OnRemoveFromServer();

    bool isAttachedToServer;
    IndexT updateEntitiesFrameCount;
    Util::StringAtom name;
    Ptr<Cell> rootCell;
    Ptr<StageBuilder> stageBuilder;
	Util::Array<Ptr<Cell>>			  visibleCells;		///注意！！！在这个类头文件中Cell一定要在GraphicsEntity前面，否则会因为GraphicsEntity中的重定义引起错误!!!而且不能在这里实现inline相关函数！！
    Util::Array<Ptr<GraphicsEntity> > entities;
    Util::Array<Ptr<GraphicsEntity> > entitiesByType[GraphicsEntity::NumTypes];
};

//------------------------------------------------------------------------------
/**
*/
inline bool
Stage::IsAttachedToServer() const
{
    return this->isAttachedToServer;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Stage::SetName(const Util::StringAtom& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
Stage::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Graphics::GraphicsEntity> >&
Stage::GetEntities() const
{
    return this->entities;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Graphics::GraphicsEntity> >&
Stage::GetEntitiesByType(Graphics::GraphicsEntity::Type entityType) const
{
    n_assert((entityType >= 0) && (entityType < Graphics::GraphicsEntity::NumTypes));
    return this->entitiesByType[entityType];
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
