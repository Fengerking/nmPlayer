 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_debug.c
    \brief    debug functions
    \author   Renjie Yu
	\change
*/
#include <stdio.h>
#include "h265dec_debug.h"
#include "stdlib.h"

#if H265DEC_DEBUG
FILE *p_dbg_file;
const VO_U32 k_debug_level  = LOG_INFO_ALL;
const VO_U32 k_debug_module = LOG_PARSER | LOG_MOTION;
	

void IniDebug()
{
	p_dbg_file = fopen("h265dec_log.txt", "ab+");

}

void flushDebug()
{
  if (p_dbg_file){
    fflush (p_dbg_file);
  }


}

void UiniDebug()
{
	if (p_dbg_file){
		fclose (p_dbg_file);
	}

}


#if ENC_DEC_TRACE
const VO_BOOL g_bEncDecTraceEnable  = VO_TRUE;
const VO_BOOL g_bEncDecTraceDisable = VO_FALSE;
VO_BOOL   g_HLSTraceEnable = VO_TRUE;
VO_BOOL   g_bJustDoIt = VO_TRUE;
VO_U64		g_nSymbolCounter = 0;
#endif

#endif
