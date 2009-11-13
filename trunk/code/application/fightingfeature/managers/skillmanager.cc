//------------------------------------------------------------------------------
//  managers/skillmanager.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/factory.h"
#include "util/guid.h"
#include "managers/skillmanager.h"
#include "game/property.h"
#include "game/entity.h"
#include "basegameattr/basegameattributes.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "basegamefeatureunit.h"

//#include "story/and.h"
//#include "story/not.h"
//#include "story/or.h"

namespace FightingFeature
{
ImplementClass(SkillManager, 'SKMG', Game::Manager);
ImplementSingleton(SkillManager);

using namespace Game;
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
SkillManager::SkillManager()
{
    ConstructSingleton;
    if (!this->ParesSkillList())
    {
        n_error("Managers::SkillManager: Error parsing data:skilllist.xml!");
    }
}

//------------------------------------------------------------------------------
/**
*/
SkillManager::~SkillManager()
{
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool 
SkillManager::ParesSkillList()
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
SkillManager::HasSkillValue(DWORD val)const
{
	if (this->skillList.Contains(val))
		return true;
	return false;
}
//------------------------------------------------------------------------------
/**
*/
SkillInfo 
SkillManager::GetSkillValue(DWORD val)const
{
	return this->skillList[val];
}

} // namespace Managers
