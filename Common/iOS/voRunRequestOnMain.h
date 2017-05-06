/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voRunRequestOnMain
 
 Contains:    VisualOn request to run on main thread header file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-19   Jeff            Create file
 *******************************************************************************/

#ifndef __VO_NSTHREAD_HELP_H__
#define __VO_NSTHREAD_HELP_H__

// since objective-c can't place in namespace, set this struct out of namespace
typedef void (* VO_MAIN_THREAD_LISTENER) (void* pUserData, int nID, void *pParam1, void *pParam2);
typedef struct
{
    VO_MAIN_THREAD_LISTENER   pListener;
    void*			          pUserData;
}voNSThreadListenerInfo;

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

// If you want to use inheritance with voRunRequestOnMain, choose private inheritance is better:
// Effective C++ 36: Differentiate between inheritance of interface and inheritance of implementation
// Effective C++ 42: Use private inheritance judiciously
class voRunRequestOnMain
{
public:
    voRunRequestOnMain();
    virtual ~voRunRequestOnMain();
    
    /**
     * Set a listener to be called when main thread is ready.
     * It will call listener function if you have set listener, so it will not call RunningRequestOnMain any more.
     * 
     * \param pInfo [in] The listener info
     */
    virtual void SetListenerInfo(voNSThreadListenerInfo *pInfo);
    
    /**
     * Post a request to run on main thread.
     * It will call listener function if you have set listener.
     * And it will call RunningRequestOnMain if you havn't set listener.
     *
     * \param bWaitUntilDone [in] Set whether block request until done
     * \param nID [in] Set your add int value (keep the memory by yourself if you set bWaitUntilDone to NO)
     * \param pParam1 [in] Set your add pointer info
     * \param pParam2 [in] Set your add pointer info
     */
    virtual void PostRunOnMainRequest(bool bWaitUntilDone, int nID, void *pParam1, void *pParam2);
    
protected:
    /**
     * Running a request on main thread.
     * It will be called if you havn't set listener
     *
     * \param nID [in] The add int value of request
     * \param pParam1 [in] The add info of request
     * \param pParam2 [in] The add info of request
     */
    virtual void RunningRequestOnMain(int nID, void *pParam1, void *pParam2);

private:
    static void OnMainThread(void* pUserData, int nID, void *pParam1, void *pParam2);

private:
    voNSThreadListenerInfo    m_cInfoCB;
    void*                     m_pObj;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __VO_NSTHREAD_HELP_H__
