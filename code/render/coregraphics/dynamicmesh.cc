//------------------------------------------------------------------------------
//  ndynamicmesh.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "coregraphics/dynamicmesh.h"
#include "resources/sharedresourceserver.h"
#include "resources/dynamicmeshresourceloader.h"
#include "coregraphics/mesh.h"
#include "coregraphics/renderdevice.h"

//------------------------------------------------------------------------------
/**
*/
namespace CoreGraphics
{
ImplementClass(DynamicMesh, 'DYMH', Core::RefCounted);

using namespace Resources;

DynamicMesh::DynamicMesh() :
    indexedRendering(true),
		primitiveType(CoreGraphics::PrimitiveTopology::TriangleList)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
DynamicMesh::~DynamicMesh()
{
	if (this->refMesh.isvalid())
		Resources::SharedResourceServer::Instance()->UnregisterSharedResource(this->refMesh.upcast<Resource>());
	this->refMesh = 0;
}

//------------------------------------------------------------------------------
/**
    Return true if object is valid, otherwise Initialize() must be called
    to prepare the object for rendering. A dynamic mesh can become
    invalid anytime, thus make sure you check before each call to
    Begin()/End().
*/
bool
DynamicMesh::IsValid() const
{
    if (this->refMesh.isvalid())
    {
		if (!this->refMesh->IsLoaded())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Initialize the dynamic mesh. This will create or lookup a mesh
    which is shared with all other dynamic mesh objects with the
    same vertex components and usage flags.
    This method must be called whenever a call to IsValid() returns false.

    @param  primType            primitive type
    @param  vertexLayout        vertex component mask
    @param  indexed             true if indexed primitive rendering is intended
    @return                     true if initialized successful
*/
bool
DynamicMesh::Initialize(const Util::Atom<Util::String>& resourceID,CoreGraphics::PrimitiveTopology::Code primType,const Util::Array<CoreGraphics::VertexComponent>& vertexComponents,
                        bool indexed, SizeT vertexSize, SizeT indexSize)
{
    this->primitiveType = primType;
    this->indexedRendering = indexed;
	this->vertexSize = vertexSize;
	this->indexSize = indexSize;

	Ptr<CoreGraphics::Mesh> mesh = 0;
    if (!this->refMesh.isvalid())
    {
  
		Resources::ResourceId meshID(resourceID);
        n_assert(!Resources::SharedResourceServer::Instance()->HasSharedResource(meshID));
		mesh = Resources::SharedResourceServer::Instance()->CreateSharedResource(meshID, CoreGraphics::Mesh::RTTI,Resources::DynamicMeshResourceLoader::RTTI).downcast<CoreGraphics::Mesh>();
		
		n_assert(mesh.isvalid());
        n_assert(mesh);
        this->refMesh = mesh;
    }
    else
    {
        mesh = this->refMesh;
    }

    // initialize the mesh
    if (!mesh->IsLoaded())
    {
		Ptr<Resources::DynamicMeshResourceLoader> dynMeshLoader;
		dynMeshLoader = mesh->GetLoader().downcast<Resources::DynamicMeshResourceLoader>();
		dynMeshLoader->SetNumberOfVertices(vertexSize);

		Ptr<CoreGraphics::VertexLayout> tmpVertexLayout = CoreGraphics::VertexLayout::Create();
		dynMeshLoader->SetVertexComponents(vertexComponents);
	
		//set the vertex buffers access mode and usage 
		dynMeshLoader->SetVertexAccessMode(CoreGraphics::VertexBuffer::AccessWrite);
		dynMeshLoader->SetVertexUsage(CoreGraphics::VertexBuffer::UsageDynamic);

		//calculate vertex width (numbytes/float)
		SizeT numComponentBytes = 0;
		SizeT i = 0;
		for(;i < vertexComponents.Size(); i++)
		{
			numComponentBytes += vertexComponents[i].GetByteSize();
		}
		
		SizeT vertexWidth = numComponentBytes/sizeof(float);
		dynMeshLoader->SetVertexWidth(vertexWidth);
		CoreGraphics::PrimitiveGroup primGroup;
		primGroup.SetBaseVertex(0);
		primGroup.SetNumVertices(vertexSize);
        primGroup.SetPrimitiveTopology(primType);
				
        if (indexed)
        {
			//init index buffer
			dynMeshLoader->SetIndexBufferType(CoreGraphics::IndexType::Index16);
			dynMeshLoader->SetNumberOfIndices(indexSize);
			
			//set the index buffers access mode and usage 
			dynMeshLoader->SetIndexAccessMode(CoreGraphics::IndexBuffer::AccessWrite);
			dynMeshLoader->SetIndexUsage(CoreGraphics::IndexBuffer::UsageDynamic);

			primGroup.SetBaseIndex(0);
			primGroup.SetNumIndices(0);
	    }
        else
        {
			dynMeshLoader->SetNumberOfIndices(0);
        }
		Util::Array<CoreGraphics::PrimitiveGroup> primGroupArray;
		primGroupArray.Append(primGroup);
		mesh->SetPrimitiveGroups(primGroupArray); 
        mesh->Load();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
DynamicMesh::BeginIndexed(ushort*& indexPointer)
{
	indexPointer   = (ushort*)refMesh->GetIndexBuffer()->Map(CoreGraphics::IndexBuffer::MapWriteDiscard);
}

//void
//DynamicMesh::BeginIndexed(ushort*& indexPointer, SizeT offset, SizeT size)
//{
//	indexPointer   = (ushort*)refMesh->GetIndexBuffer()->Map(CoreGraphics::IndexBuffer::MapNOOverWriteDiscard, offset, size);
//}

//------------------------------------------------------------------------------
/**
*/
void
DynamicMesh::EndIndexed()
{
	refMesh->GetIndexBuffer()->Unmap();
}

//------------------------------------------------------------------------------
/**
*/
void
DynamicMesh::Begin(void*& vertexPointer)
{
	vertexPointer  = (void*)refMesh->GetVertexBuffer()->Map(CoreGraphics::VertexBuffer::MapWriteDiscard);
}

void
DynamicMesh::Begin(void*& vertexPointer, SizeT offset, SizeT size)
{
	vertexPointer  = (void*)refMesh->GetVertexBuffer()->Map(CoreGraphics::VertexBuffer::MapWriteNoOverwrite, offset, size);
}

//------------------------------------------------------------------------------
/**
    Finish non-indexed rendering. 

    @param  numVertices     number of valid vertices in the vertex buffer
*/
void
DynamicMesh::End()
{
	this->refMesh->GetVertexBuffer()->Unmap();
}

inline void 
DynamicMesh::Draw(SizeT curGroup)
{
	if (this->refMesh->GetNumPrimitiveGroups() < curGroup)
		return;

	RenderDevice::Instance()->SetPrimitiveGroup(this->refMesh->GetPrimitiveGroupAtIndex(curGroup));
	RenderDevice::Instance()->Draw();
}

};// namespace
