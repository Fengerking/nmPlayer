#ifndef __ERRORHANDLER_H__
#define __ERRORHANDLER_H__

#include "voSource2.h"
#include "voSource2_IO.h"
#include "CommonError.h"
#include "voAdaptiveStreamParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CEventHandler
{
public:
	CEventHandler(VO_SOURCE2_EVENTCALLBACK * pEventCB, VO_ADAPTIVESTREAMPARSER_STREAMTYPE nType, VO_PTR pReserved);
	virtual ~CEventHandler();

	virtual VO_U32 CheckEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2, VO_PTR pReserved);
	virtual VO_U32 GetParameter( VO_U32 uID, VO_PTR pParam );
	virtual VO_U32 SetParameter( VO_U32 uID, VO_PTR pParam );
	virtual VO_VOID SetStop( VO_BOOL bStop) { m_bStop = bStop; }

protected:
	VO_U32 CheckWarningEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime);
	VO_U32 CheckInfoEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime);
	VO_U32 CheckErrorEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime);
	VO_U32 CheckCustomEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime);
	VO_U32 CheckRetry( VO_U32 ret, VO_U32 nRetryIntervalTime);

	VO_SOURCE2_EVENTCALLBACK *m_pEventCB;

	CommonError m_CommonError;

	VO_ADAPTIVESTREAMPARSER_STREAMTYPE m_nStreamType;

	VO_BOOL m_bStop;
	VO_SOURCE2_PROGRAM_TYPE m_sProgramType;
	VO_BOOL	m_bSusPend;
	VO_U32	m_nTolerantErrorNum;
	VO_U32	m_nStreamCount;
	VO_U32	m_nErrorNum;
};


#ifdef _VONAMESPACE
}
#endif

#endif
