
#ifndef _VO_SOCKET__H
#define _VO_SOCKET__H

#include "voType.h"

#ifdef _WIN32

//#include <winsock2.h>
//#include "Ws2tcpip.h"
#ifdef WINCE
//#include <winsock2.h>
#endif
#else
#include "unistd.h"
#include "netdb.h"
#include "fcntl.h"
#endif //_WIN32

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif

#define DEFAULTVOSOCKETRECVTIMEOUT	2
#define DEFAULTVOSOCKETSENDTIMEOUT	2

VO_S32 vo_socket_send( VO_S32 s , VO_PBYTE buffer , VO_S32 size );
VO_S32 vo_socket_recv( VO_S32 s , VO_PBYTE buffer , VO_S32 size );
VO_S32 vo_socket_send_safe( VO_S32 s , VO_PBYTE buffer , VO_S32 size , VO_BOOL * bexit = NULL );
VO_S32 vo_socket_close( VO_S32 s );
VO_BOOL vo_socket_init();
VO_VOID vo_socket_uninit();
VO_BOOL vo_socket_connect( VO_S32 * ptr_socket , VO_CHAR * str_host , VO_CHAR * str_port , VO_BOOL * bexit = NULL );

#ifdef ENABLE_ASYNCDNS
VO_BOOL vo_socket_connect_asyncdns( VO_S32 * ptr_socket , VO_CHAR * str_host , VO_CHAR * str_port , VO_BOOL * bexit , VO_BOOL * bDNSFailed , VO_CHAR *ptr_addr );
#endif

//set socket time out of recv & send 
VO_BOOL vo_socket_setTimeout(VO_S32 * ptr_socket, VO_S32 recv = DEFAULTVOSOCKETRECVTIMEOUT , VO_S32 send = DEFAULTVOSOCKETSENDTIMEOUT ); 

//set receive time out value acoording http server response field "X-SocketTimeout"
VO_S32 vo_socket_setReceiveTimeout(VO_S32 * ptr_socket, VO_S32 seconds); 
        
#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#endif