    /************************************************************************
    *                                                                        *
    *        VisualOn, Inc. Confidential and Proprietary, 2003                *
    *                                                                        *
    ************************************************************************/
/*******************************************************************************
    File:        voOSLog.cpp

    Contains:    voOSLog cpp file

    Written by:  Jeff

    Change History (most recent first):
    2012-10-25        Jeff            Create file

*******************************************************************************/

//#define DUMP_LOG_NS

#import "voOSLog.h"
#import <Foundation/Foundation.h>
#import "voOSFunc.h"
#import "CBaseConfig.h"
#import "voNSRecursiveLock.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voOSLog::voOSLog()
:m_bOutNsLog(false)
,m_pLogDump(NULL)
,m_nDumpFlag(0)
{
    memset(&m_cOnStreamVOLOGCB, 0, sizeof(m_cOnStreamVOLOGCB));
    
    m_cOnStreamVOLOGCB.pUserData = this;
    m_cOnStreamVOLOGCB.fCallBack = onLogPrint;
    
    // Always enable for log
    //if (voOS_EnableDebugMode(0))
	{
#ifdef _VOLOG_INFO
        VO_TCHAR	szCfgFile[1024];
        memset(szCfgFile, 0, sizeof(szCfgFile));
        voOS_GetAppFolder (szCfgFile, 1024);
        vostrcat (szCfgFile, _T("voDebugFolder/"));
        setPath(szCfgFile);
        
        if (NULL == m_pLogDump) {
            memset(szCfgFile, 0, sizeof(szCfgFile));
            voOS_GetAppFolder (szCfgFile, 1024);
            setPath(szCfgFile);
        }
#endif
    }
}

voOSLog::~voOSLog()
{
    if (NULL != m_pLogDump) {
        fclose(m_pLogDump);
        m_pLogDump = NULL;
    }
}

void voOSLog::setUseNSLog(bool enable)
{
    m_bOutNsLog = enable;
}

void voOSLog::flush()
{
    if (NULL != m_pLogDump) {
        voNSAutoLock cAuto(&m_cLock);
        fflush(m_pLogDump);
    }
}

void voOSLog::setPath(char *pPath)
{
    if (NULL == pPath) {
        return;
    }
    
    voNSAutoLock cAuto(&m_cLock);
    
    if (NULL == m_pLogDump) {
        VO_TCHAR	szCfgFile[1024];
        memset(szCfgFile, 0, sizeof(szCfgFile));
        strncpy(szCfgFile, pPath, 1024 - strlen("/voLog.cfg"));
        vostrcat (szCfgFile, _T("/voLog.cfg"));
        
        CBaseConfig m_cfgSource;
        
        if (!m_cfgSource.Open (szCfgFile))
        {
            return;
        }
        
        m_nDumpFlag = m_cfgSource.GetItemValue("OSMP_MP", "LogDumpFile", 0);
        if (0 != m_nDumpFlag) {
            char szTmp[1024];
            strncpy(szTmp, pPath, 1024 - strlen("/voLog.log"));
            strcat(szTmp, "/voLog.log");
            m_pLogDump = fopen(szTmp, "wb");
            
            if (NULL == m_pLogDump) {
                VOLOGI(" ------------------Open %s file error!\n", szTmp);
            }
        }
        
        int nLogIsNsLog = m_cfgSource.GetItemValue("OSMP_MP", "LogUseNS", 0);
        if (1 == nLogIsNsLog) {
            m_bOutNsLog = true;
        }
    }
}

VO_LOG_PRINT_CB* voOSLog::getCB()
{
    return &m_cOnStreamVOLOGCB;
}

int	voOSLog::onLogPrint(void * pUserData, int nLevel, VO_TCHAR * pLogText)
{
    voOSLog *pThis = (voOSLog *)pUserData;
    if ((NULL != pThis) && (NULL !=pLogText)) {
        return pThis->doLogPrint(nLevel, pLogText);
    }
    
    return -1;
}

int	voOSLog::doLogPrint(int nLevel, VO_TCHAR * pLogText)
{
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    if (NULL != m_pLogDump) {
        
        voNSAutoLock cAuto(&m_cLock);
        
        if (NULL != pLogText)
        {
            char szLog[1024] = {0};
            snprintf(szLog, 1024, "Time:%ld  %s", voOS_GetSysTime(), pLogText);
            
            fwrite(szLog, strlen(szLog), 1, m_pLogDump);
            
            if (2 == m_nDumpFlag) {
                fflush(m_pLogDump);
            }
        }
    }
    
    if (m_bOutNsLog) {
        NSLog(@"%s", pLogText);
    }
    else {
        printf("Time:%ld  %s", voOS_GetSysTime(), pLogText);
    }
    
    [pool release];
    
    return 0;
}

