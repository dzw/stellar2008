#ifndef MESSAGING_MOVEFOLLOW_H
#define MESSAGING_MOVEFOLLOW_H
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::MoveFollow

    A MoveFollow message. The expected behaviour is that the entity which
    receives this Message follows the target entity (defined by the
    target entity's unique id) until told otherwise.

    (C) 2005 Radon Labs GmbH
*/
#include "messaging/message.h"
#include "game/entity.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class MoveFollow : public Messaging::Message
{
    DeclareClass(MoveFollow);
    DeclareMsgId;

public:
    /// constructor
    MoveFollow();
    /// set the target entity's unique id
    void SetTargetEntityId(Game::Entity::EntityId id);
    /// get the target entity's unique id
    Game::Entity::EntityId GetTargetEntityId() const;
    /// set the distance to keep to the target entity
    void SetDistance(float d);
    /// get the distance to keep to the target entity
    float GetDistance() const;

private:
    Game::Entity::EntityId entityId;
    float distance;
};

//------------------------------------------------------------------------------
/**
*/
inline
MoveFollow::MoveFollow() :
    entityId(0),
    distance(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MoveFollow::SetTargetEntityId(Game::Entity::EntityId id)
{
    this->entityId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
Game::Entity::EntityId
MoveFollow::GetTargetEntityId() const
{
    return this->entityId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MoveFollow::SetDistance(float d)
{
    this->distance = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
MoveFollow::GetDistance() const
{
    return this->distance;
}

}; // namespace BaseGameFeature
//------------------------------------------------------------------------------
#endif