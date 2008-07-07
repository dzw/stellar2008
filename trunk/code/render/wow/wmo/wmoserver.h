#pragma once
#ifndef WOW_WMOSERVER_H
#define WOW_WMOSERVER_H
//------------------------------------------------------------------------------
/**
    @class Models::WMOServer
  
    The WMOServer loads and creates shared Model objects.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
//#include "models/model.h"
#include "resources/resourceid.h"/*
#include "models/managedmodel.h"
#include "resources/resourcemapper.h"*/
#include "io/uri.h"
#include "wow/wmo/wmo.h"
#include "wow/wmo/managedWMO.h"

//------------------------------------------------------------------------------
namespace WOW
{
class WMOServer : public Core::RefCounted
{
    DeclareClass(WMOServer);
    DeclareSingleton(WMOServer);
public:
    /// constructor
    WMOServer();
    /// destructor
    virtual ~WMOServer();


    /// open the model server
    void Open();
    /// close the model server
    void Close();
    /// return true if model server is open
    bool IsOpen() const;

	/// 创建wmo实例，直接将相应的wmo相关的model和group全部建立ModelEntity
	void CreateWMOInstance(const Resources::ResourceId& resId, Math::vector& pos, Math::vector& dir, int doodadset);

    /// save a Model to URI using the provided model saver
    //bool SaveModel(const Ptr<Model>& model, const IO::URI& uri, const Core::Rtti& saverClass);
    
	/// check if a managed model exists
	bool HasManagedWMO(const Resources::ResourceId& resId) const;
	/// load a managed Model from URI
	Ptr<ManagedWMO> LoadManagedWMO(const Resources::ResourceId& resId);
	/// lookup an existing model
	const Ptr<ManagedWMO>& LookupManagedWMO(const Resources::ResourceId& resId) const;
	/// discard a managed model
	void DiscardManagedWMO(const Ptr<ManagedWMO>& managedModel) const;
private:
    //friend class WMO;

    bool isOpen;

	//Util::Dictionary<Resources::ResourceId, Ptr<ManagedWMO>> wmos;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
WMOServer::IsOpen() const
{
    return this->isOpen;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
