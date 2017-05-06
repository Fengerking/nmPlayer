 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_debug.h
    \brief    debug functions
    \author   Renjie Yu
	\change
*/

#ifndef __H265DEC_DEBUG__
#define __H265DEC_DEBUG__
#include "voType.h"

#define LOG_FILE_NAME_FOR_INTRA_IQ_IDCT "Log_file_for_pred_IQ_IDCT.txt"




//Module 
#define LOG_PARSER              0x010
#define LOG_MOTION              0x020
#define	LOG_IDCT_DQ             0x040 
#define LOG_MC                  0x080
#define LOG_DEBLOCK             0x100
#define LOG_SA0                 0x200
#define LOG_GLOBAL              0x400
#define LOG_MODULE_ALL          0//(LOG_PARSER | LOG_MOTION | LOG_IDCT_DQ | LOG_MC | LOG_DEBLOCK | LOG_SA0)


//Output type
#define LOG_PRINT               0X1
#define LOG_FILE                0X2
#define LOG_OUTTYPE_ALL         (LOG_PRINT | LOG_FILE )


//Log info
#define LOG_STREAM_INFO         0X1
#define LOG_STAT_FINFO          0X2
#define LOG_DEBUG_INFO          0X4
#define LOG_INFO_ALL            (LOG_STREAM_INFO | LOG_STAT_FINFO | LOG_DEBUG_INFO)    

#if H265DEC_DEBUG
#define LOG_BUF_SZ 1024

extern void IniDebug();
extern void UiniDebug();
extern void flushDebug();



#if defined(_MSC_VER)
                  
#include <windows.h>
#include <stdio.h>

extern FILE *p_dbg_file;
extern const VO_U32 k_debug_level;
extern const VO_U32 k_debug_module;
#define snprintf _snprintf
#define vsnprintf _vsnprintf

static __inline void VO_LOG(int out_type, int log_level, char *str, ...)
{
	va_list args;
	char log_buf[LOG_BUF_SZ];
	va_start(args, str);
	vsprintf(log_buf, str, args);
	va_end(args);
	
	if ((log_level & k_debug_level) && (log_level & k_debug_module)) {
		if (out_type & LOG_PRINT) {
			printf("%s", log_buf);
		}
		
		if ((out_type & LOG_FILE) && p_dbg_file) {
			fprintf(p_dbg_file, "%s", log_buf);
		}
	}
	
}
#elif defined (RVDS)
static __inline void VO_LOG(int out_type, int log_level,  char *str, ...){

}

#endif//_MSC_VER

#else//H265DEC_DEBUG
static __inline void VO_LOG(int out_type, int log_level,  char *str, ...){}
#endif


#endif//__H265DEC_DEBUG__