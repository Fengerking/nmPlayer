
#ifndef __VO_WMV_PORT_H__
#define __VO_WMV_PORT_H__

#define  BUILD_WITHOUT_C_LIB  0

#if BUILD_WITHOUT_C_LIB
#define malloc  voWMVMalloc                       
#define calloc  voWMVCalloc  
#define realloc voWMVRealloc                     
#define free 	voWMVFree                          
#define memcpy	voWMVMemcpy                    
#define memmove	voWMVMemmove                 
#define memset	voWMVMemset
#define assert(x)
//#define printf  voWMVPrintf
//#define fprintf voWMVFprintf
//#define fopen   voWMVFopen
//#define fflush  voWMVFflush
//#define fclose  voWMVFclose
#define NULL    0
#define size_t  unsigned int
/*MATH*/
#define ceil   voWMVceil   
#define floor  voWMVfloor   
#define sqrt   voWMVsqrt   
#define sin    voWMVsin    
#define cos    voWMVcos    
#define tan    voWMVtan    
#define asin   voWMVasin   
#endif //BUILD_WITHOUT_C_LIB

#if !BUILD_WITHOUT_C_LIB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
//#include <malloc.h>
#include "assert.h"
#endif

#include "voVC1DID.h"

//#ifdef VOANDROID
#define STABILITY 1
#undef  assert
#define assert 
//#endif

#endif //__VO_WMV_PORT_H__