
#ifndef _CCODECC_H_
#define _CCODECC_H_

#include "voYYDef_filcmn.h"
#include "fCodec.h"
#include "fCC.h"
#include "voSource.h"
#include "fCC2.h"
#include<ctype.h>
#include "voSource2.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifndef FCC
#define FCC(ch4) ((((VO_U32)(ch4) & 0xFF) << 24) |     \
	(((VO_U32)(ch4) & 0xFF00) << 8) |    \
	(((VO_U32)(ch4) & 0xFF0000) >> 8) |  \
	(((VO_U32)(ch4) & 0xFF000000) >> 24))
#endif	//FCC

 inline   char*   strlwr(   char*   str   ) 
 { 
 	char*   orig   =   str; 
 	//   process   the   string 
 	for   (   ;   *str   !=   '\0';   str++   ) 
 		*str   =   tolower(*str); 
 	return   orig; 
 } 

class CCodeCC
{
public:
	static unsigned int GetCC(unsigned int uType);
	static unsigned int GetCC(const char *cType);
	static unsigned int GetCodecByCC(VO_SOURCE_TRACKTYPE nType, unsigned int uCC);
	static unsigned int GetCodecByCC(VO_SOURCE2_TRACK_TYPE nType, unsigned int uCC);
	
};

#ifdef _VONAMESPACE
}
#endif

#endif//end #ifndef _CCODECC_H_
