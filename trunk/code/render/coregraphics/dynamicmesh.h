#ifndef CoreGraphics_DYNAMICMESH_H
#define CoreGraphics_DYNAMICMESH_H
//------------------------------------------------------------------------------
/**
    @class DynamicMesh

	this->dynaMesh = DynamicMesh::Create();
	this->dynaMesh->Initialize(ResourceId("d"), PrimitiveTopology::TriangleList, vertexComponents, true, vertexSize*2, indexSize);
	this->dynaMesh->AddPrimitiveGroup(primGroups);
	void* v;
	this->dynaMesh->Begin(v, vertexSize*sizeof(MeshTest), vertexSize*sizeof(MeshTest));
	Memory::Copy(vertices, v, vertexSize*sizeof(MeshTest));
	//Memory::Copy(vertices, (char*)v+(vertexSize*sizeof(MeshTest)), vertexSize*sizeof(MeshTest));
	this->dynaMesh->End();
	ushort* iii;
	this->dynaMesh->BeginIndexed(iii);
	Memory::Copy(indices, iii, indexSize*sizeof(WORD));
	this->dynaMesh->EndIndexed();

	调用AddPrimitiveGroup增加多个渲染组，动态更新每组。
    (C) 2008 cTuo
*/

#include "core/refcounted.h"

#include "coregraphics/primitivetopology.h"
#include "util/string.h"
#include "coregraphics/mesh.h"


//------------------------------------------------------------------------------
namespace CoreGraphics
{
class DynamicMesh : public Core::RefCounted
{
	DeclareClass(DynamicMesh);
	
public:
    /// constructor
    DynamicMesh();
    /// destructor
    ~DynamicMesh();
    /// initialize the dynamic mesh
	bool Initialize(const Util::Atom<Util::String>& resourceID,CoreGraphics::PrimitiveTopology::Code primType, const Util::Array<CoreGraphics::VertexComponent>& vertexComponents, bool indexedRendering, SizeT vertexSize, SizeT indexSize);
    /// if this returns false, call Initialize()
    bool IsValid() const;
    /// begin indexed rendering
    void BeginIndexed(ushort*& indexPointer);
    /// end indexed rendering
    void EndIndexed();
    /// begin non-indexed rendering
    void Begin(void*& vertexPointer);
	/// begin non-indexed nooverwrite
	void Begin(void*& vertexPointer, SizeT offset, SizeT size);
    /// end non-indexed rendering
    void End();
	///
	SizeT GetVertexSize()const;
	///
	SizeT GetIndexSize()const;
	///
	CoreGraphics::PrimitiveGroup GetPrimitiveGroupAtIndex(SizeT index)const;
	///
	void AddPrimitiveGroup(const Util::Array<CoreGraphics::PrimitiveGroup>& primGroupArray);
	/// 设置要画的组
	void ApplyPrimitive(SizeT curGroup);
	/// 直接渲染某组，粒子的时候用到
	void Draw(SizeT curGroup);
protected:
	SizeT vertexSize;
	SizeT indexSize;
    bool indexedRendering;
	Ptr<CoreGraphics::Mesh> refMesh;
	CoreGraphics::PrimitiveTopology::Code primitiveType;
};

inline SizeT 
DynamicMesh::GetVertexSize()const
{
	return this->vertexSize;
}

inline SizeT 
DynamicMesh::GetIndexSize()const
{
	return this->indexSize;
}

inline void 
DynamicMesh::ApplyPrimitive(SizeT curGroup)
{
	//n_assert(curGroup < this->refMesh->GetNumPrimitiveGroups());
	this->refMesh->ApplyPrimitives(curGroup);
}

inline CoreGraphics::PrimitiveGroup 
DynamicMesh::GetPrimitiveGroupAtIndex(SizeT index)const
{
	return this->refMesh->GetPrimitiveGroupAtIndex(index);
}

inline void 
DynamicMesh::AddPrimitiveGroup(const Util::Array<CoreGraphics::PrimitiveGroup>& primGroupArray)
{
	this->refMesh->SetPrimitiveGroups(primGroupArray); 
}

};// namespace Nebula2
//------------------------------------------------------------------------------
#endif

