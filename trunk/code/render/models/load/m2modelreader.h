#pragma once
#ifndef WOW_M2MODELREADER_H
#define WOW_M2MODELREADER_H
//------------------------------------------------------------------------------
/**
    @class Models::M2ModelReader
    
    Legacy M2 binary reader, reads a subset of .n2 files into a Model.
    NOTE: This class is very inefficient and shouldn't be used
    for production code.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/load/modelreader.h"
#include "io/binaryreader.h"
#include "util/stack.h"
#include "models/load/modelheaders.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/primitivegroup.h"
//#include "wow/m2/m2modelnode.h"
#include "resources/managedmesh.h"

//------------------------------------------------------------------------------
namespace Models
{
class M2ModelReader : public Models::ModelReader
{
    DeclareClass(M2ModelReader);
public:
    /// constructor
    M2ModelReader();
    /// destructor
    virtual ~M2ModelReader();

    /// begin reading from the stream
    virtual bool Open();
    /// end reading from the stream
    virtual void Close();

    /// set the read cursor to the first Model in the stream
    virtual bool SetToFirstModel();
    /// set the read cursor to the next Model in the stream
    virtual bool SetToNextModel();
    /// set the read cursor to the first ModelNode in the current Model
    virtual bool SetToFirstModelNode();
    /// set the read cursor to the next ModelNode in the current Model
    virtual bool SetToNextModelNode();

	/// get vertex buffer
	const Ptr<CoreGraphics::VertexBuffer>& GetVertexBuffer() const;
	/// get index buffer
	const Ptr<CoreGraphics::IndexBuffer>& GetIndexBuffer() const;
	/// get primitive groups
	const Util::Array<CoreGraphics::PrimitiveGroup>& GetPrimitiveGroups() const;

private:
    /// read Model data from stream
    void ReadModelData();
    /// read ModelNode data from stream
    void ReadModelNodeData();

	/// read header data from stream
	void ReadHeaderData();
	/// read primitive groups from stream
	void ReadPrimitiveGroups();
	/// setup vertex components array
	void SetupVertexComponents();
	/// update primitive group bounding boxes
	void UpdateGroupBoundingBoxes();
	/// setup the vertex buffer object
	void SetupVertexBuffer();
	/// setup the index buffer object
	void SetupIndexBuffer();
	/// setup ManagedMesh
	void SetupManagedMesh();

    Ptr<IO::BinaryReader> binaryReader;
    bool isSetToModel;
    bool isSetToModelNode;
    Util::Stack<Util::StringAtom> modelNodeStack;
    int numTexture;

	Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
	Ptr<CoreGraphics::IndexBuffer> indexBuffer;
	Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	unsigned char* mapPtr;
	void* groupDataPtr;
	void* vertexDataPtr;
	void* indexDataPtr;

	SizeT groupDataSize;
	SizeT vertexDataSize;
	SizeT indexDataSize;

	uint numGroups;
	uint numVertices;
	uint vertexWidth;
	uint numIndices;
	uint numEdges;
	uint vertexComponentMask;

	ModelHeader *header;
	ModelView *viewPtr;
	ModelGeoset *opsPtr;
	ModelTexUnit *texPtr;
	ModelTextureDef *texDefPtr;
	ModelRenderFlags *renderFlags;
	uint16 *texLookUpPtr;

	Util::String meshName;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::VertexBuffer>&
M2ModelReader::GetVertexBuffer() const
{
	return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::IndexBuffer>&
M2ModelReader::GetIndexBuffer() const
{
	return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<CoreGraphics::PrimitiveGroup>&
M2ModelReader::GetPrimitiveGroups() const
{
	return this->primGroups;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    