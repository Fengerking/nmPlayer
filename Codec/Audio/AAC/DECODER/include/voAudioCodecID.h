#ifndef __VOAUDIOCODEC_HANDLE_
#define __VOAUDIOCODEC_HANDLE_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#define ADD_ID(func)	AACDec_##func


#define cmnMemAlloc                            ADD_ID(cmnMemAlloc) 
#define cmnMemFree                             ADD_ID(cmnMemFree) 
#define cmnMemSet                              ADD_ID(cmnMemSet) 
#define cmnMemCopy                             ADD_ID(cmnMemCopy)
#define cmnMemCheck                            ADD_ID(cmnMemCheck) 
#define cmnMemCompare                          ADD_ID(cmnMemCompare) 
#define cmnMemMove                             ADD_ID(cmnMemMove) 




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif