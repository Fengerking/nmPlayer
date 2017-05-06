/*
********************************************************************************
*
*      File             : typedef.c
*      Purpose          : Basic types.
*
********************************************************************************
*/


#ifndef typedef_h
#define typedef_h "$Id $"


#undef ORIGINAL_TYPEDEF_H /* define to get "original" ETSI version
                   //          of typedef.h                           */

#ifdef ORIGINAL_TYPEDEF_H
/*
 * this is the original code from the ETSI file typedef.h
 */
   
#if defined(__BORLANDC__) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(__ZTC__) 
typedef signed char Word8;
typedef short Word16;
typedef long Word32;
typedef int Flag;

#elif defined(__sun)
typedef signed char Word8;
typedef short Word16;
typedef long Word32;
typedef int Flag;

#elif defined(__unix__) || defined(__unix)
typedef signed char Word8;
typedef short Word16;
typedef int Word32;
typedef int Flag;

#endif
#else /* not original typedef.h */

/*
 * use (improved) type definition file typdefs.h and add a "Flag" type
 */
#include "voAMRWBDecID.h"
#include "typedefs.h"
typedef int Flag;

#endif// ORIGINAL_TYPEDEF_H
#define AMR_DUMP 0
#if AMR_DUMP
#define L_SUBFR 64
#define M 16
#include <stdio.h>
#include <stdarg.h>
#define AMR_DEBUG_FRAME				(1<< 0)
#define AMR_DEBUG_result			(1<< 1)
#define AMR_DEBUG_D_plsf			(1<< 2) 
#define AMR_DEBUG_Isf_isp		(1<< 3)
#define AMR_DEBUG_Int_isp			(1<< 4)
#define AMR_DEBUG_lagconc		(1<< 5)
#define AMR_DEBUG_Pred_lt4		(1<< 6)  
#define AMR_DEBUG_DEC_ACELP		(1<< 7) 
#define AMR_DEBUG_Preemph			(1<< 8)
#define AMR_DEBUG_Pit_shrp			(1<< 9) 
#define AMR_DEBUG_D_gain2		(1<< 10)  

#define AMR_DEBUG_Scale_sig			(1<< 11) 
#define AMR_DEBUG_voice_factor				(1<<12)
#define AMR_DEBUG_Phase_dispersion			(1<<13)
#define AMR_DEBUG_synthesis			(1<<14)
#define AMR_DEBUG_POSTPROCESS		(1<<15)
#define AMR_DEBUG_TESTFUNC			(1<<16)
#define AMR_DEBUG_ALL  0xffffffff

extern int amr_debug; 
extern FILE* fLog;
static __inline void DPRINTF(int level, char *fmt, ...)
{
	if (amr_debug & level) {
		va_list args;
		char buf[1024];
		va_start(args, fmt);
		vsprintf(buf, fmt, args);
		va_end(args);
		fprintf(fLog, "%s", buf);
	}
}
typedef enum
{
	d8  = 0,
    d16 = 1,
    d32 = 2
}DUMP_DataType;



static __inline void Dumploop(int dumpflag,char* name,int count,void* src,DUMP_DataType type)
{
	switch(type) {
	case d8:
		{
			char* data = src;
			int i;
			DPRINTF(dumpflag,"\n%s\n",name);
			for(i=0;i<count;i++)
			{
				DPRINTF(dumpflag,"[%d]=%d,",i,data[i]);	
			}

		}
		break;
	case d16:
		{
			short* data = src;
			int i;
			DPRINTF(dumpflag,"\n%s\n",name);
			for(i=0;i<count;i++)
			{
				DPRINTF(dumpflag,"[%d]=%d,",i,data[i]);	
			}
			
		}
		break;
	case d32:
		{
			int* data = src;
			int i;
			DPRINTF(dumpflag,"\n%s\n",name);
			for(i=0;i<count;i++)
			{
				DPRINTF(dumpflag,"[%d]=%d,",i,data[i]);	
			}
			
		}
		break;
	default:
		break;
	}
}
static __inline void Dumploop2(int dumpflag,char* name,int count1,int count2,void* src,DUMP_DataType type)
{
	switch(type) {
	case d8:
		{
			char* data = src;
			int i;
			DPRINTF(dumpflag,"\n%s\n",name);
			for(i=0;i<count1;i++)
			{
				Dumploop(dumpflag,"",count2,data,type);
				data+=count2;
			}
			
		}
		break;
	case d16:
		{
			short* data = src;
			int i;
			DPRINTF(dumpflag,"\n%s\n",name);
			for(i=0;i<count1;i++)
			{
				Dumploop(dumpflag,"",count2,data,type);
				data+=count2;
			}
			
		}
		break;
	case d32:
		{
			int* data = src;
			int i;
			DPRINTF(dumpflag,"\n%s\n",name);
			for(i=0;i<count1;i++)
			{
				Dumploop(dumpflag,"",count2,data,type);
				data+=count2;
			}
			
		}
		break;
	default:
		break;
	}
}

#endif//AMR_DUMP
#endif

