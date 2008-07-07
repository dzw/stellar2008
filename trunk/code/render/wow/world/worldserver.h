#pragma once
#ifndef WOW_WORLDSERVER_H
#define WOW_WORLDSERVER_H
//------------------------------------------------------------------------------
/**
    @class Models::WorldServer
  
    The WorldServer loads and creates shared Model objects.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
//#include "models/model.h"
#include "resources/resourceid.h"/*
#include "models/managedmodel.h"
#include "resources/resourcemapper.h"*/
#include "io/uri.h"
#include "wow/world/world.h"
#include "wow/world/managedWorld.h"

//------------------------------------------------------------------------------
namespace WOW
{
class WorldServer : public Core::RefCounted
{
    DeclareClass(WorldServer);
    DeclareSingleton(WorldServer);
public:
    /// constructor
    WorldServer();
    /// destructor
    virtual ~WorldServer();


    /// open the model server
    void Open();
    /// close the model server
    void Close();
    /// return true if model server is open
    bool IsOpen() const;

	/// 创建wmo实例，直接将相应的wmo相关的model和group全部建立ModelEntity
	//void CreateWMOInstance(const Resources::ResourceId& resId, Math::vector3& pos, Math::vector3& dir, int doodadset);

    /// save a Model to URI using the provided model saver
    //bool SaveModel(const Ptr<Model>& model, const IO::URI& uri, const Core::Rtti& saverClass);
    
	/// check if a managed model exists
	bool HasManagedWorld(const Resources::ResourceId& resId) const;
	/// load a managed Model from URI
	Ptr<ManagedWorld> LoadManagedWorld(const Resources::ResourceId& resId);
	/// lookup an existing model
	const Ptr<ManagedWorld>& LookupManagedWorld(const Resources::ResourceId& resId) const;
	/// discard a managed model
	void DiscardManagedWorld(const Ptr<ManagedWorld>& managedModel) const;

private:

    bool isOpen;

	//Util::Dictionary<Resources::ResourceId, Ptr<WMO>> wmos;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
WorldServer::IsOpen() const
{
    return this->isOpen;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
