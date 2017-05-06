#ifndef __MKBase_NetAddr_h__
#define __MKBase_NetAddr_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>
#include <MKBase/Result.h>
#include <MKBase/Memory.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

typedef struct _MK_NetAddr MK_NetAddr;

#define MK_NETADDR_INITVAL _MK_NETADDR_INITVAL

/******************************************************************************/

/*
 * Return an initialized MK_NetAddr structure.
 */
#if !(MK_WIN32 && __cplusplus)
    /* NOTE: Avoid VS bug when compiling as C++ */
    MK_INLINE MK_NetAddr MK_NetAddr_InitVal(void);
#endif

/*
 * Initialize aAddr structure.
 */
MK_INLINE void MK_NetAddr_Init(MK_NetAddr* aAddr);

/*
 * Free any resources owned by aAddr structure and reinitialize it.
 */
MK_INLINE void MK_NetAddr_Destroy(MK_NetAddr* aAddr);

/******************************************************************************/

/*
 * Get a NetAddr suitable for binding to a local interface using the information
 * in aHost (NULL -> ANY) and aSvc.
 *
 *   MK_S_OK              - Success
 *
 *   MK_E_AGAIN           - Temporary failure in name resolution
 *   MK_E_NOMEM           - Memory allocation error
 *
 *   MK_E_NETADDR_IARG    - Invalid argument
 *   MK_E_NETADDR_NOADDR  - No matching address found
 *   MK_E_NETADDR_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_NetAddr_GetBindAddr(const MK_Char* aHost, const MK_Char* aSvc, MK_NetAddr* aAddr);

/*
 * Get a NetAddr suitable for connecting to a remote host using the information
 * in aHost (NULL -> LOOPBACK) and aSvc.
 *
 *   MK_S_OK              - Success
 *
 *   MK_E_AGAIN           - Temporary failure in name resolution
 *   MK_E_NOMEM           - Memory allocation error
 *
 *   MK_E_NETADDR_IARG    - Invalid argument
 *   MK_E_NETADDR_NOADDR  - No matching address found
 *   MK_E_NETADDR_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_NetAddr_GetConnectAddr(const MK_Char* aHost, const MK_Char* aSvc, MK_NetAddr* aAddr);

/******************************************************************************/

/*
 * Set the port of aAddr to aPort.
 *
 *   MK_S_OK              - Success
 *
 *   MK_E_NETADDR_IARG    - Invalid argument
 *   MK_E_NETADDR_NSUP    - Unsupported address type
 *   MK_E_NETADDR_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_NetAddr_SetPort(MK_NetAddr* aAddr, MK_U16 aPort);

/******************************************************************************/

/*
 * Convert a NetAddr to numeric strings with the host/IP part in in aIP (bounded
 * by aIPSz) and the service/port part in aPort (bounded by aPortSz).
 *
 *   MK_S_OK              - Success
 *
 *   MK_E_AGAIN           - Temporary failure in name resolution
 *   MK_E_NOMEM           - Memory allocation error
 *
 *   MK_E_NETADDR_IARG    - Invalid argument
 *   MK_E_NETADDR_NOSPC   - Not enough space available
 *   MK_E_NETADDR_NONAME  - No matching name found
 *   MK_E_NETADDR_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_NetAddr_ToNumericCStr(const MK_NetAddr* aAddr, MK_Char* aIP, MK_U32 aIPSz, MK_Char* aPort, MK_U32 aPortSz);

/******************************************************************************/

/*
 * TODO:
 *  - Improve errors by adding support for more codes.
 *  - Add Get*Addr-versions which return a list of options to choose from?
 *  - Add CStr conversion with reverse name lookup?
 *  - Add NetAddr comparison?
 */   

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#if MK_WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
#endif

#if MK_HAVE_SA_STORAGE
    #define _MK_NETADDR_STORAGE_T struct sockaddr_storage
#else
    #define _MK_NETADDR_STORAGE_T struct { MK_U8 Buf[128]; }
#endif

#define _MK_NETADDR_STORAGE_LEN sizeof(_MK_NETADDR_STORAGE_T)
#define _MK_NETADDR_STORAGE_DECL _MK_NETADDR_STORAGE_T Buf;
#define _MK_NETADDR_SOCKADDR(aNA) (MK_Cast(struct sockaddr*, &(aNA)->Buf))
#define _MK_NETADDR_SOCKADDR_IN(aNA) (MK_Cast(struct sockaddr_in*, &(aNA)->Buf))

#if MK_HAVE_SA_LEN
    #define _MK_NETADDR_LEN_T
    #define _MK_NETADDR_LEN_DECL
    #define _MK_NETADDR_LEN(aNA) (_MK_NETADDR_SOCKADDR(aNA)->sa_len)
    #define _MK_NETADDR_SETLEN(aNA, aVal)
#else
    #define _MK_NETADDR_LEN_T MK_U8
    #define _MK_NETADDR_LEN_DECL _MK_NETADDR_LEN_T Len;
    #define _MK_NETADDR_LEN(aNA) ((aAddr)->Len)
    #define _MK_NETADDR_SETLEN(aNA, aVal) (_MK_NETADDR_LEN(aNA) = aVal)
#endif

#define _MK_NETADDR_LENZ(aNA) ((NULL != aNA) ? _MK_NETADDR_LEN(aNA) : 0)
#define _MK_NETADDR_SETLENZ(aNA, aVal) if (NULL != aNA) (_MK_NETADDR_LEN(aNA) = aVal); else {}

#define _MK_NETADDR_FAMILY(aNA) ((NULL != aNA) ? _MK_NETADDR_SOCKADDR(aNA)->sa_family : AF_UNSPEC)

struct _MK_NetAddr
{
    _MK_NETADDR_STORAGE_DECL
    _MK_NETADDR_LEN_DECL
};

#define _MK_NETADDR_INITVAL MK_NetAddr_InitVal()

/******************************************************************************/

MK_INLINE MK_NetAddr MK_NetAddr_InitVal(void)
{
    MK_NetAddr lAddr;
    MK_Mem_Zero(&lAddr, sizeof(lAddr));
    return lAddr;
}

MK_INLINE void MK_NetAddr_Init(MK_NetAddr* aAddr)
{
    MK_ASSERT(NULL != aAddr);
    *aAddr = MK_NetAddr_InitVal();
}

MK_INLINE void MK_NetAddr_Destroy(MK_NetAddr* aAddr)
{
    MK_NetAddr_Init(aAddr);
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
