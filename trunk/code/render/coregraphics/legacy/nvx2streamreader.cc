//------------------------------------------------------------------------------
//  nvx2streamreader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/legacy/nvx2streamreader.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "system/byteorder.h"

#if NEBULA3_LEGACY_SUPPORT
namespace Legacy
{
ImplementClass(Legacy::Nvx2StreamReader, 'N2SR', IO::StreamReader);

using namespace CoreGraphics;
using namespace Util;
using namespace Math;
using namespace Resources;
using namespace System;

//------------------------------------------------------------------------------
/**
*/
Nvx2StreamReader::Nvx2StreamReader() :
    mapPtr(0),
    groupDataPtr(0),
    vertexDataPtr(0),
    indexDataPtr(0),
    groupDataSize(0),
    vertexDataSize(0),
    indexDataSize(0),
    numGroups(0),
    numVertices(0),
    vertexWidth(0),
    numIndices(0),
    numEdges(0),
    vertexComponentMask(0)
{
	// 可用VertexComponent之和
	this->validVertexComponentMask = 0x7FF;
}

//------------------------------------------------------------------------------
/**
*/
Nvx2StreamReader::~Nvx2StreamReader()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Nvx2StreamReader::Open()
{
    n_assert(0 == this->primGroups.Size());
    n_assert(this->stream->CanBeMapped());
    n_assert(this->vertexComponents.IsEmpty());
    n_assert(0 == this->mapPtr);
    if (StreamReader::Open())
    {
        // map the stream to memory
        this->mapPtr = this->stream->Map();
        n_assert(0 != this->mapPtr);

        // read data
        this->ReadHeaderData();
        this->ReadPrimitiveGroups();
        this->SetupVertexComponents();
        this->SetupVertexBuffer();
        this->SetupIndexBuffer();
        this->UpdateGroupBoundingBoxes();

        stream->Unmap();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Nvx2StreamReader::Close()
{
    this->mapPtr = 0;
    this->groupDataPtr = 0;
    this->vertexDataPtr = 0;
    this->indexDataPtr = 0;
    this->vertexBuffer = 0;
    this->indexBuffer = 0;
    this->primGroups.Clear();
    this->vertexComponents.Clear();
    StreamReader::Close();
}

//------------------------------------------------------------------------------
/**
    This reads the nvx2 header data and checks whether the file is 
    actually an nvx2 file through the magic number. All header data
    will be read into member variables, and pointers to the
    start of the group-, vertex- and index-data will be setup.
*/
void
Nvx2StreamReader::ReadHeaderData()
{
    n_assert(0 != this->mapPtr);
    uint* headerPtr = (uint*) this->mapPtr;

    // check nvx2 magic number
    FourCC magic = ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host, *headerPtr++);
    if (magic != FourCC('NVX2'))
    {
        // not a nvx2 file, break hard
        n_error("Nvx2StreamReader: '%s' is not a nvx2 file!", this->stream->GetURI().AsString().AsCharPtr());
    }
    this->numGroups = ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host, *headerPtr++);
    this->numVertices = ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host,*headerPtr++);
    this->vertexWidth = ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host,*headerPtr++);
    this->numIndices = ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host,*headerPtr++) * 3;
    this->numEdges = ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host,*headerPtr++);
    this->vertexComponentMask = ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host,*headerPtr++);
	this->groupDataSize = 6 * sizeof(uint) * this->numGroups;
    this->vertexDataSize = this->numVertices * this->vertexWidth * sizeof(float);
    this->indexDataSize = this->numIndices * sizeof(ushort);
    this->groupDataPtr = headerPtr;
    this->vertexDataPtr = ((uchar*)this->groupDataPtr) + this->groupDataSize;
    this->indexDataPtr = ((uchar*)this->vertexDataPtr) + this->vertexDataSize;
}

//------------------------------------------------------------------------------
/**
*/
void
Nvx2StreamReader::ReadPrimitiveGroups()
{
    n_assert(this->primGroups.IsEmpty());
    n_assert(this->numGroups > 0);
    n_assert(0 != this->groupDataPtr);
    uint* ptr = (uint*) this->groupDataPtr;
    IndexT i;
    for (i = 0; i < this->numGroups; i++)
    {
        PrimitiveGroup primGroup;
        primGroup.SetBaseVertex(ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host, *ptr++));
        primGroup.SetNumVertices(ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host, *ptr++));
        primGroup.SetBaseIndex(ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host, *ptr++) * 3);   // firstTriangle
        primGroup.SetNumIndices(ByteOrder::ConvertUInt(ByteOrder::LittleEndian, ByteOrder::Host, *ptr++) * 3);   // numTriangles
        ptr += 2; // skip firstEdge and numEdges
        primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
        this->primGroups.Append(primGroup);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Nvx2StreamReader::SetupVertexComponents()
{
    n_assert(this->vertexComponents.IsEmpty());

    IndexT i;
    for (i = 0; i < 22; i++)
    {
        VertexComponent::SemanticName sem;
        VertexComponent::Format fmt;
        IndexT index = 0;
        if (vertexComponentMask & (1<<i))
        {
            switch (i)
            {
                case 0:     sem = VertexComponent::Position;     fmt=VertexComponent::Float3; break;
                case 1:     sem = VertexComponent::Normal;       fmt=VertexComponent::Float3; break;
				case 2:     sem = VertexComponent::Normal;       fmt=VertexComponent::UByte4N; break;
                case 3:     sem = VertexComponent::TexCoord;     fmt=VertexComponent::Float2; index = 0; break;
				case 4:     sem = VertexComponent::TexCoord;     fmt=VertexComponent::Short2; index = 0; break;
                case 5:     sem = VertexComponent::TexCoord;     fmt=VertexComponent::Float2; index = 1; break;
				case 6:     sem = VertexComponent::TexCoord;     fmt=VertexComponent::Short2; index = 1; break;
                case 7:     sem = VertexComponent::TexCoord;     fmt=VertexComponent::Float2; index = 2; break;
				case 8:     sem = VertexComponent::TexCoord;     fmt=VertexComponent::Short2; index = 2; break;
                case 9:     sem = VertexComponent::TexCoord;     fmt=VertexComponent::Float2; index = 3; break;
				case 10:    sem = VertexComponent::TexCoord;     fmt=VertexComponent::Short2; index = 3; break;
                case 11:    sem = VertexComponent::Color;        fmt=VertexComponent::Float4;  break;
				case 12:    sem = VertexComponent::Color;        fmt=VertexComponent::UByte4N; break;
                case 13:    sem = VertexComponent::Tangent;      fmt=VertexComponent::Float3;  break;
				case 14:    sem = VertexComponent::Tangent;      fmt=VertexComponent::UByte4N; break;
                case 15:    sem = VertexComponent::Binormal;     fmt=VertexComponent::Float3;  break;
				case 16:    sem = VertexComponent::Binormal;     fmt=VertexComponent::UByte4N; break;
                case 17:    sem = VertexComponent::SkinWeights;  fmt=VertexComponent::Float4;  break;
				case 18:    sem = VertexComponent::SkinWeights;  fmt=VertexComponent::UByte4N; break;
                case 19:    sem = VertexComponent::SkinJIndices; fmt=VertexComponent::Float4;  break;
				case 20:    sem = VertexComponent::SkinJIndices; fmt=VertexComponent::UByte4; break;
				case 21:    sem = VertexComponent::Position;	 fmt=VertexComponent::Float4;  break;
                default:
                    n_error("Can't happen!");
                    sem = VertexComponent::Position;
                    fmt = VertexComponent::Float3;
                    break;
            }
            this->vertexComponents.Append(VertexComponent(sem, index, fmt));
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
int
Nvx2StreamReader::GetVertexWidthFromMask(uint mask)
{
	int width = 0;
	IndexT i;
	for (i = 0; i < 11; i++)
	{
		if (mask & (1<<i))
		{
			switch (i)
			{
			case 0:     width += 3; break;
			case 1:     width += 3; break;
			case 2:     width += 2; break;
			case 3:     width += 2; break;
			case 4:     width += 2; break;
			case 5:     width += 2; break;
			case 6:     width += 4; break;
			case 7:     width += 3; break;
			case 8:     width += 3; break;
			case 9:     width += 4; break;
			case 10:    width += 4; break;
			}
		}
	}

	return width;
}

//------------------------------------------------------------------------------
/**
    Since nvx2 files don't contain any bounding box information
    we need to compute per-primitive-group bounding boxes
    manually by walking the triangle indices. This may be inefficient
    with large meshes.
*/
void
Nvx2StreamReader::UpdateGroupBoundingBoxes()
{
    n_assert(0 != this->vertexDataPtr);
    n_assert(0 != this->indexDataPtr);
    n_assert(this->primGroups.Size() > 0);

    float* vertexPtr = (float*) this->vertexDataPtr;
    ushort* indexPtr = (ushort*) this->indexDataPtr;
    IndexT groupIndex;
    for (groupIndex = 0; groupIndex < this->primGroups.Size(); groupIndex++)
    {
        PrimitiveGroup& group = this->primGroups[groupIndex];        
        bbox box;
        box.begin_extend();
        point p;
        IndexT ii;
        for (ii = 0; ii < group.GetNumIndices(); ii++)
        {
            float* curVertexPtr = vertexPtr + (indexPtr[ii + group.GetBaseIndex()] * this->vertexWidth);
            p.set(curVertexPtr[0], curVertexPtr[1], curVertexPtr[2]);
            box.extend(p);
        }
        group.SetBoundingBox(box);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Nvx2StreamReader::SetupVertexBuffer()
{
    n_assert(0 != this->vertexDataPtr);
    n_assert(this->vertexDataSize > 0);
    n_assert(this->numVertices > 0);    
    n_assert(this->vertexComponents.Size() > 0);

    // endian convert the vertex buffer
    #pragma warning(push)   
    #pragma warning(disable : 4127)  // expression is constant
    if (ByteOrder::Host != ByteOrder::LittleEndian)
    {
        IndexT i;
        SizeT num = this->vertexDataSize / sizeof(float);
        float* ptr = (float*) this->vertexDataPtr;
        for (i = 0; i < num; i++)
        {
            ptr[i] = ByteOrder::ConvertFloat(ByteOrder::LittleEndian, ByteOrder::Host, ptr[i]);
        }
    }
    #pragma warning(pop)

    // setup new vertex buffer
    if (!this->vertexBuffer.isvalid())
    {
        this->vertexBuffer = VertexBuffer::Create();
    }
    if (!this->vertexBufferLoader.isvalid())
    {
        this->vertexBufferLoader = MemoryVertexBufferLoader::Create();
    }

	uint componentMask = this->validVertexComponentMask & this->vertexComponentMask;
	if (1 || componentMask == this->vertexComponentMask)
	{
		this->vertexBufferLoader->Setup(this->vertexComponents, this->numVertices, this->vertexDataPtr, this->vertexDataSize);
		this->vertexBuffer->SetLoader(this->vertexBufferLoader.upcast<ResourceLoader>());
		this->vertexBuffer->Load();
		this->vertexBuffer->SetLoader(0);
		this->vertexBufferLoader = 0;
		n_assert(this->vertexBuffer->GetState() == VertexBuffer::Loaded);
	}
	else
	{
		// 过滤未知的component
		int realVertexWidth = GetVertexWidthFromMask(componentMask);
		this->vertexDataSize = this->numVertices * realVertexWidth * sizeof(float);
		this->vertexBufferLoader->Setup(this->vertexComponents, this->numVertices, this->vertexDataSize, VertexBuffer::UsageStaging, VertexBuffer::AccessWrite);
		this->vertexBuffer->SetLoader(this->vertexBufferLoader.upcast<ResourceLoader>());
		this->vertexBuffer->Load();
		this->vertexBuffer->SetLoader(0);
		this->vertexBufferLoader = 0;
		n_assert(this->vertexBuffer->GetState() == VertexBuffer::Loaded);

		void* vertexBufferPtr = this->vertexBuffer.downcast<CoreGraphics::VertexBuffer>()->Map(VertexBuffer::MapWrite);
		ReadVertices(vertexBufferPtr, realVertexWidth, componentMask);
		this->vertexBuffer.downcast<CoreGraphics::VertexBuffer>()->Unmap();
	}
}

void
Nvx2StreamReader::ReadVertices(void* buffer, int realVertexWidth, uint componentMask)
{
	float* destBuf = (float*)buffer;
	uint v = 0;
	for (v = 0; v < this->numVertices; v++)
	{
		float* vBuf = (float*)this->vertexDataPtr + v * this->vertexWidth;

		int bitIndex;
		for (bitIndex = 0; bitIndex < VertexComponent::Invalid; bitIndex++)
		{
			int mask = (1<<bitIndex);

			// skip completely if current vertex component is not in file
			if (0 == (this->vertexComponentMask & mask))
			{
				continue;
			}

			// get width of current vertex component
			int width = GetVertexWidthFromMask(mask);
			n_assert(width > 0);
			if (componentMask & mask)
			{
				// read the vertex component
				int f;
				for (f = 0; f < width; f++)
				{
					*destBuf++ = *vBuf++;
				}
			}
			else
			{
				// skip the vertex component
				vBuf += width;
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Nvx2StreamReader::SetupIndexBuffer()
{
    n_assert(0 != this->indexDataPtr);
    n_assert(this->indexDataSize > 0);
    n_assert(this->numIndices > 0);
    
    // endian convert the index buffer
    #pragma warning(push)   
    #pragma warning(disable : 4127) // expression is constant
    if (ByteOrder::Host != ByteOrder::LittleEndian)
    {
        IndexT i;
        ushort* ptr = (ushort*) this->indexDataPtr;
        for (i = 0; i < this->numIndices; i++)
        {
            ptr[i] = ByteOrder::ConvertUShort(ByteOrder::LittleEndian, ByteOrder::Host, ptr[i]);
        }
    }
    #pragma warning(pop)

    // setup a new index buffer
    if (!this->indexBuffer.isvalid())
    {
        this->indexBuffer = IndexBuffer::Create();
    }
    if (!this->indexBufferLoader.isvalid())
    {
        this->indexBufferLoader = MemoryIndexBufferLoader::Create();
    }
    this->indexBufferLoader->Setup(IndexType::Index16, this->numIndices, this->indexDataPtr, this->indexDataSize);
    this->indexBuffer->SetLoader(this->indexBufferLoader.upcast<ResourceLoader>());
    this->indexBuffer->Load();
    this->indexBuffer->SetLoader(0);
    this->indexBufferLoader = 0;
    n_assert(this->indexBuffer->GetState() == IndexBuffer::Loaded);
}

} // namespace Legacy

#endif // NEBULA3_LEGACY_SUPPORT