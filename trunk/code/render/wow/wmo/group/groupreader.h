#pragma once
#ifndef WOW_WMOGROUPREADER_H
#define WOW_WMOGROUPREADER_H
//------------------------------------------------------------------------------
/**
    @class Models::WMOGroupReader
    
    Legacy M2 binary reader, reads a subset of .n2 files into a Model.
    NOTE: This class is very inefficient and shouldn't be used
    for production code.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/load/modelreader.h"
#include "io/binaryreader.h"
#include "util/stack.h"
#include "WOW/m2/modelheaders.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/primitivegroup.h"
//#include "wow/m2/m2modelnode.h"
#include "resources/managedmesh.h"
#include "wow/wmo/wmo.h"
#include "math/vector3.h"
#include "graphics/sorting/bspnode.h"

//------------------------------------------------------------------------------
namespace WOW
{

struct WMOBatch {
	signed char bytes[12];
	unsigned int indexStart;
	unsigned short indexCount, vertexStart, vertexEnd;
	unsigned char flags, texture;
};

struct WMOGroupHeader {
	int nameStart, nameStart2, flags;
	float box1[3], box2[3];
	short portalStart, portalCount;
	short batches[4];
	uint8 fogs[4];
	int32 unk1, id, unk2, unk3;
};

/// planetype 的值为A、B、C，分别代表X,Y,Z轴分割面对齐的轴，fDisk面与轴的距离。
struct t_BSP_NODE 
{
	short planetype;			// 分割面位置
	short children[2];			// index of bsp child node(rightin this array)
	unsigned short numfaces;	// num of triangle faces
	unsigned short firstface;	// index of the first triangle index(in [[WMO#MOBR_chunk | MOBR]])
	short nUnk;					// 0
	float fDisk;				// 分割面D值
};

class WMOGroupReader : public Models::ModelReader
{
    DeclareClass(WMOGroupReader);
public:
    /// constructor
    WMOGroupReader();
    /// destructor
    virtual ~WMOGroupReader();

    /// begin reading from the stream
    virtual bool Open();
    /// end reading from the stream
    virtual void Close();
	/// parse resource and build model hierarchy
	virtual bool FillModel();

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


	/// read primitive groups from stream
	void ReadPrimitiveGroups();
	/// setup vertex components array
	void SetupVertexComponents();
	/// update primitive group bounding boxes
	void UpdateGroupBoundingBoxes();
	/// setup ManagedMesh
	void SetupManagedMesh();
	///
	void ReadGroup();
	///
	void BuildBsp(Ptr<Graphics::BspNode>& parentNode, IndexT index);
	///
	Ptr<Graphics::BspNode> CreateBspNode(IndexT index);

    Ptr<IO::BinaryReader> binaryReader;
    bool isSetToModel;
    bool isSetToModelNode;
    Util::Stack<Util::StringAtom> modelNodeStack;

	Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
	Ptr<CoreGraphics::IndexBuffer> indexBuffer;
	Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
	Util::Array<CoreGraphics::VertexComponent> vertexComponents;

	Util::String meshName;


	Ptr<WMO> wmo;
	WMO::Group* group;

	int flags;
	Math::vector3 v1,v2;
	SizeT nTriangles, nVertices, nIndices;
	//GLuint dl,dl_light;
	Math::vector center;
	float rad;
	int num;
	int fog;
	int nDoodads, nBatches;
	short *ddr;
	//Liquid *lq;
	//std::vector< std::pair<GLuint, int> > lists;
	Util::Dictionary<IndexT, int> lists;

	Math::vector3 b1,b2;
	Math::vector vmin, vmax;
	bool indoor, hascv;
	bool visible;

	bool outdoorLights;
	Util::String name;

	int curBatch;

	WMOBatch *batches;

	SizeT numBsp;
	t_BSP_NODE *bsps;
	unsigned short numBspIndices;
	unsigned short* bspIndices;

	uint16 *indices;

};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::VertexBuffer>&
WMOGroupReader::GetVertexBuffer() const
{
	return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::IndexBuffer>&
WMOGroupReader::GetIndexBuffer() const
{
	return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<CoreGraphics::PrimitiveGroup>&
WMOGroupReader::GetPrimitiveGroups() const
{
	return this->primGroups;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    