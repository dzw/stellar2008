/*----------------------------------------------------------------------*/
// 档名: cBaseReader.h
// 说明:
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
	// 错误码
	enum {
		errFileReady,   // 档案未备妥
		errParsing,     // 解析档案错误
		errFileSize,    // 档案大小错误
		errFileOpen,    // 开档错误
		errAlloc,       // 记忆体配置错误
	};
	//------------------------------------------
	// TOKEN TYPE
	// GetToken() 会回传的类型
	// 在符号的处理方面,
	// '.' dot 因为是数字的小数点, 不当符号做处理
	// '"' dblquote 是字串资料的区隔, 不当符号做处理
	// '_' underline 可以当成识别字的一部份, 不当符号做处理
	//------------------------------------------
	enum {
		ttIGNORE = -2,   // 忽略的型别
		ttERROR,         // 解析途中发生错误
		ttUNKNOWN,       // 未知
		ttFILEEND,       // 档案结尾 '\0'
		ttBRACEOPEN,     // { - 左大括号
		ttBRACECLOSE,    // } - 右大括号
		ttPARENOPEN,     // ( - 左小括号
		ttPARENCLOSE,    // ) - 右小括号
		ttBRACKETOPEN,   // [ - 左中括号
		ttBRACKETCLOSE,  // ] - 右中括号
		ttEQUALMARK,     // = - 等号    (equal mark
		ttCOMMA,         // , - 逗号    (comma
		ttDOT,           // . - 小数点  (dot
		ttEXCLA,         // ! - 惊叹号  (exclamation mark
		ttPLUS,          // + - 加号  (plus
		ttMINUS,         // - - 减号  (minus
		ttULINE,         // _ - 底线    (underline
		ttSTAR,          // * - 星号    (star
		ttSLASH,         // / - 斜线    (slash
		ttPERCENT,       // % - 百分比  (percent
		ttCOLON,         // : - 冒号    (colon
		ttSEMICOLON,     // ; - 分号    (semicolon
		ttHAT,           // ^ - 指数?   (hat
		ttBELOW,         // < - 小于    (below than
		ttGREATER,       // > - 大于    (greater than
		ttQUERY,         // ? - 问号    (query
		ttAT,            // @ - 小老鼠  (at
		ttAND,           // & - 和      (and
		ttBAR,           // | - 或      (bar
		ttSHARP,         // # - 井字    (sharp
		ttBACKSLASH,     // \\ - 反斜线 (backslash
		ttQUOTE,         // \' - 单引号 (quote
		ttDQUOTE,        // \" - 双引号 (dquote
		ttRETURN,        // '\r'
		ttNEXTLINE,      // '\n'
		ttTAB,           // '\t'
		ttBELL,          // '\a'
		ttBACKSPACE,     // '\b'
		ttVERTTAB,       // '\v'
		ttSPACE,         // ' '
		//
		ttNUMBER,        // 单位元数字
		ttALPHA,         // 单位元文字
		ttMULTIBYTE,     // 双位元文字
		//
		ttBASEMAX        // 衍生类别的tokenType要从这里开始宣告起, 不然会错乱.
	};

protected:
	static char m_pchDelimiter[]; // 符号
	static int  m_aDelimiter[];   // 符号对应码

	char  m_szPicked[3];          // Pick的字元, 因为最多只有到双字元,
	// 加上'\0'结尾字元, 共三个

	int   m_iFileSize;            // 档案的大小
	char* m_pFileData;            // 储存档案资料的位置
	char* m_pNextChar;            // 目前分析的位置
	char* m_pBackupPtr;           // 备份的指标
	bool  m_bAlready;             // 确认读取完毕

	int   m_iPickedType;          // Token的类型
	//- functions
	int  PickChar(char* &pNextChar, bool bPick = true);  // 回传 TokenType
	char* GetNextChar()
	{
		if (m_bAlready == false) return NULL;
		return m_pNextChar;
	}  // 回会回传 TokenType, 不会写入m_szToken
	// 取得一个字节
	char* GetPicked()  { return m_szPicked; }
	// 错误讯息处理.
	virtual void CallErrorMsg(int no); // 错误讯息
	int  GetStringLength(char* szString)
	{
		if (szString == NULL) return 0;
		if (*szString == '\0') return 0;

		return strlen(szString);
	}
public:
	cBaseReader();
	virtual ~cBaseReader();

	// 载入
	bool LoadFromFile(char* fileName);          // 档案读入记忆体
	bool LoadFromMemory(char* data, int size);
	bool LoadFromMemoryX(char* szStringL, char* szContext, char* szStringR);
	// 重设
	bool Reset();
	bool IsAlready() { return m_bAlready; }
	bool IsWhiteSpace(int tokenType);
	// 小心使用... 
	void BackupPtr(); // 备份目前的 m_pNextChar
	void ReloadPtr(); // 重载备份的 m_pNextChar
};
//---------------------------------------------------------------------------

#endif
