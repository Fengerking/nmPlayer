#ifndef __VOOGGDECID_H_
#define __VOOGGDECID_H_

#ifndef _WIN32_WCE
#ifndef _DEBUG

#define RENAME(func)	_##func

//#define g_hADPCMDecInst                               voADPCMDec00000018 
#define g_memOP                                       voADPCMDec00000019 
#define voADPCMDecADPCM                               voADPCMDec00000020 
#define voADPCMDecAlignBits                           voADPCMDec00000021 
//#define voADPCMDecGetBits                             voADPCMDec00000022 
#define voADPCMDecGetOutputData                       voADPCMDec00000023 
#define voADPCMDecGetParam                            voADPCMDec00000024 
#define voADPCMDecIMAADPCMDec                         voADPCMDec00000025 
#define voADPCMDecITUG726ADPCMDec                     voADPCMDec00000026 
#define voADPCMDecInit                                voADPCMDec00000027 
//#define voADPCMDecInitBits                            voADPCMDec00000028 
#define voADPCMDecInitIMAInfo                         voADPCMDec00000029 
#define voADPCMDecInitITUG726Info                     voADPCMDec00000030 
#define voADPCMDecInitMSInfo                          voADPCMDec00000031 
#define voADPCMDecMSADPCMDec                          voADPCMDec00000032 
#define voADPCMDecReadFMTChunk                        voADPCMDec00000033 
#define voADPCMDecSWFADPCMDec                         voADPCMDec00000034 
#define voADPCMDecSetInputData                        voADPCMDec00000035 
#define voADPCMDecSetParam                            voADPCMDec00000036 
#define voADPCMDecSkipBits                            voADPCMDec00000037 
#define voADPCMDecStreamInit                          voADPCMDec00000038 
#define voADPCMDecUninit                              voADPCMDec00000039 
#define voADPCMDecaLawADPCMDec                        voADPCMDec00000040 
//#define voADPCMDecmem_free                            voADPCMDec00000041 
//#define voADPCMDecmem_malloc                          voADPCMDec00000042 
#define voADPCMDecuLawADPCMDec                        voADPCMDec00000043 

//#define _g_hADPCMDecInst                               RENAME(voADPCMDec00000018) 
#define _g_memOP                                       RENAME(voADPCMDec00000019) 
#define _voADPCMDecADPCM                               RENAME(voADPCMDec00000020) 
#define _voADPCMDecAlignBits                           RENAME(voADPCMDec00000021) 
#define _voADPCMDecGetBits                             RENAME(voADPCMDec00000022) 
#define _voADPCMDecGetOutputData                       RENAME(voADPCMDec00000023) 
#define _voADPCMDecGetParam                            RENAME(voADPCMDec00000024) 
#define _voADPCMDecIMAADPCMDec                         RENAME(voADPCMDec00000025) 
#define _voADPCMDecITUG726ADPCMDec                     RENAME(voADPCMDec00000026) 
#define _voADPCMDecInit                                RENAME(voADPCMDec00000027) 
#define _voADPCMDecInitBits                            RENAME(voADPCMDec00000028) 
#define _voADPCMDecInitIMAInfo                         RENAME(voADPCMDec00000029) 
#define _voADPCMDecInitITUG726Info                     RENAME(voADPCMDec00000030) 
#define _voADPCMDecInitMSInfo                          RENAME(voADPCMDec00000031) 
#define _voADPCMDecMSADPCMDec                          RENAME(voADPCMDec00000032) 
#define _voADPCMDecReadFMTChunk                        RENAME(voADPCMDec00000033) 
#define _voADPCMDecSWFADPCMDec                         RENAME(voADPCMDec00000034) 
#define _voADPCMDecSetInputData                        RENAME(voADPCMDec00000035) 
#define _voADPCMDecSetParam                            RENAME(voADPCMDec00000036) 
#define _voADPCMDecSkipBits                            RENAME(voADPCMDec00000037) 
#define _voADPCMDecStreamInit                          RENAME(voADPCMDec00000038) 
#define _voADPCMDecUninit                              RENAME(voADPCMDec00000039) 
#define _voADPCMDecaLawADPCMDec                        RENAME(voADPCMDec00000040) 
#define _voADPCMDecmem_free                            RENAME(voADPCMDec00000041) 
#define _voADPCMDecmem_malloc                          RENAME(voADPCMDec00000042) 
#define _voADPCMDecuLawADPCMDec                        RENAME(voADPCMDec00000043) 




#endif //#ifndef _DEBUG
#endif //#ifndef _WIN32_WCE
#endif //#ifndef __VOOGGDECID_H_