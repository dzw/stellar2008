#pragma once
#ifndef APP_GAMEAPPLICATION_H
#define APP_GAMEAPPLICATION_H
//------------------------------------------------------------------------------
/**
    @class App::GameApplication

    Nebula3's default game application. It creates and triggers the GameServer.
    For game features it creates the core and graphicsfeature which is used in every
    gamestate (such as level gamestates or only gui gamestates).

    (C) 2007 Radon Labs GmbH
*/
#include "app/application.h"
#include "appgame/statehandler.h"
#include "game/gameserver.h"
#include "core/singleton.h"
#include "util/dictionary.h"
#include "util/stringatom.h"

using namespace Util;

namespace CoreFeature
{
    class CoreFeatureUnit;
}
namespace GraphicsFeature
{
    class GraphicsFeatureUnit;
}

//------------------------------------------------------------------------------
namespace App
{
class GameApplication : public Application
{
    DeclareSingleton(GameApplication);   

public:
    /// constructor
    GameApplication();
    /// destructor
    virtual ~GameApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
    /// run the application
    virtual void Run();

    /// add an application state handler
    void AddStateHandler(const Ptr<StateHandler>& state);
    /// find a state handler by name
    const Ptr<StateHandler>& FindStateHandlerByName(const Util::String& stateName) const;
    /// return pointer to current state handler
    const Ptr<StateHandler>& GetCurrentStateHandler() const;
    /// return state handler of current state
    const Util::String& GetCurrentState() const;
    /// get number of application states
    int GetNumStates() const;
    /// get state handler at index
    const Ptr<StateHandler>& GetStateHandlerAt(int index) const;
    /// request a new state which will be applied at the end of the frame
    void RequestState(const Util::String& stateName);

protected:
    /// setup application state handlers
    virtual void SetupStateHandlers();
    /// cleanup application state handlers
    virtual void CleanupStateHandlers(); 
    /// setup game features
    virtual void SetupGameFeatures();
    /// cleanup game features
    virtual void CleanupGameFeatures(); 
    /// perform a state transition
    virtual void DoStateTransition();
    /// set an application state
    void SetState(const String& s);

    /// state handlers
    String requestedState;
    String curState;
    String nextState;
    Util::Dictionary<Util::StringAtom, Ptr<StateHandler> > stateHandlers;

    /// game server
    Ptr<Game::GameServer> gameServer;

    /// default game features
    Ptr<CoreFeature::CoreFeatureUnit> coreFeature;
    Ptr<GraphicsFeature::GraphicsFeatureUnit> graphicsFeature;
};
//------------------------------------------------------------------------------
/**
*/
inline int
GameApplication::GetNumStates() const
{
    return this->stateHandlers.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<StateHandler>&
GameApplication::GetStateHandlerAt(int index) const
{
    return this->stateHandlers.ValueAtIndex(index);
}

//------------------------------------------------------------------------------
/**
    Returns the currently active application state. Can be 0 if no valid 
    state is set.
*/
inline const String&
GameApplication::GetCurrentState() const
{
    return this->curState;
}

} // namespace App
//------------------------------------------------------------------------------
#endif
