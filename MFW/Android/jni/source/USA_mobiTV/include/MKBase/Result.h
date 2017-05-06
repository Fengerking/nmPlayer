/*
 * Defines result codes & handling macros.
 *
 * The result codes are defined as a signed integer where 0 is defined as
 * success. A value > 0 is also defined as success but different values can be 
 * used to indicate special considerations. All values < 0 are defined as
 * errors.
 *
 * The actual format of the codes is:
 *
 *    0               1               2               3
 *    0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |S|  R  |          CAT          |   R   |         CODE          |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *    S    = Sign bit (0 for S-codes, 1 for E-codes)
 *    R    = Reserved bits (always 0)
 *    CAT  = Category
 *    CODE = Code
 */

#ifndef __MKBase_Result_h__
#define __MKBase_Result_h__

#include <MKBase/Type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* HELPER MACROS                                                              */
/******************************************************************************/

/*
 * Helper macros for masking out different parts of the Result-code.
 */
#define MK_CATEGORY(R) _MK_CATEGORY(R)
#define MK_CODE(R) _MK_CODE(R)

/*
 * Helper macros for defining Categories, Success- and Error-codes.
 */
#define MK_C(C) _MK_C(C)
#define MK_S(C, S) _MK_S(C, S)
#define MK_E(C, E) _MK_E(C, E)

/******************************************************************************/
/* UNSPECIFIED/GENERIC CODES                                                  */
/******************************************************************************/

/* Category code & helper macros for code creation */

#define MK_C_USPEC         MK_C(0x000)
#define MK_S_USPEC(aCode)  MK_S(MK_C_USPEC, aCode) 
#define MK_E_USPEC(aCode)  MK_E(MK_C_USPEC, aCode)

/* Success codes */

#define MK_S_OK      MK_S_USPEC(0x000) /* Success */
#define MK_S_COND    MK_S_USPEC(0x001) /* Success, with a condition */
#define MK_S_ASYNC   MK_S_USPEC(0x002) /* Success, with async completion */

/* Error codes */

#define MK_E_FAIL    MK_E_USPEC(0x000) /* Unspecified failure */
#define MK_E_CRIT    MK_E_USPEC(0x001) /* Unspecified critical/abortive failure */

#define MK_E_PCOND   MK_E_USPEC(0x008) /* Assertion/precondition failed (likely BUG) */
#define MK_E_NPOS    MK_E_USPEC(0x009) /* Error which should be impossible (likely BUG) */
#define MK_E_PTR     MK_E_USPEC(0x00a) /* Invalid pointer value (likely BUG) */
#define MK_E_NINIT   MK_E_USPEC(0x00b) /* Encountered uninitialized value (likely BUG) */

#define MK_E_IARG    MK_E_USPEC(0x00c) /* Invalid argument */
#define MK_E_NRNG    MK_E_USPEC(0x00d) /* Index/value not in valid range */
#define MK_E_NSUP    MK_E_USPEC(0x00e) /* Unsupported argument value */
#define MK_E_NOENT   MK_E_USPEC(0x00f) /* No matching entry available */
#define MK_E_SNTX    MK_E_USPEC(0x010) /* Syntax error in data */

#define MK_E_OFLOW   MK_E_USPEC(0x011) /* Value too large for datatype */
#define MK_E_UFLOW   MK_E_USPEC(0x012) /* Value too small for datatype */

#define MK_E_NOSPC   MK_E_USPEC(0x013) /* Not enough space available */
#define MK_E_NODATA  MK_E_USPEC(0x014) /* Not enough data available */
#define MK_E_END     MK_E_USPEC(0x015) /* End reached (EOF, EOS or similar) */

#define MK_E_NIMPL   MK_E_USPEC(0x020) /* Action not implemented */
#define MK_E_NACT    MK_E_USPEC(0x021) /* Action valid but disabled */
#define MK_E_NSTAT   MK_E_USPEC(0x022) /* Action valid but not in current state */
#define MK_E_NPERM   MK_E_USPEC(0x023) /* Action not permitted */

#define MK_E_BUSY    MK_E_USPEC(0x030) /* Resource busy */
#define MK_E_AGAIN   MK_E_USPEC(0x031) /* Resource temporarily unavailable */

#define MK_E_INPROG  MK_E_USPEC(0x040) /* Operation now in progress */
#define MK_E_ALREADY MK_E_USPEC(0x041) /* Operation already in progress */
#define MK_E_TMOUT   MK_E_USPEC(0x042) /* Operation timed out */
#define MK_E_INTR    MK_E_USPEC(0x043) /* Operation was interrupted */
#define MK_E_CNCL    MK_E_USPEC(0x044) /* Operation was cancelled */
#define MK_E_DLOCK   MK_E_USPEC(0x045) /* Operation would have deadlocked */

#define MK_E_NOMEM   MK_E_USPEC(0x0f0) /* Memory allocation error */
#define MK_E_CLOCK   MK_E_USPEC(0x0f8) /* Couldn't determine wallclock time */

/******************************************************************************/
/* FILE CODES                                                                 */
/******************************************************************************/

/* Category code & helper macros for code creation */

#define MK_C_FILE         MK_C(0x004)
#define MK_S_FILE(aCode)  MK_S(MK_C_FILE, aCode) 
#define MK_E_FILE(aCode)  MK_E(MK_C_FILE, aCode)

/* Success codes */

/* Error codes */

#define MK_E_FILE_FAIL    MK_E_FILE(0x000) /* Unspecified failure */
#define MK_E_FILE_CRIT    MK_E_FILE(0x001) /* Unspecified critical/abortive failure */

/* TODO: Define more File errors */

/******************************************************************************/
/* NETADDR CODES                                                              */
/******************************************************************************/

/* Category code & helper macros for code creation */

#define MK_C_NETADDR         MK_C(0x006)
#define MK_S_NETADDR(aCode)  MK_S(MK_C_NETADDR, aCode) 
#define MK_E_NETADDR(aCode)  MK_E(MK_C_NETADDR, aCode)

/* Success codes */

/* Error codes */

#define MK_E_NETADDR_FAIL    MK_E_NETADDR(0x000) /* Unspecified failure */
#define MK_E_NETADDR_CRIT    MK_E_NETADDR(0x001) /* Unspecified critical/abortive failure */

#define MK_E_NETADDR_IARG    MK_E_NETADDR(0x00c) /* Invalid argument */
#define MK_E_NETADDR_NSUP    MK_E_NETADDR(0x00e) /* Unsupported address type */

#define MK_E_NETADDR_NOSPC   MK_E_NETADDR(0x013) /* Not enough space avaialble */

#define MK_E_NETADDR_NOADDR  MK_E_NETADDR(0x100) /* No matching address found */
#define MK_E_NETADDR_NONAME  MK_E_NETADDR(0x101) /* No matching name found */

/******************************************************************************/
/* SOCKET CODES                                                               */
/******************************************************************************/

/* Category code & helper macros for code creation */

#define MK_C_SOCK         MK_C(0x007)
#define MK_S_SOCK(aCode)  MK_S(MK_C_SOCK, aCode) 
#define MK_E_SOCK(aCode)  MK_E(MK_C_SOCK, aCode)

/* Success codes */

/* Error codes */

#define MK_E_SOCK_FAIL    MK_E_SOCK(0x000) /* Unspecified failure */
#define MK_E_SOCK_CRIT    MK_E_SOCK(0x001) /* Unspecified critical/abortive failure */

/* TODO: Define more Socket errors */

/******************************************************************************/
/* EVTCTRL CODES                                                              */
/******************************************************************************/

/* Category code & helper macros for code creation */

#define MK_C_EVTCTRL         MK_C(0x00a)
#define MK_S_EVTCTRL(aCode)  MK_S(MK_C_EVTCTRL, aCode) 
#define MK_E_EVTCTRL(aCode)  MK_E(MK_C_EVTCTRL, aCode)

/* Success codes */

/* Error codes */

#define MK_E_EVTCTRL_FAIL    MK_E_EVTCTRL(0x000) /* Unspecified failure */
#define MK_E_EVTCTRL_CRIT    MK_E_EVTCTRL(0x001) /* Unspecified critical/abortive failure */

/* TODO: Define more EvtCtrl errors */

/******************************************************************************/
/* HELPER MACROS                                                              */
/******************************************************************************/

#define MK_OK(aRes) (MK_S_OK == (aRes))
#define MK_PASS(aRes) (MK_S_OK <= (aRes))
#define MK_FAIL(aRes) (MK_S_OK > (aRes))

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Resolves MKBase (and sub-library) result-codes. Returns non-NULL if the
 * result-code was resolved and NULL otherwise.ific code matched).
 */
const MK_Char* MK_GetCategoryDesc(MK_Result aRes, const MK_Char** aName);
const MK_Char* MK_GetResultDesc(MK_Result aRes, const MK_Char** aName);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

/* TODO: Add some #ifdefs to allow pre-definition of macros (for testing purposes) */

#define _MK_C(aCat) (aCat)

#define _MK_RDEF(aCat, aVal) (((aCat) << 16) | (aVal))
#define _MK_S(aCat, aVal) (_MK_S32C(0) | _MK_RDEF(aCat, aVal))
#define _MK_E(aCat, aVal) ((_MK_S32C(1) << 31) | _MK_RDEF(aCat, aVal))

#define _MK_CATEGORY(R) _MK_CODE((R) >> 16)
#define _MK_CODE(R) ((R) & 0xfff)

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
