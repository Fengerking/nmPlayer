#ifndef _CCCOMANDPARSER_H_
#define _CCCOMANDPARSER_H_
#include "voType.h"
#include "CvoBaseMemOpr.h"


//Special Character Symbol Code  for CEA_608

const VO_BYTE SPECIAL_CHARACTER_AND_PREAMBLE_ADDRESS_ROW_1_2_SYMBOL = 0x11;

//Special Character Set Table for CEA_608
const VO_BYTE SPECIAL_CHARACTER_MID_ROW_CONTROL_START = 0x20;
const VO_BYTE SPECIAL_CHARACTER_MID_ROW_CONTROL_END = 0x2F;

const VO_BYTE SPECIAL_CHARACTER_SPECIAL_CODE_START = 0x30;
const VO_BYTE SPECIAL_CHARACTER_SPECIAL_CODE_END = 0x3F;

const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_START_1 = 0x40;
const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_END_1 = 0x5F;

const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_START_2 = 0x60;
const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_2_END_2 = 0x7F;

//Preamble Character Symbol Code for CEA_608
const VO_BYTE PREAMBLE_ADDRESS_ROW_3_4_SYMBOL = 0x12;
const VO_BYTE PREAMBLE_ADDRESS_ROW_5_6_SYMBOL = 0x15;
const VO_BYTE PREAMBLE_ADDRESS_ROW_7_8_SYMBOL = 0x16;
const VO_BYTE PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL = 0x17;
const VO_BYTE PREAMBLE_ADDRESS_ROW_11_SYMBOL = 0x10;
const VO_BYTE PREAMBLE_ADDRESS_ROW_12_13_SYMBOL = 0x13;
const VO_BYTE PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL = 0x14;

//Control Character Set Table for CEA_608
const VO_BYTE CONTROL_CHARACTER_START = 0x20;
const VO_BYTE CONTROL_CHARACTER_END = 0x2F;



//Standard Character Set Table for CEA_608
const VO_BYTE STANDARD_CHARACTER_START = 0x20;
const VO_BYTE STANDARD_CHARACTER_END = 0x7F;

typedef enum
{
	command_rollUp = 0,
	command_Carriage_return,
	command_indent,
	command_none
}comand_type;

#define MAX_LINE_COUNT 4
#define MAX_CHARACTER_COUNT_IN_ONE_LINE 40

typedef struct 
{
	VO_BYTE LineBuffer[MAX_CHARACTER_COUNT_IN_ONE_LINE];
	VO_U8 CharCnt;

}LineCharInfo;

class CCComandParser :public CvoBaseMemOpr
{
public:
	CCComandParser();
	virtual ~CCComandParser();
	virtual VO_BOOL ParserCommandCode(VO_PBYTE pInputData, VO_U32 cbInputData,VO_PBYTE pOutputData, VO_U32 *cbOutputData){return VO_FALSE;}
	virtual VO_U8 GetLineCount(){return m_nLineNumber;}
	virtual LineCharInfo& GetLineInfoByLineNo(VO_U8 nLineNo){return m_LineInfo[nLineNo];}
	virtual VO_VOID ResetLineNo(){m_nLineNumber--;}
protected:
//	VO_U32 m_nCharCount;
//	VO_BYTE * m_CharBuffer;
	comand_type m_eCharType;
	LineCharInfo m_LineInfo[MAX_LINE_COUNT];
	VO_U8 m_nLineNumber;
private:
	
	
};


class CC608ComandParser :public CCComandParser
{
public:
	CC608ComandParser();
	virtual ~CC608ComandParser();
	VO_BOOL ParserCommandCode(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL ParserSpecialChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL ParserStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL	ParserMIDROWChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL ParserControlChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL ParserPreambleChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL ParserNonStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData);
protected:
private:
	
//	VO_U8 m_nCharCountInLine;
};

#endif //_CCCOMANDPARSER_H_