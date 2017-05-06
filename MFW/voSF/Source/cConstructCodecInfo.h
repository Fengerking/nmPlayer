#ifndef _cConstructCodecInfo_H_
#define _cConstructCodecInfo_H_

#ifdef __cplusplus
extern "C" {
#endif

bool ConstructESDS(unsigned char *pInput , int nInputSize , unsigned char *pOutput , int &nOutputSize);

#ifdef __cplusplus
}
#endif

#endif //_cConstructCodecInfo_H_