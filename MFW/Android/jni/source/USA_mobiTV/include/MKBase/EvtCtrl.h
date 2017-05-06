#ifndef __MKBase_EvtCtrl_h__
#define __MKBase_EvtCtrl_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>
#include <MKBase/Result.h>
#include <MKBase/Sync.h>

#include <MKBase/File.h>
#include <MKBase/Socket.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* EvtCtrl INTERFACE                                                          */
/******************************************************************************/

typedef struct _MK_EvtCtrl MK_EvtCtrl;

#define MK_EVTCTRL_INITVAL _MK_EVTCTRL_INITVAL

/******************************************************************************/

/*
 * Callback used to report errors returned from event handler functions.
 *
 * NOTE: This is a very simplistic error reporting function and may need to be
 *       refactored into something a bit more flexible in the future.
 */
typedef void (*MK_EvtErrFunc)(void* aObj, MK_Result aErr);

/******************************************************************************/

/*
 * Return an initialized MK_EvtCtrl structure.
 */
#if !(MK_WIN32 && __cplusplus)
    /* NOTE: Avoid VS bug when compiling as C++ */
    MK_INLINE MK_EvtCtrl MK_EvtCtrl_InitVal(void);
#endif

/*
 * Initialize aCtrl structure.
 */
MK_INLINE void MK_EvtCtrl_Init(MK_EvtCtrl* aCtrl);

/*
 * Free any resources owned by aCtrl structure and reinitialize it (alias for
 * MK_EvtCtrl_Close function). Triggers a MK_E_INTR error from any pending
 * MK_EvtCtrl_Wait() calls.
 */
MK_INLINE void MK_EvtCtrl_Destroy(MK_EvtCtrl* aCtrl);

/******************************************************************************/

/*
 * Create an EvtCtrl instance and store it in aCtrl.
 *
 *   MK_S_OK              - Success
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_EvtCtrl_Open(MK_EvtCtrl* aCtrl);

/*
 * Free any resources owned by aCtrl structure and reinitialize it (interrupts
 * any currently running MK_EvtCtrl_Wait() call).
 */
void MK_EvtCtrl_Close(MK_EvtCtrl* aCtrl);

/******************************************************************************/

/*
 * Set the error callback function. The aObj pointer is a user provided value
 * which will be available in the callback to aFunc.
 */
void MK_EvtCtrl_SetErrEvtFunc(MK_EvtCtrl* aCtrl, MK_EvtErrFunc aFunc, void* aObj);

/******************************************************************************/

/*
 * Wait a maximum of aMSec (0 for Poll) milliseconds for one or more event
 * registered with aCtrl to occur. Returns a value > 0 if events were processed,
 * 0 if the timeout expired and < 0 if an error occured:
 *
 *   MK_E_AGAIN           - Instance is in use by another thread
 *   MK_E_IARG            - Instance is not opened (or was closed)
 *   MK_E_INTR            - Wait was interrupted
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 *
 * NOTE: The precision of the timeout may wary between platforms and should not
 *       be relied on to have any given precision.
 */
MK_S32 MK_EvtCtrl_Wait(MK_EvtCtrl* aCtrl, MK_U32 aMSec);
MK_INLINE MK_S32 MK_EvtCtrl_Poll(MK_EvtCtrl* aCtrl);

/*
 * Interrupt a running or pending call to MK_EvtCtrl_Wait() (e.g. will cause the
 * currently running or the next call to Wait() to return MK_E_INTR).
 */
void MK_EvtCtrl_Interrupt(MK_EvtCtrl* aCtrl);

/******************************************************************************/
/* TmrEvt INTERFACE                                                           */
/******************************************************************************/

typedef struct _MK_TmrEvt MK_TmrEvt;

typedef MK_Result (*MK_TmrEvtFunc)(MK_TmrEvt* aEvt, MK_S32 aCnt);

#define MK_TMREVT_INITVAL _MK_TMREVT_INITVAL

/******************************************************************************/

/*
 * Return an initialized MK_TmrEvt structure.
 */
#if !(MK_WIN32 && __cplusplus)
    /* NOTE: Avoid VS bug when compiling as C++ */
    MK_INLINE MK_TmrEvt MK_TmrEvt_InitVal(void);
#endif

/*
 * Initialize aEvt structure.
 */
MK_INLINE void MK_TmrEvt_Init(MK_TmrEvt* aEvt);

/*
 * Free any resources owned by aEvt structure and reinitialize it (alias for
 * MK_TmrEvt_Close function).
 */
MK_INLINE void MK_TmrEvt_Destroy(MK_TmrEvt* aEvt);

/******************************************************************************/

/*
 * Create a TmrEvt (periodic timer) instance, register it with aCtrl, and store
 * it in aEvt. If aOn is TRUE aFunc will be called if/when one or more intervals
 * of aMSec have passed in a call to Wait/Poll for aCtrl (and aObj will be
 * accessible as user-defined data).
 *
 * Preconditions:
 *
 *   aEvt != NULL && aCtrl != NULL && aMSec > 0 && aFunc != NULL
 *
 * Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_BUSY            - Event in use (e.g. opened or possibly uninitialized)
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_TmrEvt_Open(MK_TmrEvt* aEvt, MK_EvtCtrl* aCtrl, MK_U32 aMSec, MK_TmrEvtFunc aFunc, void* aObj, MK_Bool aOn);

/*
 * Free any resources owned by aEvt structure and reinitialize it. Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_TmrEvt_Close(MK_TmrEvt* aEvt);

/******************************************************************************/

/*
 * Update an already open TmrEvt instance with new values.
 *
 * TODO: Complete documentation.
 */
MK_Result MK_TmrEvt_Update(MK_TmrEvt* aEvt, MK_U32 aMSec, MK_TmrEvtFunc aFunc, void* aObj, MK_Bool aOn);

/******************************************************************************/

/*
 * Toggle the on/off state of aEvt. Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_TmrEvt_Toggle(MK_TmrEvt* aEvt, MK_Bool aOn);

/******************************************************************************/

/*
 * Return the callback registered for aEvt.
 */
MK_INLINE MK_TmrEvtFunc MK_TmrEvt_GetFunc(MK_TmrEvt* aEvt);

/*
 * Return the user-defined object for aEvt.
 */
MK_INLINE void* MK_TmrEvt_GetObj(MK_TmrEvt* aEvt);

/*
 * Return the EvtCtrl instance in which aEvt is registered.
 */
MK_INLINE MK_EvtCtrl* MK_TmrEvt_GetCtrl(MK_TmrEvt* aEvt);

/*
 * Return the timer interval of aEvt.
 */
MK_INLINE MK_U32 MK_TmrEvt_GetMSec(MK_TmrEvt* aEvt);

/******************************************************************************/
/* FileEvt INTERFACE                                                          */
/******************************************************************************/

typedef struct _MK_FileEvt MK_FileEvt;

typedef MK_Result (*MK_FileEvtFunc)(MK_FileEvt* aEvt, MK_S32 aCnt);

#define MK_FILEEVT_INITVAL _MK_FILEEVT_INITVAL

/******************************************************************************/

/*
 * Return an initialized MK_FileEvt structure.
 */
#if !(MK_WIN32 && __cplusplus)
    /* NOTE: Avoid VS bug when compiling as C++ */
    MK_INLINE MK_FileEvt MK_FileEvt_InitVal(void);
#endif

/*
 * Initialize aEvt structure.
 */
MK_INLINE void MK_FileEvt_Init(MK_FileEvt* aEvt);

/*
 * Free any resources owned by aEvt structure and reinitialize it (alias for
 * MK_FileEvt_Close function).
 */
MK_INLINE void MK_FileEvt_Destroy(MK_FileEvt* aEvt);

/******************************************************************************/

/*
 * Create an FileEvt (I/O events) instance, register it with aCtrl, and store
 * it in aEvt. If aIOn is TRUE aIFunc will be called if aFile is readable during
 * a call to Wait/Poll for aCtrl (and aIObj will be accesible as user-defined
 * data). If aOOn is TRUE aOFunc will be called if aFile is writable during a
 * call to Wait/Poll for aCtrl (and aOObj will be accesible as user-defined
 * data).
 *
 * Preconditions:
 *
 *   aEvt != NULL && aCtrl != NULL && aFile != NULL &&
 *   (aIFunc != NULL || aOFunc != NULL)
 *
 * Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_BUSY            - Event in use (e.g. opened or possibly uninitialized)
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_FileEvt_Open(MK_FileEvt* aEvt, MK_EvtCtrl* aCtrl, MK_File* aFile, MK_FileEvtFunc aIFunc, void* aIObj, MK_Bool aIOn);

/*
 * Free any resources owned by aEvt structure and reinitialize it. Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_FileEvt_Close(MK_FileEvt* aEvt);

/******************************************************************************/

/*
 * Toggle the on/off state of I/O events for aEvt (only if I/O func != NULL).
 * Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_FileEvt_ToggleI(MK_FileEvt* aEvt, MK_Bool aOn);

/******************************************************************************/

/*
 * Return the input callback registered for aEvt.
 */
MK_INLINE MK_FileEvtFunc MK_FileEvt_GetIFunc(MK_FileEvt* aEvt);

/*
 * Return the input user-defined object for aEvt.
 */
MK_INLINE void* MK_FileEvt_GetIObj(MK_FileEvt* aEvt);

/*
 * Return the EvtCtrl instance in which aEvt is registered.
 */
MK_INLINE MK_EvtCtrl* MK_FileEvt_GetCtrl(MK_FileEvt* aEvt);

/*
 * Return the file instance which aEvt monitors.
 */
MK_INLINE MK_File* MK_FileEvt_GetFile(MK_FileEvt* aEvt);

/******************************************************************************/
/* SockEvt INTERFACE                                                          */
/******************************************************************************/

typedef struct _MK_SockEvt MK_SockEvt;

typedef MK_Result (*MK_SockEvtFunc)(MK_SockEvt* aEvt, MK_S32 aCnt);

#define MK_SOCKEVT_INITVAL _MK_SOCKEVT_INITVAL

/******************************************************************************/

/*
 * Return an initialized MK_SockEvt structure.
 */
#if !(MK_WIN32 && __cplusplus)
    /* NOTE: Avoid VS bug when compiling as C++ */
    MK_INLINE MK_SockEvt MK_SockEvt_InitVal(void);
#endif

/*
 * Initialize aEvt structure.
 */
MK_INLINE void MK_SockEvt_Init(MK_SockEvt* aEvt);

/*
 * Free any resources owned by aEvt structure and reinitialize it (alias for
 * MK_SockEvt_Close function).
 */
MK_INLINE void MK_SockEvt_Destroy(MK_SockEvt* aEvt);

/******************************************************************************/

/*
 * Create an SockEvt (I/O events) instance, register it with aCtrl, and store
 * it in aEvt. If aIOn is TRUE aIFunc will be called if aSock is readable during
 * a call to Wait/Poll for aCtrl (and aIObj will be accesible as user-defined
 * data). If aOOn is TRUE aOFunc will be called if aSock is writable during a
 * call to Wait/Poll for aCtrl (and aOObj will be accesible as user-defined
 * data).
 *
 * Preconditions:
 *
 *   aEvt != NULL && aCtrl != NULL && aSock != NULL &&
 *   (aIFunc != NULL || aOFunc != NULL)
 *
 * Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_BUSY            - Event in use (e.g. opened or possibly uninitialized)
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_SockEvt_Open(MK_SockEvt* aEvt, MK_EvtCtrl* aCtrl, MK_Sock* aSock, MK_SockEvtFunc aIFunc, void* aIObj, MK_Bool aIOn, MK_SockEvtFunc aOFunc, void* aOObj, MK_Bool aOOn);

/*
 * Free any resources owned by aEvt structure and reinitialize it. Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_SockEvt_Close(MK_SockEvt* aEvt);

/******************************************************************************/

/*
 * Toggle the on/off state of I/O events for aEvt (only if I/O func != NULL).
 * Returns:
 *
 *   MK_S_OK              - Success
 *   MK_E_EVTCTRL_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_SockEvt_ToggleI(MK_SockEvt* aEvt, MK_Bool aOn);
MK_Result MK_SockEvt_ToggleO(MK_SockEvt* aEvt, MK_Bool aOn);

/******************************************************************************/

/*
 * Return the input callback registered for aEvt.
 */
MK_INLINE MK_SockEvtFunc MK_SockEvt_GetIFunc(MK_SockEvt* aEvt);

/*
 * Return the input user-defined object for aEvt.
 */
MK_INLINE void* MK_SockEvt_GetIObj(MK_SockEvt* aEvt);

/*
 * Return the output callback registered for aEvt.
 */
MK_INLINE MK_SockEvtFunc MK_SockEvt_GetOFunc(MK_SockEvt* aEvt);

/*
 * Return the output user-defined object for aEvt.
 */
MK_INLINE void* MK_SockEvt_GetOObj(MK_SockEvt* aEvt);

/*
 * Return the EvtCtrl instance in which aEvt is registered.
 */
MK_INLINE MK_EvtCtrl* MK_SockEvt_GetCtrl(MK_SockEvt* aEvt);

/*
 * Return the socket instance which aEvt monitors.
 */
MK_INLINE MK_Sock* MK_SockEvt_GetSock(MK_SockEvt* aEvt);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#if !MK_HAVE_KQUEUE && !MK_HAVE_EPOLL && !MK_WIN32
    /* TODO: Implement a generic poll-based version */
    #error MK_EvtCtrl API currently unsupported for current platform/configuration
#endif

struct _MK_EvtCtrl
{
    #if !MK_HAVE_KQUEUE
        #define _MK_TMRSET_MAX 16
        MK_PInt TLk;
        MK_S32 TSz;
        MK_TmrEvt* TSet[_MK_TMRSET_MAX];
    #endif

    #define _MK_FILESET_MAX 8
    MK_PInt FLk;
    MK_S32 FSz;
    MK_FileEvt* FSet[_MK_FILESET_MAX];

    #if !MK_HAVE_KQUEUE && !MK_HAVE_EPOLL
        #define _MK_SOCKSET_MAX 16
        MK_PInt SLk;
        MK_S32 SSz;
        MK_SockEvt* SSet[_MK_SOCKSET_MAX];
    #endif

    MK_PInt WLk;
    #if MK_HAVE_KQUEUE || MK_HAVE_EPOLL
        MK_S32 WCtl;
        MK_S32 WSig[2];
    #elif MK_WIN32
        WSAEVENT WSig;
        WSAEVENT WSet[_MK_SOCKSET_MAX];
    #endif
    MK_PInt WILk;
    MK_EvtErrFunc WEFunc;
    void* WEObj;
};

#if MK_HAVE_KQUEUE
    #define _MK_EVTCTRL_INITVAL {MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, -1, {-1, -1}, MK_SYNC_OWNID_INV, NULL, NULL}
#elif MK_HAVE_EPOLL
    #define _MK_EVTCTRL_INITVAL {MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, -1, {-1, -1}, MK_SYNC_OWNID_INV, NULL, NULL}
#elif MK_WIN32
    #define _MK_EVTCTRL_INITVAL {MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, WSA_INVALID_EVENT, {WSA_INVALID_EVENT}, MK_SYNC_OWNID_INV, NULL, NULL}
#else
    #define _MK_EVTCTRL_INITVAL {MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, -1, {NULL}, MK_SYNC_OWNID_INV, NULL, NULL}
#endif

typedef MK_Result (*_MK_EvtFunc)(void* aEvt, MK_S32 aCnt);

typedef struct
{
    _MK_EvtFunc Func;
    void* Obj;
    MK_Bool On;
} _MK_Evt;

#define _MK_EVT_INITVAL {NULL, NULL, MK_FALSE}

/******************************************************************************/

MK_INLINE MK_EvtCtrl MK_EvtCtrl_InitVal(void)
{
    MK_EvtCtrl lCtrl = MK_EVTCTRL_INITVAL;
    return lCtrl;
}

MK_INLINE void MK_EvtCtrl_Init(MK_EvtCtrl* aCtrl)
{
    MK_ASSERT(NULL != aCtrl);
    *aCtrl = MK_EvtCtrl_InitVal();
}

MK_INLINE void MK_EvtCtrl_Destroy(MK_EvtCtrl* aCtrl)
{
    MK_EvtCtrl_Close(aCtrl);
}

/******************************************************************************/

MK_INLINE MK_S32 MK_EvtCtrl_Poll(MK_EvtCtrl* aCtrl)
{
    return MK_EvtCtrl_Wait(aCtrl, 0);
}

/******************************************************************************/

struct _MK_TmrEvt
{
    MK_PInt Lk;
    _MK_Evt Evt[1];

    MK_EvtCtrl* Ctrl;
    MK_U32 MSec;

    #if !MK_HAVE_KQUEUE
        MK_S32 Idx;
        MK_U32 Last;
    #endif
};

#if MK_HAVE_KQUEUE
    #define _MK_TMREVT_INITVAL {MK_SYNC_OWNID_INV, {_MK_EVT_INITVAL}, NULL, 0}
#else
    #define _MK_TMREVT_INITVAL {MK_SYNC_OWNID_INV, {_MK_EVT_INITVAL}, NULL, 0, -1, 0}
#endif

/******************************************************************************/

MK_INLINE MK_TmrEvt MK_TmrEvt_InitVal(void)
{
    MK_TmrEvt lEvt = MK_TMREVT_INITVAL;
    return lEvt;
}

MK_INLINE void MK_TmrEvt_Init(MK_TmrEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    *aEvt = MK_TmrEvt_InitVal();
}

MK_INLINE void MK_TmrEvt_Destroy(MK_TmrEvt* aEvt)
{
    MK_TmrEvt_Close(aEvt);
}

/******************************************************************************/

MK_INLINE MK_TmrEvtFunc MK_TmrEvt_GetFunc(MK_TmrEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return MK_Cast(MK_TmrEvtFunc, aEvt->Evt[0].Func);
}

MK_INLINE void* MK_TmrEvt_GetObj(MK_TmrEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Evt[0].Obj;
}

MK_INLINE MK_EvtCtrl* MK_TmrEvt_GetCtrl(MK_TmrEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Ctrl;
}

MK_INLINE MK_U32 MK_TmrEvt_GetMSec(MK_TmrEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->MSec;
}

/******************************************************************************/

struct _MK_FileEvt
{
    MK_PInt Lk;
    _MK_Evt Evt[1];

    MK_EvtCtrl* Ctrl;
    MK_File* File;

    MK_S32 Idx;
};

#define _MK_FILEEVT_INITVAL {MK_SYNC_OWNID_INV, {_MK_EVT_INITVAL}, NULL, NULL, -1}

/******************************************************************************/

MK_INLINE MK_FileEvt MK_FileEvt_InitVal(void)
{
    MK_FileEvt lEvt = MK_FILEEVT_INITVAL;
    return lEvt;
}

MK_INLINE void MK_FileEvt_Init(MK_FileEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    *aEvt = MK_FileEvt_InitVal();
}

MK_INLINE void MK_FileEvt_Destroy(MK_FileEvt* aEvt)
{
    MK_FileEvt_Close(aEvt);
}

/******************************************************************************/

MK_INLINE MK_FileEvtFunc MK_FileEvt_GetIFunc(MK_FileEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return MK_Cast(MK_FileEvtFunc, aEvt->Evt[0].Func);
}

MK_INLINE void* MK_FileEvt_GetIObj(MK_FileEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Evt[0].Obj;
}

MK_INLINE MK_EvtCtrl* MK_FileEvt_GetCtrl(MK_FileEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Ctrl;
}

MK_INLINE MK_File* MK_FileEvt_GetFile(MK_FileEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->File;
}

/******************************************************************************/

struct _MK_SockEvt
{
    MK_PInt Lk;
    _MK_Evt Evt[2];

    MK_EvtCtrl* Ctrl;
    MK_Sock* Sock;

    #if !MK_HAVE_KQUEUE && !MK_HAVE_EPOLL
        MK_S32 Idx;
    #endif
};

#if MK_HAVE_KQUEUE || MK_HAVE_EPOLL
    #define _MK_SOCKEVT_INITVAL {MK_SYNC_OWNID_INV, {_MK_EVT_INITVAL, _MK_EVT_INITVAL}, NULL, NULL}
#else
    #define _MK_SOCKEVT_INITVAL {MK_SYNC_OWNID_INV, {_MK_EVT_INITVAL, _MK_EVT_INITVAL}, NULL, NULL, -1}
#endif

/******************************************************************************/

MK_INLINE MK_SockEvt MK_SockEvt_InitVal(void)
{
    MK_SockEvt lEvt = MK_SOCKEVT_INITVAL;
    return lEvt;
}

MK_INLINE void MK_SockEvt_Init(MK_SockEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    *aEvt = MK_SockEvt_InitVal();
}

MK_INLINE void MK_SockEvt_Destroy(MK_SockEvt* aEvt)
{
    MK_SockEvt_Close(aEvt);
}

/******************************************************************************/

MK_INLINE MK_SockEvtFunc MK_SockEvt_GetIFunc(MK_SockEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return MK_Cast(MK_SockEvtFunc, aEvt->Evt[0].Func);
}

MK_INLINE void* MK_SockEvt_GetIObj(MK_SockEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Evt[0].Obj;
}

MK_INLINE MK_SockEvtFunc MK_SockEvt_GetOFunc(MK_SockEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return MK_Cast(MK_SockEvtFunc, aEvt->Evt[1].Func);
}

MK_INLINE void* MK_SockEvt_GetOObj(MK_SockEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Evt[1].Obj;
}

MK_INLINE MK_EvtCtrl* MK_SockEvt_GetCtrl(MK_SockEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Ctrl;
}

MK_INLINE MK_Sock* MK_SockEvt_GetSock(MK_SockEvt* aEvt)
{
    MK_ASSERT(NULL != aEvt);
    return aEvt->Sock;
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
