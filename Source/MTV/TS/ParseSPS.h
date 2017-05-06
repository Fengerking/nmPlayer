#ifndef _PARSESPS_H_
#define _PARSESPS_H_


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else

#ifdef __cplusplus
extern "C" {
#endif
#endif

int GetSizeInfo(unsigned char* buf,int size ,int* width,int* height);
bool ParserSEI(unsigned char* pInPutbuf,int InPutbufSize ,unsigned char* pOutPutbuf,int *OutPutbufSize);


#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif
#endif




#endif//_PARSESPS_H_