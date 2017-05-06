
#ifndef _CSMTHPARSER_H_
#define  _CSMTHPARSER_H_

#include "voParser.h"
#include "voSmthBase.h"
#include "CSMTHParserCtrl.h"
#include "voLog.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//static voCMutex			m_AutoLock;
class CSMTH_Parser//	:	public CBaseParser
{
public:
	CSMTH_Parser();
	virtual ~CSMTH_Parser(void);
	
	CSMTHParserCtrl *m_pSmthParserCtrl;
public:
	virtual VO_U32 Open(VO_PARSER_INIT_INFO* pParam);
	virtual	VO_U32 Close();
	virtual	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	virtual VO_U32 SelectProgram(VO_U32 uStreamID);
	virtual VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_BOOL GetLicState();	
	
protected:



private:
	//static void RecvParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf)
	static void OnParsed(VO_PARSER_OUTPUT_BUFFER* pBuf);
	void HandleParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf);

private:
	//ITsParseController* m_pTsCtrl;
	VO_U64				m_nFirstTimeStamp;
	VO_U32				program_time;
	VO_PTR              m_pHLicCheck;	
	
};
#ifdef _VONAMESPACE
}
#endif

#endif