//------------------------------------------------------------------------------
//  managers/skillmanager.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fskillmanager.h"
#include "core/factory.h"
#include "util/guid.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"

//#include "story/and.h"
//#include "story/not.h"
//#include "story/or.h"

namespace Fighter
{
ImplementClass(Fighter::FSkillManager, 'SKMG', Core::RefCounted);
ImplementSingleton(Fighter::FSkillManager);

using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
FSkillManager::FSkillManager():
isOpen(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FSkillManager::~FSkillManager()
{
    DestructSingleton;
	if (this->IsOpen())
	{
		this->Close();
	}
}

void 
FSkillManager::Open()
{
	n_assert(!this->isOpen);
	if (!this->ParesSkillList())
    {
        n_error("Managers::SkillManager: Error parsing data:skilllist.xml!");
    }
	this->isOpen = true;
}

void 
FSkillManager::Close()
{
	n_assert(this->isOpen);
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
FSkillManager::ParesSkillList()
{
	IoServer* ioServer = IoServer::Instance();
	// create XML reader on file
	Ptr<Stream> stream = ioServer->CreateStream("data:skilllist.xml");
	Ptr<XmlReader> xmlReader = XmlReader::Create();
	xmlReader->SetStream(stream);
	if (xmlReader->Open())
	{
		if (xmlReader->HasNode("/SkillList"))
		{
			xmlReader->SetToNode("/SkillList");
			if (xmlReader->SetToFirstChild("Skill")) do
			{
				SkillInfo info;
				if (xmlReader->HasAttr("KeyValue"))
					info.keyValue = xmlReader->GetInt("KeyValue");
				if (xmlReader->HasAttr("SP"))
					info.sp = xmlReader->GetInt("SP");
				if (xmlReader->HasAttr("MP"))
					info.mp = xmlReader->GetInt("MP");
				if (xmlReader->HasAttr("AP"))
					info.ap = xmlReader->GetInt("AP");
				if (xmlReader->HasAttr("VP"))
					info.vp = xmlReader->GetInt("VP");
				if (xmlReader->HasAttr("Delay"))
					info.delay = xmlReader->GetInt("Delay");
				if (xmlReader->HasAttr("animName"))
					info.animName = xmlReader->GetString("animName");
				this->skillList.Add(info.keyValue, info);
			}
			while (xmlReader->SetToNextChild("Skill"));
		}
	}

	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
FSkillManager::HasSkillValue(DWORD val)const
{
	if (this->skillList.Contains(val))
		return true;
	return false;
}
//------------------------------------------------------------------------------
/**
*/
SkillInfo 
FSkillManager::GetSkillValue(DWORD val)const
{
	return this->skillList[val];
}

} // namespace Managers
