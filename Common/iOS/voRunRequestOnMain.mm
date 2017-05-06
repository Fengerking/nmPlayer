/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voRunRequestOnMain
 
 Contains:    VisualOn request to run on main thread cpp file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-19   Jeff            Create file
 *******************************************************************************/

#import <Foundation/Foundation.h>
#import "voRunRequestOnMain.h"
#import "voRunRequestOnMainObjC.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voRunRequestOnMain::voRunRequestOnMain()
{
    memset(&m_cInfoCB, 0, sizeof(m_cInfoCB));
    
    voNSThreadListenerInfo cInfo;
    memset(&cInfo, 0, sizeof(cInfo));
    cInfo.pUserData = this;
    cInfo.pListener = OnMainThread;
    
    m_pObj = [[voRunRequestOnMainObjC alloc] init];
    [(voRunRequestOnMainObjC *)m_pObj setDelegateCB:&cInfo];
}

voRunRequestOnMain::~voRunRequestOnMain()
{
    voNSThreadListenerInfo cInfo;
    memset(&cInfo, 0, sizeof(cInfo));
    [(voRunRequestOnMainObjC *)m_pObj setDelegateCB:&cInfo];
    [(voRunRequestOnMainObjC *)m_pObj release];
}

void voRunRequestOnMain::SetListenerInfo(voNSThreadListenerInfo *pInfo)
{
    if (NULL == pInfo) {
        return;
    }
    memcpy(&m_cInfoCB, pInfo, sizeof(m_cInfoCB));
}

void voRunRequestOnMain::PostRunOnMainRequest(bool bWaitUntilDone, int nID, void *pParam1, void *pParam2)
{
    [(voRunRequestOnMainObjC *)m_pObj postRunOnMainRequest:bWaitUntilDone nID:nID pParam1:pParam1 pParam2:pParam2];
}

void voRunRequestOnMain::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    
}

void voRunRequestOnMain::OnMainThread(void* pUserData, int nID, void *pParam1, void *pParam2)
{
    voRunRequestOnMain *pThis = (voRunRequestOnMain *)pUserData;
    
    if (NULL == pThis) {
        return;
    }
    
    if (NULL != (pThis->m_cInfoCB).pListener) {
        (pThis->m_cInfoCB).pListener((pThis->m_cInfoCB).pUserData, nID, pParam1, pParam2);
    }
    else {
        pThis->RunningRequestOnMain(nID, pParam1, pParam2);
    }
}
