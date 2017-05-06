#ifndef __CDRM_VERIMATRIX_AES128_H__
#define __CDRM_VERIMATRIX_AES128_H__

#if defined _IOS
#include "DRM_Verimatrix_AES128_iOS.h"
#elif defined _LINUX_ANDROID
#include "DRM_Verimatrix_AES128_Other.h"
#elif defined _WIN32
#include "DRM_Verimatrix_AES128_Win.h"
#elif defined _MAC_OS
#include "DRM_Verimatrix_AES128_mac.h"
#endif

#endif //__CDRM_VISUALON_AES128_H__
