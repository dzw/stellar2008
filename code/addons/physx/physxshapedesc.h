#ifndef PHYSX_PHYSXSHAPEDESC_H
#define PHYSX_PHYSXSHAPEDESC_H
//------------------------------------------------------------------------------
/**
	@class PhysX::PhysXShapeDesc

    (C) 2010
*/
#include "core/refcounted.h"
#include <NxPhysics.h>

namespace PhysX
{

class PhysXShapeDesc : public Core::RefCounted
{
    DeclareClass(PhysXShapeDesc);

public:
    PhysXShapeDesc();
    ~PhysXShapeDesc();
    
    NxShapeType GetType() const;
    
    // Get and set the shape that was created from this descriptor.
    NxShape* GetShape() const;
    void SetShape(NxShape* pkShape);
    
    NxMat34 GetLocalPose() const;
    void SetLocalPose(const NxMat34& kPose);
   
    virtual void ToShapeDesc(NxShapeDesc& kShapeDesc) const;

    virtual void ToPlaneDesc(NxPlaneShapeDesc& kPlaneDesc) const;
    virtual void FromPlaneDesc(const NxPlaneShapeDesc& kPlaneDesc);
    
    virtual void ToSphereDesc(NxSphereShapeDesc& kSphereDesc) const;
    virtual void FromSphereDesc(const NxSphereShapeDesc& kSphereDesc);

    virtual void ToBoxDesc(NxBoxShapeDesc& kBoxDesc) const;
    virtual void FromBoxDesc(const NxBoxShapeDesc& kBoxDesc);

    virtual void ToCapsuleDesc(NxCapsuleShapeDesc& kCapsuleDesc) const;
    virtual void FromCapsuleDesc(const NxCapsuleShapeDesc& kCapsuleDesc);

    virtual void ToConvexDesc(NxConvexShapeDesc& kConvexDesc,
        const bool bKeepMesh, const bool bCheckPlatform = true);

    virtual void ToTriMeshDesc(NxTriangleMeshShapeDesc& kTriMeshDesc,
        const bool bKeepMesh, const bool bCheckPlatform = true);

    void SetMeshDesc(NiPhysXMeshDesc* pkMeshDesc);
    NiPhysXMeshDescPtr GetMeshDesc();
    
    // *** begin Emergent internal use only ***

    virtual void FromShapeDesc(const NxShapeDesc& kShapeDesc);
    
    virtual bool StreamCanSkip();
    
    // *** end Emergent internal use only ***

protected:

    // All the stuff in common to all shapes.
    NxShapeType m_eType;
    NxMat34 m_kLocalPose;
    NxU32 m_uiShapeFlags;
    NxCollisionGroup m_uiShapeGroup;
    NxMaterialIndex m_uiMaterialIndex;
    NxReal m_fDensity;
    NxReal m_fMass;
    NxReal m_fSkinWidth;
    NiFixedString m_kShapeName;
    NxU32 m_auiCollisionBits[4];

    // Used to encode the individual shapes.
    NxReal m_fVal1;
    NxReal m_fVal2;
    NxVec3 m_kPoint1;
    NxU32 m_uiCapsuleFlags;
    
    // Mesh, if there is one
    NiPhysXMeshDescPtr m_spMeshDesc;
    
    // Shape created from the descriptor.
    NxShape* m_pkShape;
};

NiSmartPointer(NiPhysXShapeDesc);

#include "NiPhysXShapeDesc.inl"

};
#endif // NIPHYSXSHAPEDESC_H

