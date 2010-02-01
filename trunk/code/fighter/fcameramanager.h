#pragma once
#ifndef FIGHTER_CAMERAMANAGER_H
#define FIGHTER_CAMERAMANAGER_H
//------------------------------------------------------------------------------
/**

*/
#include "core/refcounted.h"
#include "core/types.h"
#include "core/singleton.h"
#include "graphics/cameraentity.h"
#include "graphics/view.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Fighter
{
class FCameraManager : public Core::RefCounted
{
	DeclareClass(FCameraManager);
	DeclareSingleton(FCameraManager);
public:
    /// constructor
    FCameraManager();
    /// destructor
    virtual ~FCameraManager();
	/// open the application
    virtual bool Open(const Ptr<Graphics::View>& v);
    /// close the application
    virtual void Close();
	/// checking
	bool IsOpen()const;
	///
	void Update();

	const Ptr<Graphics::CameraEntity>& GetCamera()const;
	Math::vector TransformDirection(const Math::vector& dir);
private:
	bool isOpen;
	Ptr<Graphics::CameraEntity> camera;
};

inline bool 
FCameraManager::IsOpen()const
{
	return isOpen;
}

inline const Ptr<Graphics::CameraEntity>& 
FCameraManager::GetCamera()const
{
	return this->camera;
}

} // namespace Test
//------------------------------------------------------------------------------
#endif
