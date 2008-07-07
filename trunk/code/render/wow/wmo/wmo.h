#pragma once
#ifndef WOW_WMO_H
#define WOW_WMO_H
//------------------------------------------------------------------------------
/**
    @class Models::WMO
    
	2008 ctuo
*/
#include "resources/resource.h"
#include "math/bbox.h"
//#include "models/modelnode.h"
//#include "attr/attributecontainer.h"
//#include "models/visresolvecontainer.h"
//#include "wow/m2/modelheaders.h"
#include "math/vector3.h"
#include "math/vector.h"
#include "graphics/modelentity.h"
#include "graphics/sorting/convexzone.h"
//#include "graphics/sorting/wmozone.h"
#include "models/managedmodel.h"

namespace Graphics
{
	class ConvexZone;
}

//------------------------------------------------------------------------------
namespace WOW
{
class WMOReader;

struct WMOMaterial {
	int flags;
	int specular;
	int transparent;
	int nameStart;
	unsigned int col1;
	int d3;
	int nameEnd;
	unsigned int col2;
	int d4;
	float f1,f2;
	int dx[5];
	// read up to here -_-
	int tex;
};

struct WMOLight {
	unsigned int flags, color;
	Math::vector3 pos;
	float intensity;
	float unk[5];
	float r;

	Math::float4 fcolor;

	//void init(MPQFile &f);
	//void setup(GLint light);

	//static void setupOnce(GLint light, Math::vector3 dir, Math::vector3 lcol);
};

struct WMOPV {
	Math::vector3 a,b,c,d;
};

struct WMOPR {
	short portal, group, dir, reserved;
};

struct WMODoodadSet {
	char name[0x14];
	int start;
	int size;
	int unused;
};

struct WMOLiquidHeader {
	int X, Y, A, B;
	Math::vector3 pos;
	short type;
};

struct WMOFog {
	unsigned int flags;
	Math::vector3 pos;
	float r1, r2, fogend, fogstart;
	unsigned int color1;
	float f2;
	float f3;
	unsigned int color2;
	// read to here (0x30 bytes)
	Math::float4 color;
	//void init(MPQFile &f);
	//void setup();
};

struct ModelInfo 
{
	Util::String m2Name;
	Math::vector pos;
	Math::vector dir;
	float w;
	float sc;
};

class WMO : public Resources::Resource
{
    DeclareClass(WMO);
public:
	//------------------------------------------------------------------------------
	/**
		为了让ConvexZone更好的独立使用，特别建立一个Group类，用来存放一些WMO的Group信息
		这些信息与portal处理没太大关系，所以不放在ConvexZone中.使ConvexZone更通用
	*/
	class Group
	{
	public:
		///
		Group();
		///
		~Group();
		///
		void SetDDR(short* d);
		///
		short GetDDR(int index);
		///
		void SetBoundBox(const Math::bbox& b);
		///
		Math::bbox& GetBoundBox();
		///
		void SetDoodads(int num);
		///
		int GetDoodads();
		/// 
		void SetConvexZone(const Ptr<Graphics::ConvexZone>& g);
		/// 
		const Ptr<Graphics::ConvexZone>& GetConvexZone();
		///
		void SetGroupName(const Resources::ResourceId& name);
		///
		const Resources::ResourceId& GetGroupName();
		///
		bool IsOutDoor()const;
		///
		void SetFlags(int f);
	private:
		short* ddr;
		Math::bbox boundingBox;
		int nDoodads;
		int flages;
		Ptr<Graphics::ConvexZone> gc;
		Resources::ResourceId resId;
	};

    /// constructor
    WMO();
    /// destructor
    virtual ~WMO();

    /// unload the resource, or cancel the pending load
    virtual void Unload();

    /// set the model's local bounding box
    void SetBoundingBox(const Math::bbox& b);
    /// get the model's local bounding box
    const Math::bbox& GetBoundingBox() const;


    /// create a ModelInstance of the WMO
	bool CreateInstance(Math::vector& pos, Math::vector& dir, int ndoodadset);
    /// get all attached model instances
    //const Util::Array<Ptr<ModelInstance>>& GetInstances() const;

	/// get texture name
	const Util::String& GetTextureName(IndexT index)const;
	/// get material of group
	const WMOMaterial* GetMaterial(IndexT index)const;
	/// get doodad set
	const WMODoodadSet* GetDoodadSet(IndexT index)const;
	/// get groups
	//const Util::Array<Util::String>& GetGroupsName()const;
	/// 
	//void Rotate(float x0, float y0, float *x, float *y, float angle);
	///
	void SetBoundBox(Math::bbox& b);
	///
	Group* CreateGroup(const Resources::ResourceId& name, Math::bbox& b, int flags);
	///
	WMO::Group*	GetGroup(const Resources::ResourceId& name);
	/// 设置实例化所需的参数
	void SetInstanceInfo(Math::vector& ofs, Math::vector& dir, int doodadset);
	/// 
	bool CheckModelToZone(const Ptr<Graphics::GraphicsEntity>& entity, const Ptr<Graphics::Cell>& zone);

	Math::bbox boundingBox;
    bool inLoadResources;

	/// 保存所有group
	//Util::Array<Ptr<Models::Model>> groups;
	/// 所有用到的m2模型
	Util::Array<Ptr<Models::Model>> modelis;
	/// 实例化group
	Util::Array<Ptr<Graphics::ModelEntity>> instances;

	/// 实例化wmo用到的参数 
	Math::vector ofs;
	Math::vector dir;
	int doodadset;

	int nTextures, nGroups, nP, nLights, nModels, nDoodads, nDoodadSets, nX;
	WMOMaterial *mat;
	Math::vector3 v1, v2;
	bool ok;
	Util::Array<Util::String> textures;
	Util::Array<ModelInfo> models;
	//Util::Array<Resources::ResourceId> groupsName;

	Util::Array<WMOLight> lights;
	Util::Array<WMOPV> pvs;
	Util::Array<WMOPR> prs;
	Util::Array<WMOFog> fogs;
	Util::Array<WMODoodadSet> doodadsets;

	Ptr<Models::Model> skybox;
	int sbid;
	
	//Util::Dictionary<Resources::ResourceId, Group*> groups;
	/// Directionary里也有该数据，不过其中的数据会被排序，而portal设置的时候必需要正确的顺序，所以多建一个数组专做这事。
	Util::Array<Group*> zones;
	/// 用来判断model添加到哪个Zone
	Util::Array<Ptr<Graphics::GraphicsEntity>> modelInstances;
	/// 记录实例化后的指针，方便释放、管理？
	//Util::Array<Ptr<Graphics::ModelEntity>> groupInstances;
	//Util::Array<Ptr<Graphics::ModelEntity>> modelInstances;
};

//------------------------------------------------------------------------------
/**
*/
inline short 
WMO::Group::GetDDR(int index)
{
	n_assert(0 != ddr);
	return this->ddr[index];
}

//------------------------------------------------------------------------------
/**
*/
inline int 
WMO::Group::GetDoodads()
{
	return this->nDoodads;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
WMO::Group::SetFlags(int f)
{
	this->flages = f;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
WMO::Group::IsOutDoor()const
{
	return !(this->flages&&0x2000);
}

//------------------------------------------------------------------------------
/**
*/
inline Math::bbox& 
WMO::Group::GetBoundBox()
{
	return this->boundingBox;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
WMO::Group::SetDDR(short* d)
{
	n_assert(0 != d);
	this->ddr = d;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
WMO::Group::SetBoundBox(const Math::bbox& b)
{
	//boundingBox.extend(b);
	boundingBox = b;

	if (0 != gc)
		gc->SetBoundingBox(boundingBox);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
WMO::Group::SetDoodads(int num)
{
	this->nDoodads = num;
}



//------------------------------------------------------------------------------
/**
*/
inline void 
WMO::SetInstanceInfo(Math::vector& ofs, Math::vector& dir, int doodadset)
{
	this->ofs = ofs;
	this->dir = dir;
	this->doodadset = doodadset;
}

//------------------------------------------------------------------------------
/**
*/
inline void
WMO::SetBoundingBox(const Math::bbox& b)
{
    this->boundingBox = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox&
WMO::GetBoundingBox() const
{
    return this->boundingBox;
}

//------------------------------------------------------------------------------
/**
*/
//inline const Util::Array<Ptr<ModelInstance>>&
//WMO::GetInstances() const
//{
//    return this->instances;
//}

//------------------------------------------------------------------------------
/**
*/
//inline const Util::Array<Util::String>&
//WMO::GetGroupsName() const
//{
//	return this->groupsName;
//}

//------------------------------------------------------------------------------
/**
*/
inline void 
WMO::SetBoundBox(Math::bbox& b)
{
	boundingBox.extend(b);
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    