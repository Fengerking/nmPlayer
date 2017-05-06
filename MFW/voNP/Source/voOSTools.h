//
//  voOSTools.h
//
//  Created by Lin Jun on 10/24/12.
//  Copyright (c) 2012 VisualOn. All rights reserved.
//

#ifndef _VO_OS_TOOLS_H_
#define _VO_OS_TOOLS_H_

#include "voNPWrapType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

//zip buffers,return value is the buffer size after compress,-1 indicator error
int         voCompress(unsigned char* pInput, int nInputSize, unsigned char* pOutput, int nOutputSize);
    
bool        IsHighPerformanceDevices();
bool        GetCpuInfo(VONP_CPU_INFO* pInfo);
const char* GetOSVersion();
    
    
#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif // end of _VO_OS_TOOLS_H_
