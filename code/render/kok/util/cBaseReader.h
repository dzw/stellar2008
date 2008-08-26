/*----------------------------------------------------------------------*/
// ����: cBaseReader.h
// ˵��:
/*----------------------------------------------------------------------*/
#ifndef cBaseReaderH
#define cBaseReaderH
#include <windows.h>
#include <mbstring.h>
//---------------------------------------------------------------------------
#include "NHelper.h"
//---------------------------------------------------------------------------
class cBaseReader
{
public:
	//------------------------------------------
	// ������
	enum {
		errFileReady,   // ����δ����
		errParsing,     // ������������
		errFileSize,    // ������С����
		errFileOpen,    // ��������
		errAlloc,       // ���������ô���
	};
	//------------------------------------------
	// TOKEN TYPE
	// GetToken() ��ش�������
	// �ڷ��ŵĴ�����,
	// '.' dot ��Ϊ�����ֵ�С����, ��������������
	// '"' dblquote ���ִ����ϵ�����, ��������������
	// '_' underline ���Ե���ʶ���ֵ�һ����, ��������������
	//------------------------------------------
	enum {
		ttIGNORE = -2,   // ���Ե��ͱ�
		ttERROR,         // ����;�з�������
		ttUNKNOWN,       // δ֪
		ttFILEEND,       // ������β '\0'
		ttBRACEOPEN,     // { - �������
		ttBRACECLOSE,    // } - �Ҵ�����
		ttPARENOPEN,     // ( - ��С����
		ttPARENCLOSE,    // ) - ��С����
		ttBRACKETOPEN,   // [ - ��������
		ttBRACKETCLOSE,  // ] - ��������
		ttEQUALMARK,     // = - �Ⱥ�    (equal mark
		ttCOMMA,         // , - ����    (comma
		ttDOT,           // . - С����  (dot
		ttEXCLA,         // ! - ��̾��  (exclamation mark
		ttPLUS,          // + - �Ӻš@  (plus
		ttMINUS,         // - - ���š@  (minus
		ttULINE,         // _ - ����    (underline
		ttSTAR,          // * - �Ǻ�    (star
		ttSLASH,         // / - б��    (slash
		ttPERCENT,       // % - �ٷֱ�  (percent
		ttCOLON,         // : - ð��    (colon
		ttSEMICOLON,     // ; - �ֺ�    (semicolon
		ttHAT,           // ^ - ָ��?   (hat
		ttBELOW,         // < - С��    (below than
		ttGREATER,       // > - ����    (greater than
		ttQUERY,         // ? - �ʺ�    (query
		ttAT,            // @ - С����  (at
		ttAND,           // & - ��      (and
		ttBAR,           // | - ��      (bar
		ttSHARP,         // # - ����    (sharp
		ttBACKSLASH,     // \\ - ��б�� (backslash
		ttQUOTE,         // \' - ������ (quote
		ttDQUOTE,        // \" - ˫���� (dquote
		ttRETURN,        // '\r'
		ttNEXTLINE,      // '\n'
		ttTAB,           // '\t'
		ttBELL,          // '\a'
		ttBACKSPACE,     // '\b'
		ttVERTTAB,       // '\v'
		ttSPACE,         // ' '
		//
		ttNUMBER,        // ��λԪ����
		ttALPHA,         // ��λԪ����
		ttMULTIBYTE,     // ˫λԪ����
		//
		ttBASEMAX        // ��������tokenTypeҪ�����￪ʼ������, ��Ȼ�����.
	};

protected:
	static char m_pchDelimiter[]; // ����
	static int  m_aDelimiter[];   // ���Ŷ�Ӧ��

	char  m_szPicked[3];          // Pick����Ԫ, ��Ϊ���ֻ�е�˫��Ԫ,
	// ����'\0'��β��Ԫ, ������

	int   m_iFileSize;            // �����Ĵ�С
	char* m_pFileData;            // ���浵�����ϵ�λ��
	char* m_pNextChar;            // Ŀǰ������λ��
	char* m_pBackupPtr;           // ���ݵ�ָ��
	bool  m_bAlready;             // ȷ�϶�ȡ���

	int   m_iPickedType;          // Token������
	//- functions
	int  PickChar(char* &pNextChar, bool bPick = true);  // �ش� TokenType
	char* GetNextChar()
	{
		if (m_bAlready == false) return NULL;
		return m_pNextChar;
	}  // �ػ�ش� TokenType, ����д��m_szToken
	// ȡ��һ���ֽ�
	char* GetPicked()  { return m_szPicked; }
	// ����ѶϢ����.
	virtual void CallErrorMsg(int no); // ����ѶϢ
	int  GetStringLength(char* szString)
	{
		if (szString == NULL) return 0;
		if (*szString == '\0') return 0;

		return strlen(szString);
	}
public:
	cBaseReader();
	virtual ~cBaseReader();

	// ����
	bool LoadFromFile(char* fileName);          // �������������
	bool LoadFromMemory(char* data, int size);
	bool LoadFromMemoryX(char* szStringL, char* szContext, char* szStringR);
	// ����
	bool Reset();
	bool IsAlready() { return m_bAlready; }
	bool IsWhiteSpace(int tokenType);
	// С��ʹ��... 
	void BackupPtr(); // ����Ŀǰ�� m_pNextChar
	void ReloadPtr(); // ���ر��ݵ� m_pNextChar
};
//---------------------------------------------------------------------------

#endif
