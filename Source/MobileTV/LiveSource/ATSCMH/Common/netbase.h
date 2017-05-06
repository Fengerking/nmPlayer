#ifndef __NETBASE_H__
#define __NETBASE_H__

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
#   include <winsock2.h>
#   include <Windows.h>
#   include <ws2tcpip.h>
#elif defined(LINUX)
#   include "vocrstypes.h"
#   include "vocrsapis.h"
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <sys/select.h>
#   include <sys/types.h>
#   include <arpa/inet.h>
#endif

#endif //__NETBASE_H__