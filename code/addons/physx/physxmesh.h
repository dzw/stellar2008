#ifndef PHYSX_PHYSXMESH_H
#define PHYSX_PHYSXMESH_H
//------------------------------------------------------------------------------
/**
    @class Physics::PhysXMesh
    
    Holds the geometry data for a collide mesh. Meshes are usually cached in 
    the MeshCache, so that identical meshes are only loaded once.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "coregraphics/primitivegroup.h"
#include "coregraphics/cpuvertexbuffer.h"
#include "coregraphics/cpuindexbuffer.h"

//------------------------------------------------------------------------------
namespace PhysX
{
class PhysXMesh : public Core::RefCounted
{
    DeclareClass(PhysXMesh);
public:
    /// constructor
    PhysXMesh();
    /// destructor
    virtual ~PhysXMesh();
    /// set filename of mesh
    void SetFilename(const Util::String& n);
    /// get filename of mesh
    const Util::String& GetFilename() const;
    /// load the mesh data
    bool Load();
    /// unload the mesh data
    void Unload();
    /// return true if mesh data is loaded
    bool IsLoaded() const;
    /// get number of mesh groups
    int GetNumGroups() const;
    /// mesh group at index
    const CoreGraphics::PrimitiveGroup& GetGroupAt(int index) const;
    /// get overall number of vertices
    int GetNumVertices() const;
    /// get overall number of indices
    int GetNumIndices() const;
    /// get vertex byte size
    int GetVertexByteSize() const;
    /// get pointer to complete vertex buffer
    float* GetVertexPointer() const;
    /// get pointer to index buffer
    int* GetIndexPointer() const;
    /// unmap vertex buffer
    void UnmapVertexBuffer();
    /// unmap index buffer
    void UnmapIndexBuffer();
    /// get number of vertices in group
    int GetGroupNumVertices(int groupIndex) const;
    /// get number of indices in group
    int GetGroupNumIndices(int groupIndex) const;
    /// get pointer to first vertex in group
    float* GetGroupVertexPointer(int groupIndex) const;
    /// get pointer to first index in group
    int* GetGroupIndexPointer(int groupIndex) const;

    /// begin append mesh
    void BeginAppendMesh(int numVertices, int numIndices, int vertexWidth);
    /// append mesh from file to buffer
    bool AppendMesh(const Ptr<PhysXMesh>& mesh, const Math::matrix44& transform);
    /// end append
    void EndAppendMesh();
    /// free buffer
    void FreeBuffer();    

private:
    /// update the group bounding boxes (slow!)
    void UpdateGroupBoundingBoxes(float* vertexBufferData, int* indexBufferData);

    Util::String filename;
   
    Ptr<CoreGraphics::CPUVertexBuffer> vertexBuffer;
    Ptr<CoreGraphics::CPUIndexBuffer> indexBuffer;
    
    bool isLoaded;
    bool inAppend;
    uint vertexIndex;
    uint indicesIndex;

    Util::Array<CoreGraphics::PrimitiveGroup> meshGroups;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysXMesh::SetFilename(const Util::String& n)
{
    this->filename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
PhysXMesh::GetFilename() const
{
    return this->filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
PhysXMesh::IsLoaded() const
{
    return this->isLoaded;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysXMesh::GetNumGroups() const
{
    n_assert(this->isLoaded);
    return this->meshGroups.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const CoreGraphics::PrimitiveGroup& 
PhysXMesh::GetGroupAt(int index) const
{
    n_assert(this->isLoaded);
    return this->meshGroups[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysXMesh::GetNumVertices() const
{
    n_assert(this->isLoaded);
    n_assert(this->vertexBuffer.isvalid());
    return this->vertexBuffer->GetNumVertices();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysXMesh::GetNumIndices() const
{
    n_assert(this->isLoaded);
    n_assert(this->indexBuffer.isvalid());
    return this->indexBuffer->GetNumIndices();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysXMesh::GetVertexByteSize() const
{
    n_assert(this->isLoaded);
    n_assert(this->vertexBuffer.isvalid());
    return this->vertexBuffer->GetVertexLayout()->GetVertexByteSize();
}

//------------------------------------------------------------------------------
/**
*/
inline
float*
PhysXMesh::GetVertexPointer() const
{
    n_assert(this->isLoaded && this->vertexBuffer.isvalid());    
    return (float*)this->vertexBuffer->Map(Base::ResourceBase::MapRead);
}

//------------------------------------------------------------------------------
/**
*/
inline
int*
PhysXMesh::GetIndexPointer() const
{
    n_assert(this->isLoaded && this->indexBuffer.isvalid());
    return (int*)this->indexBuffer->Map(Base::ResourceBase::MapRead);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PhysXMesh::UnmapVertexBuffer()
{
    this->vertexBuffer->Unmap();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PhysXMesh::UnmapIndexBuffer()
{
    this->indexBuffer->Unmap();
}

}
//------------------------------------------------------------------------------
#endif
