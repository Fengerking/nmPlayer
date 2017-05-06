#ifndef __VOOGGDECID_H_
#define __VOOGGDECID_H_

#ifndef _WIN32_WCE
#ifndef _DEBUG

#define RENAME(func)	_##func

//#define g_hFLACDecInst                                voFLACDec00000000 
#define g_memOP                                       voFLACDec00000001 
//#define voFLACDecAlignBits                            voFLACDec00000002 
//#define voFLACDecCalcBytesUsed                        voFLACDec00000003 
#define voFLACDecChanAssignMix                        voFLACDec00000004 
//#define voFLACDecGetBits                              voFLACDec00000005 
//#define voFLACDecGetRiceBits                          voFLACDec00000006 
//#define voFLACDecGetUnaryBits                         voFLACDec00000007 
//#define voFLACDecGetUtf8                              voFLACDec00000008 
//#define voFLACDecGetsBits                             voFLACDec00000009 
#define voFLACDecHeaderDecode                         voFLACDec00000010 
//#define voFLACDecHeaderSync                           voFLACDec00000011 
//#define voFLACDecInitBits                             voFLACDec00000012 
#define voFLACDecInitStreamBuf                        voFLACDec00000013 
#define voFLACDecReadMetaInfo                         voFLACDec00000014 
//#define voFLACDecSkipBits                             voFLACDec00000015 
#define voFLACDecStreamBufferUpdata                   voFLACDec00000016 
#define voFLACDecStreamFinish                         voFLACDec00000017 
#define voFLACDecStreamFlush                          voFLACDec00000018 
#define voFLACDecStreamInit                           voFLACDec00000019 
#define voFLACDecUpdateFrameBuffer                    voFLACDec00000020 
#define voFLACDec_GetOutputData                       voFLACDec00000021 
#define voFLACDec_GetParam                            voFLACDec00000022 
#define voFLACDec_Init                                voFLACDec00000023 
#define voFLACDec_SetInputData                        voFLACDec00000024 
#define voFLACDec_SetParam                            voFLACDec00000025 
#define voFLACDec_Uninit                              voFLACDec00000026 
//#define voFLACDecmem_free                             voFLACDec00000027 
//#define voFLACDecmem_malloc                           voFLACDec00000028 
#define voFLACDecodeSubFrame                          voFLACDec00000029 

#define _g_hFLACDecInst                                RENAME(voFLACDec00000000) 
#define _g_memOP                                       RENAME(voFLACDec00000001) 
#define _voFLACDecAlignBits                            RENAME(voFLACDec00000002) 
#define _voFLACDecCalcBytesUsed                        RENAME(voFLACDec00000003) 
#define _voFLACDecChanAssignMix                        RENAME(voFLACDec00000004) 
#define _voFLACDecGetBits                              RENAME(voFLACDec00000005) 
#define _voFLACDecGetRiceBits                          RENAME(voFLACDec00000006) 
#define _voFLACDecGetUnaryBits                         RENAME(voFLACDec00000007) 
#define _voFLACDecGetUtf8                              RENAME(voFLACDec00000008) 
#define _voFLACDecGetsBits                             RENAME(voFLACDec00000009) 
#define _voFLACDecHeaderDecode                         RENAME(voFLACDec00000010) 
#define _voFLACDecHeaderSync                           RENAME(voFLACDec00000011) 
#define _voFLACDecInitBits                             RENAME(voFLACDec00000012) 
#define _voFLACDecInitStreamBuf                        RENAME(voFLACDec00000013) 
#define _voFLACDecReadMetaInfo                         RENAME(voFLACDec00000014) 
#define _voFLACDecSkipBits                             RENAME(voFLACDec00000015) 
#define _voFLACDecStreamBufferUpdata                   RENAME(voFLACDec00000016) 
#define _voFLACDecStreamFinish                         RENAME(voFLACDec00000017) 
#define _voFLACDecStreamFlush                          RENAME(voFLACDec00000018) 
#define _voFLACDecStreamInit                           RENAME(voFLACDec00000019) 
#define _voFLACDecUpdateFrameBuffer                    RENAME(voFLACDec00000020) 
#define _voFLACDec_GetOutputData                       RENAME(voFLACDec00000021) 
#define _voFLACDec_GetParam                            RENAME(voFLACDec00000022) 
#define _voFLACDec_Init                                RENAME(voFLACDec00000023) 
#define _voFLACDec_SetInputData                        RENAME(voFLACDec00000024) 
#define _voFLACDec_SetParam                            RENAME(voFLACDec00000025) 
#define _voFLACDec_Uninit                              RENAME(voFLACDec00000026) 
//#define _voFLACDecmem_free                             RENAME(voFLACDec00000027) 
//#define _voFLACDecmem_malloc                           RENAME(voFLACDec00000028) 
#define _voFLACDecodeSubFrame                          RENAME(voFLACDec00000029)


#endif //#ifndef _DEBUG
#endif //#ifndef _WIN32_WCE
#endif //#ifndef __VOOGGDECID_H_