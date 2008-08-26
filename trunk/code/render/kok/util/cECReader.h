//---------------------------------------------------------------------------
#ifndef cECReaderH
#define cECReaderH
//---------------------------------------------------------------------------
//#include "..\NHelper\cBaseReader.h"
#include "NHelper.h"
//---------------------------------------------------------------------------
#define EC_MAXLEN_TOKEN    4096 // �ø� 4k Byte ��.
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
		ttCOMMENT = ttBASEMAX, // ע��
		ttIDENTIFIER,          // ʶ����
		ttFLOAT,               // ������
		ttINT,                 // ����
		ttSTRING,              // �ִ�
		ttTRUE,                // true
		ttFALSE,               // false
		ttECMAX                // ��������tokenType, Ҫ�������ֵ��ʼ������.
	};
protected:
	char m_szToken[EC_MAXLEN_TOKEN];
	void EatSpace();
	int  ParseQuotedString(); // ʹ��˫���ſ��������ִ�
	void ParseIdentifier();   // 
	int  OnGotIdentifier();
public:
	cECReader();
	virtual ~cECReader();
	// ȡ����һ���ֽ�
	int GetToken();
	char* GetTokenStr() { return m_szToken; }
	int   GetTokenInt();
	double GetTokenDouble();

	// 070711 cyhsieh ����ֵ
	__int64 GetTokenInt64( void );
};
//---------------------------------------------------------------------------
#endif
