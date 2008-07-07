#pragma once
#ifndef LEGACY_M2StreamReader_H
#define LEGACY_M2StreamReader_H
//------------------------------------------------------------------------------
/**
    @class Legacy::M2StreamReader
  
    A stream reader which reads legacy nvx2 binary mesh files.

    NOTE: this class exists purely for debugging and shouldn't be used in
    production code!

    (C) 2007 Radon Labs GmbH
*/    
#include "core/config.h"
#if NEBULA3_LEGACY_SUPPORT
#include "io/streamreader.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/primitivegroup.h"
#include "models/load/modelheaders.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Legacy
{
class M2StreamReader : public IO::StreamReader
{
    DeclareClass(M2StreamReader);
public:
    /// contructor
    M2StreamReader();
    /// destructor
    virtual ~M2StreamReader();
    /// begin reading from the stream, read entire data
    virtual bool Open();
    /// end reading from the stream, destroys loaded objects
    virtual void Close();
    /// get vertex buffer
    const Ptr<CoreGraphics::VertexBuffer>& GetVertexBuffer() const;
    /// get index buffer
    const Ptr<CoreGraphics::IndexBuffer>& GetIndexBuffer() const;
    /// get primitive groups
    const Util::Array<CoreGraphics::PrimitiveGroup>& GetPrimitiveGroups() const;

private:
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
	/// fix coordiate system
	Math::vector FixCoordSystem(Math::vector &v);

    Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
    Ptr<CoreGraphics::IndexBuffer> indexBuffer;
    Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
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
    Util::Array<CoreGraphics::VertexComponent> vertexComponents;

	ModelHeader *header;
	ModelView *viewPtr;
	ModelGeoset *opsPtr;
	ModelTexUnit *texPtr;
	ModelTextureDef *texDefPtr;
	uint16 *texLookUpPtr;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::VertexBuffer>&
M2StreamReader::GetVertexBuffer() const
{
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::IndexBuffer>&
M2StreamReader::GetIndexBuffer() const
{
    return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<CoreGraphics::PrimitiveGroup>&
M2StreamReader::GetPrimitiveGroups() const
{
    return this->primGroups;
}

} // namespace Legacy
//------------------------------------------------------------------------------
#endif // LEGACY_M2StreamReader_H
#endif // NEBULA3_LEGACY_SUPPORT