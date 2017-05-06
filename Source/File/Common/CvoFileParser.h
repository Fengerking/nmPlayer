#pragma once
#include "voYYDef_filcmn.h"
#include "CvoBaseObject.h"
#include "CvoBaseMemOpr.h"
#include "CGFileChunk.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CvoFileBaseParser :
	public CvoBaseObject, 
	public CvoBaseMemOpr
{
public:
	CvoFileBaseParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CvoFileBaseParser();

public:
	virtual VO_U64			GetCurrParseFilePos()	{return m_pFileChunk->FGetFilePos();}

protected:
	CGFileChunk*		m_pFileChunk;
};

class CvoFileHeaderParser : 
	public CvoFileBaseParser
{
public:
	CvoFileHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CvoFileHeaderParser();

public:
	//parse file header to get information
	virtual VO_BOOL			ReadFromFile() = 0;
};

//callback return value macros
//0xabcdefgh
//h - continuable flag
//g - need skip flag
#define CBRT_FALSE							0x0
#define CBRT_CONTINUABLENOTNEEDSKIP			0x1
#define CBRT_CONTINUABLEANDNEEDSKIP			0x3
#define CBRT_IS_CONTINUABLE(res)			(res & 0x1)
#define CBRT_IS_NEEDSKIP(res)				(res & 0x2)
//return CBRT_XXX
typedef VO_U8	(*ONDATACALLBACK)	(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam);
//return VO_FALSE mean Block Stream End!!
typedef VO_BOOL	(*ONBLOCKCALLBACK)	(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam);
class CvoFileDataParser : 
	public CvoFileBaseParser
{
public:
	CvoFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CvoFileDataParser();

public:
	virtual VO_VOID			Init(VO_PTR pParam) = 0;

	virtual VO_BOOL			SetStartFilePos(VO_U64 ullStartFilePos, VO_BOOL bInclHeader = VO_FALSE);

	VO_VOID					SetOnDataCallback(VO_PTR pUser, VO_PTR pUserData, ONDATACALLBACK fCallback);
	VO_VOID					SetOnBlockCallback(VO_PTR pUser, VO_PTR pUserData, ONBLOCKCALLBACK fCallback);

	virtual VO_VOID			SetBlockStream(VO_U8 btStreamNum) {m_btBlockStream = btStreamNum;}
	virtual VO_VOID			SetParseEnd(VO_BOOL bEnd) {m_bParseEnd = bEnd;}
	virtual VO_BOOL			IsParseEnd() {return m_bParseEnd;}

	virtual VO_BOOL			Step();

protected:
	virtual VO_BOOL			StepB() = 0;

protected:
	VO_U8					m_btBlockStream;
	VO_BOOL					m_bParseEnd;

	ONDATACALLBACK			m_fODCallback;
	VO_PTR					m_pODUser;
	VO_PTR					m_pODUserData;

	ONBLOCKCALLBACK			m_fOBCallback;
	VO_PTR					m_pOBUser;
	VO_PTR					m_pOBUserData;
};

#ifdef _VONAMESPACE
}
#endif
