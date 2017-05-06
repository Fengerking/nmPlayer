#ifndef __IOERROR_H__
#define __IOERROR_H__

#include "voSource2.h"
#include "voSource2_IO.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define IO_FAIL_TOLERATE_COUNTS 10
#define IO_CONTINUE_TOLERATE_COUNTS 3
//#define IOERROR_FAIL_TOLERATE_TIME 1000 * 60 * 2
//This data should not be larger than 4, cause vmax limitation that one chunk return error
//may take 30s.Customers think 30x10s is too long, now reduce it to 30x4
#define DRM_FAIL_TOLERATE_COUNTS 4
#define INIT_DATA_TOLERATE_COUNTS 10
#define FILEUNSUPPORT_FAIL_TOLERATE_COUNTS 10

typedef struct
{
	VO_U32 nFailTolerateNumbers;
	VO_U32 nContinueTolerateNumbers;
	VO_U32 nStartTime;   //
	VO_U32 nLastErrCode; //defined in voSource2_IO.h
}TOLERATE_ST;

class CommonError
{
public:
	CommonError();
	virtual ~CommonError();

	/*param *pRetryIntervalTime [out]*/
	virtual VO_U32 CheckError(VO_U32 nType, VO_U32 nParam2, VO_U32 *pRetryIntervalTime);
	virtual VO_U32 SetParameter( VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetRetryIntervalTime( VO_U32 nErrCode);
	virtual VO_VOID Reset();
protected:
	virtual VO_U32 ForPlaylist(VO_U32 *pRetryIntervalTime, VO_BOOL isError );
	virtual VO_U32 ForChunk( VO_U32 nParam2, VO_U32 *pRetryIntervalTime, VO_BOOL isError );
	virtual VO_U32 ForDrm(VO_U32 nParam2, VO_U32 *pRetryIntervalTime, VO_BOOL isError);
	virtual VO_U32 ForFileFormat(VO_U32 nParam2, VO_U32 *pRetryIntervalTime , VO_BOOL isError);
	
	virtual VO_U32 ToCheckError(TOLERATE_ST *pTolerate, VO_BOOL isError, VO_U32 nErrCode);
	VO_U32 CheckLiveError(TOLERATE_ST *pTolerate, VO_U32 nErrCode);
	VO_U32 CheckVodError(TOLERATE_ST *pTolerate, VO_U32 nErrCode);


	TOLERATE_ST m_stAudio;
	TOLERATE_ST m_stVideo;
	TOLERATE_ST m_stSubtitle;
	TOLERATE_ST m_stPlaylist;

	VO_U32	m_uDrmErrorCount;
	VO_U32   m_uUnSupportCount;
	VO_U32	m_uInitDataFailCount;

	VO_U32   m_uRetryTimeout;
	VO_SOURCE2_PROGRAM_TYPE m_sProgramType ;
};


#ifdef _VONAMESPACE
}
#endif

#endif
