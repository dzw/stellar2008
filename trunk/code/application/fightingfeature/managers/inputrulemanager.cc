//------------------------------------------------------------------------------
//  managers/inputrulemanager.cc
//  (C) 2009 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/factory.h"
#include "util/guid.h"
#include "managers/inputrulemanager.h"
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
ImplementClass(InputRuleManager, 'IRMG', Game::Manager);
ImplementSingleton(InputRuleManager);

using namespace Game;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
InputRuleManager::InputRuleManager()
{
    ConstructSingleton;
    if (!this->ParesRule())
    {
        n_error("Managers::InputRuleManager: Error parsing data:inputrule.xml!");
    }
}

//------------------------------------------------------------------------------
/**
*/
InputRuleManager::~InputRuleManager()
{
	this->inputRule.SetSize(0);
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool 
InputRuleManager::ParesRule()
{
	if (this->inputRule.Size() == 0)
	{
		IoServer* ioServer = IoServer::Instance();
		// create XML reader on file
		Ptr<Stream> stream = ioServer->CreateStream("data:inputrule.xml");
		Ptr<XmlReader> xmlReader = XmlReader::Create();
		xmlReader->SetStream(stream);
		if (xmlReader->Open())
		{
			int charSize = 0;
			if (xmlReader->HasNode("InputRule"))
			{
				xmlReader->SetToNode("InputRule");
				n_assert(xmlReader->HasAttr("CharSize"));
				charSize = xmlReader->GetInt("CharSize");
			}
			this->inputRule.SetSize(charSize);
			if (xmlReader->HasNode("Position"))
			{
				xmlReader->SetToNode("Position");
				n_assert(xmlReader->HasAttr("Num"));
				int pos = xmlReader->GetInt("Num");
				if (pos < (int)this->inputRule.Size())
				{
					if (xmlReader->SetToFirstChild("Key")) do
					{
						n_assert(xmlReader->HasAttr("KeyValue"));
						this->inputRule[pos].InsertSorted(xmlReader->GetInt("KeyValue"));
					}
					while (xmlReader->SetToNextChild("Key"));
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------------
/**
	获得本次输入的值
*/
DWORD 
InputRuleManager::CheckInvalid(const Util::Array<DWORD>& buffer)const
{
	DWORD val = 0;
	for (SizeT i = 0; i < buffer.Size(); i++)
	{
		if (i >= this->inputRule.Size())
		{
			break;
		}

		IndexT index = this->inputRule[i].BinarySearchIndex(buffer[i]);
		if (index != InvalidIndex)
		{
			val += buffer[i];
		}
	}

	return val;
}

} // namespace Managers
