#pragma once
#ifndef KOK_THINGINSTANCE_H
#define KOK_THINGINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::ThingInstance
    
    (C) 2008 cTuo
*/
#include "models/modelinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ThingInstance : public Models::ModelInstance
{
    DeclareClass(ThingInstance);
public:
    /// constructor
    ThingInstance();
    /// destructor
    virtual ~ThingInstance();

    /// perform per-frame update (after setting transform, visibility, time, etc)
    virtual void Update();
	void SetVisiableRepeat(int id);
	void SetTexture(int nodeId, int texId);

	const Util::String& GetFilePath()const;
protected:
	virtual void OnAttachToModel(const Ptr<Models::Model>& model);
	virtual void NotifyVisible(IndexT frameIndex);

	Util::String filePath;
};

inline const Util::String& 
ThingInstance::GetFilePath()const
{
	return filePath;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
