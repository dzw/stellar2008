#ifndef PROPERTIES_GRAPHICSPROPERTY_H
#define PROPERTIES_GRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    This is the standard graphics property which adds visibility to a game 
    entity. 

    NOTE: There are cases where the graphics property may depend on a 
    physics property (for complex physics entities which require several 
    graphics entities to render themselves). Thus it is recommended that 
    physics properties are attached before graphics properties.
    
    (C) 2007 Radon Labs GmbH
*/
#include "game/property.h"
#include "game/entity.h"
#include "graphicsattr/graphicsattributes.h"

//------------------------------------------------------------------------------
namespace Graphics
{
    class ModelEntity;
};

namespace GraphicsFeature
{
class GraphicsProperty : public Game::Property
{
	DeclareClass(GraphicsProperty);
public:
    /// constructor
    GraphicsProperty();
    /// destructor
    virtual ~GraphicsProperty();

    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);   
    /// override to provide a self managed graphics resource (default is Attr::Graphics)
    virtual const Util::String& GetGraphicsResource() const;

protected:
    /// setup graphics entities
    virtual void SetupGraphicsEntities();
    /// update the graphics entity's transform
    virtual void UpdateTransform(const Math::matrix44& m, bool setDirectly=false);
    // shows or hides all attached graphics entities
    void SetVisible(bool visible);

    Util::Array<Ptr<Graphics::ModelEntity> > graphicsEntities;
};
RegisterClass(GraphicsProperty);
//------------------------------------------------------------------------------
/**    
    Get the default graphics resource, which is Attr::Graphics.
    subclasses may override this to provide a self managed resource.
*/
inline const Util::String&
GraphicsProperty::GetGraphicsResource() const
{
    return (GetEntity()->GetString(Attr::Graphics));    
}
}; // namespace Property
//------------------------------------------------------------------------------
#endif
