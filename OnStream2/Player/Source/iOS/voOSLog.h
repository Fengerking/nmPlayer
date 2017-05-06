    /************************************************************************
    *                                                                        *
    *        VisualOn, Inc. Confidential and Proprietary, 2003 - 2011        *
    *                                                                        *
    ************************************************************************/
/*******************************************************************************
    File:        voOSLog.h

    Contains:    voOSLog header file

    Written by:  Jeff

    Change History (most recent first):
    2012-10-25        Jeff            Create file

*******************************************************************************/
#ifndef __VO_OS_LOG_H__
#define __VO_OS_LOG_H__

#include "voLog.h"
#include "voNSRecursiveLock.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class voOSLog
{
public:
    voOSLog();
    virtual ~voOSLog();
    void setPath(char *pPath);
    
    virtual VO_LOG_PRINT_CB*    getCB();
    virtual void                setUseNSLog(bool enable);
    virtual void                flush();

    virtual int         doLogPrint(int nLevel, VO_TCHAR * pLogText);
    static  int         onLogPrint(void * pUserData, int nLevel, VO_TCHAR * pLogText);
    
protected:
    VO_LOG_PRINT_CB		    m_cOnStreamVOLOGCB;
    int                     m_nDumpFlag;
    bool                    m_bOutNsLog;
    
    voNSRecursiveLock       m_cLock;
    FILE*                   m_pLogDump;
};

#endif // __VO_PLAYER_SDK_H__
