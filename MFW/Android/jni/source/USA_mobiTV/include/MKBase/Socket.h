#ifndef __MKBase_Socket_h__
#define __MKBase_Socket_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>
#include <MKBase/Result.h>
#include <MKBase/NetAddr.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

typedef struct _MK_Sock MK_Sock;

#define MK_SOCK_INITVAL _MK_SOCK_INITVAL

/******************************************************************************/

/*
 * Return an initialized MK_Sock structure.
 */
#if !(MK_WIN32 && __cplusplus)
    /* NOTE: Avoid VS bug when compiling as C++ */
    MK_INLINE MK_Sock MK_Sock_InitVal(void);
#endif

/*
 * Initialize aSock structure.
 */
MK_INLINE void MK_Sock_Init(MK_Sock* aSock);

/*
 * Free any resources owned by aSock structure and reinitialize it (alias for
 * MK_Sock_Close function).
 */
MK_INLINE void MK_Sock_Destroy(MK_Sock* aSock);

/******************************************************************************/

/*
 * Create a UDP socket and store it in aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_OpenUDP(MK_Sock* aSock);

/*
 * Create a TCP socket and store it in aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_OpenTCP(MK_Sock* aSock);

/*
 * Shutdown write/read or both communication channels for the connected socket
 * aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_Shutdown(MK_Sock* aSock, MK_Bool aWr, MK_Bool aRd);

/*
 * Free any resources owned by aSock structure and reinitialize it.
 */
void MK_Sock_Close(MK_Sock* aSock);

/******************************************************************************/

/*
 * Allow/disallow reuse of address in use by aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_SetReuseAddr(MK_Sock* aSock, MK_Bool aOn);

/*
 * Set timeout value for output/input operations to aMSec for aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_NIMPL        - Option not supported for socket (always on WinCE).
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_SetSendTimeout(MK_Sock* aSock, MK_U32 aMSec);
MK_Result MK_Sock_SetRecvTimeout(MK_Sock* aSock, MK_U32 aMSec);

/*
 * Set buffer sizes for output/input operations to aSz for aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_NIMPL        - Option not supported for socket (RecvBuf for TCP on WinCE).
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_SetSendBuf(MK_Sock* aSock, MK_U32 aSz);
MK_Result MK_Sock_SetRecvBuf(MK_Sock* aSock, MK_U32 aSz);

/*
 * Enable/disable non-blocking mode for aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_SetNonBlocking(MK_Sock* aSock, MK_Bool aOn);

/*
 * Set multicast send TTL for aSock:
 *
 *   0   = Same host
 *   1   = Same subnet (default)
 *   ...
 *   255 = Unrestricted
 *
 * Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_SetMCastTTL(MK_Sock* aSock, MK_U8 aTTL);

/*
 * Set multicast send interface for aSock (NULL = default). Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_SetMCastIF(MK_Sock* aSock, MK_NetAddr* aIFace);

/******************************************************************************/

/*
 * Bind aSock to the interface address indicated by aAddr. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_Bind(MK_Sock* aSock, const MK_NetAddr* aAddr);

/*
 * Listen for incoming connections on aSock with a queue of aQLen. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_Listen(MK_Sock* aSock, MK_U32 aQLen);

/*
 * Accepts an incoming connection on aSock and saves the connected socket in
 * aNew. Also saves the address of the connecting entity in aAddr unless a
 * NULL value was provided. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_Accept(MK_Sock* aSock, MK_Sock* aNew, MK_NetAddr* aAddr);

/*
 * Connect aSock to the host address indicated by aAddr. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_INPROG       - Connection attempt in progress (non-blocking only)
 *   MK_E_TMOUT        - Connection attempt timed out
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_Connect(MK_Sock* aSock, const MK_NetAddr* aAddr);

/*
 * Join/Leave the multicast group designated by aMCast using the interface in
 * aIFace (NULL == default interface) for the socket aSock (or rather the port
 * bound by aSock). Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_Join(MK_Sock* aSock, const MK_NetAddr* aMCast, const MK_NetAddr* aIFace);
MK_Result MK_Sock_Leave(MK_Sock* aSock, const MK_NetAddr* aMCast, const MK_NetAddr* aIFace);

/******************************************************************************/

/*
 * Get and clear any pending error for the socket aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_TMOUT        - Operation attempt timed out
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_GetError(MK_Sock* aSock);

/*
 * Get the locally bound/remote address for the socket aSock. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_Sock_GetLocalAddr(MK_Sock* aSock, MK_NetAddr* aAddr);
MK_Result MK_Sock_GetRemoteAddr(MK_Sock* aSock, MK_NetAddr* aAddr);

/*
 * Check validity of aSock. Returns MK_TRUE if valid and MK_FALSE otherwise.
 */
MK_INLINE MK_Bool MK_Sock_IsValid(MK_Sock* aSock);

/*
 * Check if aSock is connected. Returns MK_TRUE if socket is connected and
 * MK_FALSE otherwise.
 *
 * NOTE: Exacly when a previously connected socket goes to being disconnected
 *       varies between platforms so using this function alone to detect a
 *       disconnection may be troublesome.
 */
MK_Bool MK_Sock_IsConnected(MK_Sock* aSock);

/*
 * Returns number of bytes directly available for receive (without blocking). A
 * negatve value means an error occured.
 *
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_Sock_Avail(MK_Sock* aSock);

/*
 * Check if aSock is at EOS. Returns MK_TRUE if socket is at EOS and MK_FALSE
 * otherwise.
 *
 * NOTE: This function may have unpredictable results between platforms for TCP
 *       sockets prior to establishing their type with a Listen() or Connect()
 *       call.
 */
MK_INLINE MK_Bool MK_Sock_PendingEOS(MK_Sock* aSock);

/******************************************************************************/

/*
 * Wait a maximum of aMSec (0 for IsWritable) millisecond for the socket aSock
 * to become writable. Returns a value > 0 if the socket became writable, 0 if
 * the timeout expired and < 0 if an error occured:
 *
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_Sock_WaitWritable(MK_Sock* aSock, MK_U32 aMSec);

/*
 * Check if aSock is writable. Returns MK_TRUE if socket is writable and
 * MK_FALSE otherwise.
 */
MK_INLINE MK_Bool MK_Sock_IsWritable(MK_Sock* aSock)
{
    return (0 < MK_Sock_WaitWritable(aSock, 0));
}

/*
 * Wait a maximum of aMSec (0 for IsReadable) millisecond for the socket aSock
 * to become readable. Returns a value > 0 if the socket became readable, 0 if
 * the timeout expired and < 0 if an error occured.
 *
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_Sock_WaitReadable(MK_Sock* aSock, MK_U32 aMSec);

/*
 * Check if aSock is readable. Returns MK_TRUE if socket is readable and
 * MK_FALSE otherwise.
 */
MK_INLINE MK_Bool MK_Sock_IsReadable(MK_Sock* aSock)
{
    return (0 < MK_Sock_WaitReadable(aSock, 0));
}

/******************************************************************************/

/*
 * Send aBufSz bytes of data from aBuf (SendTo also specifies destination in
 * aAddr). Returns the number of bytes actually sent (which may be less than
 * aBufSz). A return value equal to aBufSz indicates success, a value < aBufSz
 * indicates a partial send (due to timeout, interruption or EOS) and a
 * value < 0 indicates an error:
 *
 *   MK_E_AGAIN        - Would block/timeout
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_Sock_Send(MK_Sock* aSock, const void* aBuf, MK_U32 aBufSz);
MK_S32 MK_Sock_SendTo(MK_Sock* aSock, const void* aBuf, MK_U32 aBufSz, const MK_NetAddr* aAddr);

/*
 * Receive aBufSz data into aBuf from aSock (RecvFrom also saves the sender
 * address in aAddr). Returns the number of bytes actually received (which may
 * be less than aBufSz). A return-value equal to aBufSz indicates success, a
 * value < aBufSz but > 0 indicates a partial receive (due to timeout,
 * interruption or EOS), a value equal to 0 indicates EOS and a value < 0
 * indicates an error:
 *
 *   MK_E_AGAIN        - Would block/timeout
 *   MK_E_SOCK_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_Sock_Recv(MK_Sock* aSock, void* aBuf, MK_U32 aBufSz);
MK_S32 MK_Sock_RecvFrom(MK_Sock* aSock, void* aBuf, MK_U32 aBufSz, MK_NetAddr* aAddr);

/******************************************************************************/

/*
 * TODO:
 *  - Improve errors by adding support for more codes.
 *  - Add getters for some option values?
 *  - Support keep-alive? We don't support keep-alive since WinMo doesn't allow
 *    setting the timing values and the defaults at not very useful.
 *  - SNDTIMEO/RCVTIMEO seems unsupported on WinMo, may need a hack to replace
 *    them... or perhaps remove them and use external timeouts instead.
 *  - Only IPv4 is supported for some functions.
 *  - We currently don't support IP_MULTICAST_LOOP because of platform differences.
 */   

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#if MK_WIN32
    #define _MK_SOCK_T SOCKET
    #define _MK_SOCK_INV INVALID_SOCKET
#else
    #define _MK_SOCK_T MK_S32
    #define _MK_SOCK_INV -1
#endif

struct _MK_Sock
{
    _MK_SOCK_T Hdl;
};

#define _MK_SOCK_INITVAL {_MK_SOCK_INV}

/******************************************************************************/

MK_INLINE MK_Sock MK_Sock_InitVal(void)
{
    MK_Sock lSock = MK_SOCK_INITVAL;
    return lSock;
}

MK_INLINE void MK_Sock_Init(MK_Sock* aSock)
{
    MK_ASSERT(NULL != aSock);
    *aSock = MK_Sock_InitVal();
}

MK_INLINE void MK_Sock_Destroy(MK_Sock* aSock)
{
    MK_Sock_Close(aSock);
}

/******************************************************************************/

MK_INLINE MK_Bool MK_Sock_IsValid(MK_Sock* aSock)
{
    return (NULL != aSock && _MK_SOCK_INV != aSock->Hdl) ? MK_TRUE : MK_FALSE;
}

MK_INLINE MK_Bool MK_Sock_PendingEOS(MK_Sock* aSock)
{
    return (MK_Sock_IsValid(aSock) && MK_Sock_IsReadable(aSock) && 0 == MK_Sock_Avail(aSock)) ? MK_TRUE : MK_FALSE;
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
