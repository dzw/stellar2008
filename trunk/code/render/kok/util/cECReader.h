//---------------------------------------------------------------------------
#ifndef cECReaderH
#define cECReaderH
//---------------------------------------------------------------------------
//#include "..\NHelper\cBaseReader.h"
#include "NHelper.h"
//---------------------------------------------------------------------------
#define EC_MAXLEN_TOKEN    4096 // 用个 4k Byte 吧.
//---------------------------------------------------------------------------
class cECReader : public cBaseReader
{
public:
	struct stKEYWORDITEM
	{
		char  *m_szName;
		int    m_iTokenType;
	};
	enum {
		ttCOMMENT = ttBASEMAX, // 注解
		ttIDENTIFIER,          // 识别字
		ttFLOAT,               // 浮点数
		ttINT,                 // 整数
		ttSTRING,              // 字串
		ttTRUE,                // true
		ttFALSE,               // false
		ttECMAX                // 衍生类别的tokenType, 要从这个数值开始宣告起.
	};
protected:
	char m_szToken[EC_MAXLEN_TOKEN];
	void EatSpace();
	int  ParseQuotedString(); // 使用双引号框起来的字串
	void ParseIdentifier();   // 
	int  OnGotIdentifier();
public:
	cECReader();
	virtual ~cECReader();
	// 取得下一个字节
	int GetToken();
	char* GetTokenStr() { return m_szToken; }
	int   GetTokenInt();
	double GetTokenDouble();

	// 070711 cyhsieh 经验值
	__int64 GetTokenInt64( void );
};
//---------------------------------------------------------------------------
#endif
