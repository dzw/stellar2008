#pragma once
#ifndef KOK_TERRAINREADER_H
#define KOK_TERRAINREADER_H
//------------------------------------------------------------------------------
/**
    @class KOK::TerrainReader
    
    (C) 2008 cTuo
*/
#include "models/load/modelreader.h"
#include "util/string.h"
#include "resources/resourceid.h "
#include "kokterrain/terrain.h"

//------------------------------------------------------------------------------
namespace KOK
{

class TerrainReader : public Models::ModelReader
{
    DeclareClass(TerrainReader);
public:
    /// constructor
    TerrainReader();
    /// destructor
    virtual ~TerrainReader();

    /// begin reading from the stream
    virtual bool Open();
    /// end reading from the stream
    virtual void Close();
    /// parse resource
    virtual bool FillModel();
private:
	void ReadString(Util::String& str, int len);
	/// 解析所有段
	int LoadToField();
	/// 添加模型
	void LoadModels(/*const Ptr<Stream>& stream*/);
	/// 添加声音
	void LoadAudios(/*const Ptr<Stream>& stream*/);

	BYTE* mapBuffer;
	Resources::ResourceId modelResId;
	Ptr<Terrain> terrain;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    